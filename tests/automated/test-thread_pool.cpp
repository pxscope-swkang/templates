#include "catch.hpp"
#include <array>
#include <iomanip>
#include <iostream>
#include <kangsw/misc.hxx>
#include <kangsw/thread_pool.hxx>

using namespace kangsw;
using namespace std;

TEST_CASE("thread pool default operation", "[thread_pool]")
{
    for (int ITER = 50; --ITER;) {
        printf("THREAD POOL TEST [%4d] --- \n", ITER);
        enum { num_cases = 1024 };

        timer_thread_pool thr{1024, 1};
        thr.max_task_wait_time = 2ms;
        static array<pair<double, future_proxy<double>>, num_cases> futures;
        static array<char, num_cases> executed_list;
        memset(executed_list.data(), 0, sizeof executed_list);

        atomic_int fill_index = 0;
        static array<chrono::system_clock::time_point, num_cases> finish_time;
        auto pivot_time = chrono::system_clock::now();

        // multithreaded enqueing
        counter_range counter(num_cases);
        for_each(std::execution::par_unseq, counter.begin(), counter.end(), [&](size_t i) {
            auto exec_time = chrono::system_clock::now() + chrono::milliseconds(rand() % 200);
            futures[i] = make_pair(
              (double)i,
              thr.launch_timer(
                exec_time,
                [&, at_exec = exec_time](double c) {
                    this_thread::sleep_for(chrono::milliseconds(rand() % 110));
                    executed_list[static_cast<size_t>(c + 0.5)] = 1;
                    finish_time[fill_index++] = chrono::system_clock::time_point(at_exec - pivot_time);
                    return c * c;
                },
                (double)i));
        });

        size_t num_error = 0;

        int index = 0;
        using chrono::system_clock;
        auto elapse_begin = system_clock::now();
        int out_count = 0;
        do {
            auto log_value = log(++index) / log(5);
            bool next_line = abs(log_value - round(log_value)) < 1e-6;

            cout << setw(8) << chrono::duration_cast<chrono::milliseconds>(system_clock::now() - elapse_begin).count() << " ms "
                 << ">> Threads (" << setw(4) << thr.num_workers()
                 << ") Count [" << setw(6) << thr.num_total_waitings()
                 << "] Avg Wait: "
                 << chrono::duration<float>(thr.average_wait()).count()
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
        CHECK(std::count(executed_list.begin(), executed_list.end(), 0) == 0);
        CHECK(finish_time.back() > finish_time.front());

        for (auto& pair : futures) {
            num_error += pair.first * pair.first != pair.second.get();
        }

        CHECK(thr.num_workers() != 2);
        REQUIRE(num_error == 0);

        printf("\n");
    }
}