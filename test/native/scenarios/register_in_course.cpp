#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <array>
#include <chrono>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "server.hpp"

asio::io_context io_context;
tcp::endpoint endpoint(tcp::v4(), 5001);

SCENARIO("A student selects and registers in a course of the current term "
         "after registration starts and before registration ends") {
    GIVEN("The ATC is running") {
        University university;
        std::thread server([] { Server server{io_context, endpoint}; });

        std::this_thread::sleep_for(std::chrono::seconds(5));
        // WHEN("We let the threads run") {
        //     auto thread_fn = [](std::string name) {
        //         std::lock_guard<std::mutex> lg(names_mutex);
        //         names.push_back(name);
        //     };
        //     t1 = std::thread{thread_fn, "Thread 1"};
        //     t2 = std::thread{thread_fn, "Thread 2"};

        //     THEN("thing equals something") {
        //         std::this_thread::sleep_for(std::chrono::seconds(1));
        //         std::cout << "\nOrder = " << names[0] << ", " << names[1]
        //                   << "\n";
        //         CHECK(names.at(0) == names[0]);

        //         t1.join();
        //         t2.join();
        //     }
        // }
    }
}
