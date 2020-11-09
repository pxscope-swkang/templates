#include "catch.hpp"
#include <bitset>
#include <iomanip>
#include <iostream>
#include <kangsw/misc.hxx>
#include <kangsw/thread_pool.hxx>

using namespace kangsw;
using namespace std;

TEST_CASE("thread pool default operation", "[thread_pool]")
{
    printf("THREAD POOL TEST --- \n");
    size_t num_cases = 512;

    timer_thread_pool thr{1024, 1};
    thr.max_task_wait_time = 1ms;
    thr.num_max_workers(64);
    vector<pair<double, future<double>>> futures;
    futures.resize(num_cases);

    // multithreaded enqueing
    counter_range counter(num_cases);
    for_each(std::execution::par_unseq, counter.begin(), counter.end(), [&](size_t i) {
        futures[i] = make_pair(
          (double)i,
          thr.launch_timer(
               chrono::milliseconds(rand() % 200),
               [](double c) {
                   this_thread::sleep_for(chrono::milliseconds(rand() % 110));
                   return c * c;
               },
               (double)i)
            .get_future());
    });

    size_t num_error = 0;

    int index = 0;
    using chrono::system_clock;
    auto elapse_begin = system_clock::now();
    do {
        bool next_line = std::bitset<32>(++index).count() == 1;

        cout << setw(8) << chrono::duration_cast<chrono::milliseconds>(system_clock::now() - elapse_begin).count() << " ms "
             << ">> Threads (" << setw(4) << thr.num_workers()
             << ") Count [" << setw(6) << thr.num_total_waitings()
             << "] Avg Wait: "
             << chrono::duration<float>(thr.average_wait()).count()
             << (next_line ? "\n" : "\r");
        this_thread::sleep_for(33ms);
    } while (thr.num_total_waitings() > 0);

    for (auto& pair : futures) {
        num_error += pair.first * pair.first != pair.second.get();
    }

    CHECK(thr.num_workers() != 2);
    REQUIRE(num_error == 0);

    printf("\n");
}