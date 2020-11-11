#include <string>
#include <thread>
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"
#include "kangsw/atomic_queue.hxx"

namespace kangsw::container_test::queue {
TEST_CASE("Queue basic operations", "[lock_free_queue]")
{
    using kangsw::atomic_queue;
    using std::string;
    size_t num_case = 128;
    atomic_queue<string> queue{num_case};
    size_t num_fails = 0;

    REQUIRE(queue.empty());

    for (size_t i = 0; i < num_case; ++i) {
        bool push_successful = queue.try_push("Hell, world!");
        num_fails += !(push_successful);
        num_fails += !(queue.size() == i + 1);
    }

    REQUIRE(num_fails == 0);
    REQUIRE(queue.size() == num_case);

    num_fails = 0;
    for (int i = 0; i < num_case; ++i) {
        string v;
        bool pop_successful = queue.try_pop(v);
        num_fails += !(v == "Hell, world!");
        num_fails += !(pop_successful);
    }

    REQUIRE(num_fails == 0);

    {
        string v;
        REQUIRE(queue.try_pop(v) == false);
        REQUIRE(queue.empty());
    }
}

TEST_CASE("Queue async operations", "[lock_free_queue]")
{
    using kangsw::atomic_queue;
    using std::thread;
    using std::vector;
    using namespace std::chrono_literals;

    const size_t num_thr_wr = std::thread::hardware_concurrency() / 2;
    const size_t num_thr_rd = std::thread::hardware_concurrency() / 2;
    const size_t num_case = 16384;
    const size_t queue_capacity = 1024;

    vector<char> destinations;
    vector<thread> writers;
    vector<thread> readers;
    atomic_queue<size_t> queue{queue_capacity};

    destinations.resize(num_case * num_thr_wr);
    std::atomic_bool do_read = true;
    std::atomic_size_t read_count = 0;
    std::atomic_size_t num_wr_fail = 0;
    std::atomic_size_t num_rd_fail = 0;
    auto elapse_begin = std::chrono::system_clock::now();

    for (int i = 0; i < num_thr_wr; ++i) {
        writers.emplace_back([&, index = i]() {
            std::this_thread::sleep_for(100ms);
            for (int k = 0; k < num_case; ++k) {
                size_t idx = index * num_case + k;
                while (queue.try_push(idx) != true) {
                    ++num_wr_fail;
                }
            }
        });
    }

    for (int i = 0; i < num_thr_rd; ++i) {
        readers.emplace_back([&]() {
            while (queue.empty()) {}

            while (do_read) {
                size_t popped;
                if (queue.try_pop(popped)) {
                    destinations[popped] += 1;
                    ++read_count;
                }
                else if (queue.empty() != false) {
                    ++num_rd_fail;
                }

                if (read_count == destinations.size()) {
                    break;
                }
            }
        });
    }

    for (auto& thr : writers) { thr.join(); }
    for (auto& thr : readers) { thr.join(); }

    std::chrono::duration<double> elapsed = std::chrono::system_clock::now() - elapse_begin;
    INFO("Processing %llu element queue: %f seconds\n", num_case, elapsed.count());

    auto zero_cnt = std::count(destinations.begin(), destinations.end(), 0);
    auto not_one_count = std::count(destinations.begin(), destinations.end(), 1);
    INFO("rd_fail: " << num_rd_fail << ", wr_fail: " << num_wr_fail);
    CHECK(read_count == destinations.size());
    CHECK(zero_cnt == 0);
    CHECK((destinations.size() - not_one_count) == 0);
} // namespace kangsw::container_test::queue
} // namespace kangsw::container_test::queue