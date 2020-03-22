#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "client.hpp"
#include "server.hpp"

#include "step_definitions.hpp"

constexpr std::string_view host = "localhost";
constexpr std::string_view port = "5001";

TEST_SUITE("Registering in a course") {
    University university;

    asio::io_context clerk_io_context;
    asio::io_context student_io_context;
    tcp::resolver clerk_resolver{clerk_io_context};
    tcp::resolver student_resolver{student_io_context};
    const auto clerk_endpoints   = clerk_resolver.resolve(host, port);
    const auto student_endpoints = student_resolver.resolve(host, port);

    asio::io_context server_io_context;
    tcp::endpoint server_endpoint(tcp::v4(), 5001);

    Client clerk{clerk_io_context, clerk_endpoints};
    Client student{student_io_context, student_endpoints};
    Server server{server_io_context, server_endpoint, university};

    std::thread server_thread;
    std::thread clerk_thread;
    std::thread student_thread;

    SCENARIO("Background") {
        GIVEN("The server is running") {
            server_thread  = std::thread([&] { server_io_context.run(); });
            clerk_thread   = std::thread([&] { clerk_io_context.run(); });
            student_thread = std::thread([&] { student_io_context.run(); });
        }

        GIVEN("The clerk is logged in") {
            clerk.send(Message("clerk"));
            clerk.send(Message("admin"));
        }

        GIVEN("There is an existing course 12345, Witchcraft, 1") {
            clerk.send(Message("cac"));
            clerk.send(Message("12345, Witchcraft, 1"));
        }

        GIVEN("There is an existing student 123456789, joe") {
            there_is_an_existing_student(clerk, "123456789, joe");
        }

        GIVEN("There is an existing student 123456788, murphy") {
            there_is_an_existing_student(clerk, "123456789, joe");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        GIVEN("The student has logged in as 123456789, joe") {
            student.send(Message("student"));
            student.send(Message("123456789, joe"));
        }
    }

    SCENARIO("A student registers in a course of the current term") {
        GIVEN("We fastforward 10 days.") {
            std::this_thread::sleep_for(std::chrono::seconds(DAY_LENGTH * 10));
        }

        GIVEN("The student enters register for course") {
            student.send(Message("rfc"));
        }

        WHEN("The student enters 12345") { student.send(Message("12345")); }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        THEN("The student 123456789 is registered in the course 12345") {
            CHECK(true ==
                  university.course(12345).value().has_student(123456789));
        }

        AND_THEN("Death time") {
            clerk_io_context.stop();
            student_io_context.stop();
            server_io_context.stop();

            clerk_thread.join();
            student_thread.join();
            server_thread.join();
        }
    }
}
