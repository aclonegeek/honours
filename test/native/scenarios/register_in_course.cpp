#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <array>
#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

static std::vector<std::string> names;
static std::mutex names_mutex;

SCENARIO("Threads adding their names to a vector") {
    GIVEN("We have 2 threads running") {
        std::thread t1;
        std::thread t2;

        WHEN("We let the threads run") {
            auto thread_fn = [](std::string name) {
                std::lock_guard<std::mutex> lg(names_mutex);
                names.push_back(name);
            };
            t1 = std::thread{thread_fn, "Thread 1"};
            t2 = std::thread{thread_fn, "Thread 2"};

            THEN("thing equals something") {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::cout << "\nOrder = " << names[0] << ", " << names[1]
                          << "\n";
                CHECK(names.at(0) == names[0]);

                t1.join();
                t2.join();
            }
        }
    }
}
