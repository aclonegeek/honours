#include "university.hpp"
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

class ScenarioContext {
public:
    ScenarioContext()
        : server_endpoint(tcp::endpoint(tcp::v4(), 5001)),
          server(Server(this->server_io_context, this->server_endpoint,
                        this->_university)),
          clerk_resolver(tcp::resolver(this->clerk_io_context)),
          student_resolver(tcp::resolver(this->student_io_context)),
          clerk_endpoints(this->clerk_resolver.resolve(host, port)),
          student_endpoints(this->student_resolver.resolve(host, port)),
          clerk(Client(this->clerk_io_context, this->clerk_endpoints)),
          _student(Client(this->student_io_context, this->student_endpoints)) {
        std::cout << "\nCTOR\n";

        this->server_thread =
            std::thread([&] { this->server_io_context.run(); });
        this->clerk_thread = std::thread([&] { this->clerk_io_context.run(); });
        this->student_thread =
            std::thread([&] { this->student_io_context.run(); });

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        this->background();
    }

    ~ScenarioContext() {
        std::cout << "\nDTOR\n";

        this->clerk_io_context.stop();
        this->student_io_context.stop();
        this->server_io_context.stop();

        this->server_thread.join();
        this->clerk_thread.join();
        this->student_thread.join();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void background() {
        the_clerk_is_logged_in(this->clerk);
        there_is_an_existing_course(this->clerk, "12345, Witchcraft, 1");
        there_is_an_existing_student(this->clerk, "123456789, joe");

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        the_student_has_logged_in_as(this->_student, "123456789, joe");
    }

    const University& university() const { return this->_university; }
    Client& student() { return this->_student; }

private:
    asio::io_context clerk_io_context;
    asio::io_context student_io_context;
    asio::io_context server_io_context;

    tcp::resolver clerk_resolver;
    tcp::resolver student_resolver;
    tcp::endpoint server_endpoint;

    tcp::resolver::results_type clerk_endpoints;
    tcp::resolver::results_type student_endpoints;

    std::thread server_thread;
    std::thread clerk_thread;
    std::thread student_thread;

    University _university;

    Client clerk;
    Client _student;
    Server server;
};

TEST_SUITE_BEGIN("Registering in a course");

// clang-format off
SCENARIO("A student registers in a course after registration starts and before registration ends") {
    std::cout << "\nA student registers in a course after registration starts and before registration ends\n";
    // clang-format on
    ScenarioContext ctx;

    GIVEN("We fastforward 10 days.") {
        std::this_thread::sleep_for(std::chrono::seconds(DAY_LENGTH * 10));

        GIVEN("The student enters rfc") {
            ctx.student().send(Message("rfc"));

            WHEN("The student enters 12345") {
                ctx.student().send(Message("12345"));

                std::this_thread::sleep_for(std::chrono::milliseconds(10));

                THEN(
                    "The student 123456789 is registered in the course 12345") {
                    CHECK(true ==
                          ctx.university().course(12345).value().has_student(
                              123456789));
                }
            }
        }
    }
}

// clang-format off
SCENARIO("A student registers in a course that doesn't exist") {
    std::cout << "\nA student registers in a course that doesn't exist\n";
    // clang-format on
    ScenarioContext ctx;

    GIVEN("We fastforward 10 days.") {
        std::this_thread::sleep_for(std::chrono::seconds(DAY_LENGTH * 10));

        GIVEN("The student enters rfc") {
            ctx.student().send(Message("rfc"));

            WHEN("The student enters 2") {
                ctx.student().send(Message("2"));

                std::this_thread::sleep_for(std::chrono::milliseconds(10));

                THEN("The course does not exist") {
                    CHECK(false == ctx.university().course(2).has_value());
                }
            }
        }
    }
}

// clang-format off
SCENARIO("A student registers in a course before registration starts") {
    // clang-format on
    ScenarioContext ctx;
    std::cout << "\nOI2: "
              << ctx.university().course(12345).value().has_student(123456789)
              << "\n";

    GIVEN("The student enters rfc") {
        ctx.student().send(Message("rfc"));

        WHEN("The student enters 12345") {
            ctx.student().send(Message("12345"));

            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            THEN("The student 123456789 is not registered in 12345") {
                CHECK(false ==
                      ctx.university().course(12345).value().has_student(
                          123456789));
            }
        }
    }
}

TEST_SUITE_END();
