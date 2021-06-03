/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <ki6080@gmail.com> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.      Seungwoo Kang.
 * ----------------------------------------------------------------------------
 */
#include <array>
#include <iomanip>
#include <iostream>
#include <kangsw/helpers/misc.hxx>
#include <kangsw/thread/thread_pool.hxx>
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"

using namespace kangsw;
using namespace std;
namespace kangsw::thread_pool_test {
TEST_CASE("thread pool default operation", "[.]") {
    constexpr int NUM_CASE = 1024;
    printf("<< THREAD POOL TEST >>");
    for (int ITER = 1; ITER; ITER--) {
        printf("\n [%4d] -------------------------------- \n", ITER);

        timer_thread_pool thr{32, 1};
        thr.max_task_interval_time = 14ms;
        thr.max_task_wait_time = 100ms;
        thr.max_stall_interval_time = 1022322ms;
        thr.average_weight = 10;
        static array<pair<double, std::shared_ptr<future_proxy<double>>>, NUM_CASE> futures;
        static array<char, NUM_CASE> executed_list;
        memset(executed_list.data(), 0, sizeof executed_list);

        atomic_int fill_index = 0;
        static array<chrono::system_clock::time_point, NUM_CASE> finish_time;
        auto pivot_time = chrono::system_clock::now();

        thr.num_max_workers(256);

        // multithreaded enqueing
        iota counter(NUM_CASE);
        for_each(std::execution::par_unseq, counter.begin(), counter.end(), [&](size_t i) {
            auto exec_time = chrono::system_clock::now() + chrono::milliseconds(rand() % 1200);
            futures[i] = make_pair(
              (double)i,
              thr.add_timer(
                   exec_time, [&, at_exec = exec_time](double c) {
                       this_thread::sleep_for(chrono::milliseconds(rand() % 43));
                       finish_time[fill_index++] = chrono::system_clock::time_point(at_exec - pivot_time);
                       executed_list[static_cast<size_t>(c + 0.5)] = 1;
                       return (int)(c + 0.5);
                   },
                   (double)i)
                ->then([&](int c) {
                    this_thread::sleep_for(chrono::milliseconds(rand() % 14));
                    executed_list[static_cast<size_t>(c + 0.5)] = 2;
                    return (double)c;
                })
                ->then([i]() {
                    // this_thread::sleep_for(chrono::milliseconds(rand() % 16));
                    executed_list[static_cast<size_t>(i + 0.5)] = 3;
                    return (double)i * i;
                }));
        });

        size_t num_error = 0;

        int index = 0;
        using chrono::system_clock;
        auto elapse_begin = system_clock::now();
        int out_count = 0;
        do {
            auto log_value = log(++index) / log(5);
            bool next_line = abs(log_value - round(log_value)) < 1e-6;

            int st[4] = {};
            for (auto& el : executed_list) {
                ++st[el];
            }

            cout << setw(8) << chrono::duration_cast<chrono::milliseconds>(system_clock::now() - elapse_begin).count() << " ms "
                 << ">> Threads (" << setw(4) << thr.num_workers()
                 << ") Count [" << setw(6) << thr.num_total_waitings()
                 << kangsw::format(" (%4d/%4d/%4d/%4d)", st[0], st[1], st[2], st[3])
                 << "] Avg Wait: " << setprecision(4) << setw(8) << fixed
                 << chrono::duration<double>(thr.average_wait()).count() * 1000 << " ms"
                 << (next_line || out_count == 1 ? "\n" : "\r");
            this_thread::sleep_for(33ms);
        } while ((out_count += thr.num_total_waitings() == 0) < 7);

        string display = "Unhandled: {";
        for (auto& l : executed_list) {
            if (l == 0) {
                display += to_string(&l - executed_list.data()) + ", ";
            }
        }
        display += "}";
        INFO(display);
        CHECK(std::count(executed_list.begin(), executed_list.end(), 3) == NUM_CASE);
        CHECK(finish_time.back() > finish_time.front());

        for (auto& pair : futures) {
            num_error += pair.first * pair.first != pair.second->get();
        }

        CHECK(thr.num_workers() != 2);
        REQUIRE(num_error == 0);

        cout << "\033F";
    }

    cout << '\n';
}

TEST_CASE("Timer accuracy test", "[.]") {
    using namespace std::chrono;

    static constexpr int NUM_CASE = 5120;
    timer_thread_pool workers;

    vector<system_clock::duration> errors{NUM_CASE};

    std::for_each(
      std::execution::par_unseq, errors.begin(), errors.end(),
      [&](system_clock::duration& error_dest) {
          auto delay = 500us * (rand() % 1000) + (1us * rand() % 500) + 10ms;
          auto issue = system_clock::now();

          /*
          std::thread([&error_dest, delay, issue]() {
              std::this_thread::sleep_until(issue + delay);
              error_dest = (system_clock::now() - issue) - delay;
          }).detach();
          /*/
          workers.add_timer(delay, [&error_dest, delay, issue]() {
              error_dest = (system_clock::now() - issue) - delay;
          });
          //*/
          // std::this_thread::sleep_for(1us);
      });

    while (workers.num_total_waitings() != 0 && workers.num_available_workers() != 0) {
        std::this_thread::sleep_for(200ms);
    }
    std::this_thread::sleep_for(1000ms);

    std::sort(errors.begin(), errors.end());
    auto percent_5 = NUM_CASE * 5 / 100;
    auto avg_err = std::reduce(errors.begin(), errors.end()) / NUM_CASE;
    auto min_v = std::reduce(errors.begin(), errors.begin() + percent_5) / percent_5;
    auto max_v = std::reduce(errors.end() - percent_5, errors.end()) / percent_5;

    auto to_micro = [](auto value) { return duration<double, std::micro>(value).count(); };
    auto min_mult_by_N = (to_micro(min_v) * NUM_CASE);
    auto max_div_by_N = to_micro(max_v) / NUM_CASE;
    WARN("Num Case         : " << NUM_CASE);
    WARN("Average Error Is : " << to_micro(avg_err) << " us");
    WARN("Min Error        : " << to_micro(min_v) << " us");
    WARN("Max Div by N     : " << max_div_by_N << " us");
    WARN("Average Wait     : " << to_micro(workers.average_wait()) << " us");
    WARN("Max Error        : " << to_micro(max_v) << " us");
    WARN("Min Mult by N    : " << min_mult_by_N << " us");
    WARN("5% Samples       : " << percent_5);
    WARN("Num Workers      : " << workers.num_workers());
    REQUIRE(to_micro(min_v) < 500);
}
} // namespace kangsw::thread_pool_test