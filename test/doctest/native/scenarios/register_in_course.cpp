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
          joe_resolver(tcp::resolver(this->joe_io_context)),
          murphy_resolver(tcp::resolver(this->murphy_io_context)),
          clerk_endpoints(this->clerk_resolver.resolve(host, port)),
          joe_endpoints(this->joe_resolver.resolve(host, port)),
          murphy_endpoints(this->murphy_resolver.resolve(host, port)),
          clerk(Client(this->clerk_io_context, this->clerk_endpoints)),
          _joe(Client(this->joe_io_context, this->joe_endpoints)),
          _murphy(Client(this->murphy_io_context, this->murphy_endpoints)) {
        this->server_thread =
            std::thread([&] { this->server_io_context.run(); });
        this->clerk_thread = std::thread([&] { this->clerk_io_context.run(); });
        this->joe_thread   = std::thread([&] { this->joe_io_context.run(); });
        this->murphy_thread =
            std::thread([&] { this->murphy_io_context.run(); });

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        this->background();
    }

    ~ScenarioContext() {
        this->clerk_io_context.stop();
        this->joe_io_context.stop();
        this->murphy_io_context.stop();
        this->server_io_context.stop();

        this->server_thread.join();
        this->clerk_thread.join();
        this->joe_thread.join();
        this->murphy_thread.join();
    }

    void background() {
        the_clerk_is_logged_in(this->clerk);
        there_is_an_existing_course(this->clerk, "12345, Witchcraft, 1");
        there_is_an_existing_student(this->clerk, "123456789, joe");
        there_is_an_existing_student(this->clerk, "123456788, murphy");

        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        the_student_has_logged_in_as(this->_joe, "123456789, joe");
    }

    const University& university() const { return this->_university; }
    Client& joe() { return this->_joe; }
    Client& murphy() { return this->_murphy; }

private:
    asio::io_context server_io_context;
    asio::io_context clerk_io_context;
    asio::io_context joe_io_context;
    asio::io_context murphy_io_context;

    tcp::endpoint server_endpoint;
    tcp::resolver clerk_resolver;
    tcp::resolver joe_resolver;
    tcp::resolver murphy_resolver;

    const tcp::resolver::results_type clerk_endpoints;
    const tcp::resolver::results_type joe_endpoints;
    const tcp::resolver::results_type murphy_endpoints;

    std::thread server_thread;
    std::thread clerk_thread;
    std::thread joe_thread;
    std::thread murphy_thread;

    University _university;

    Client clerk;
    Client _joe;
    Client _murphy;
    Server server;
};

TEST_SUITE_BEGIN("Registering in a course");

// clang-format off
SCENARIO("A student registers in a course after registration starts and before registration ends") {
    // clang-format on
    ScenarioContext ctx;

    GIVEN("We fastforward 10 days.") {
        std::this_thread::sleep_for(std::chrono::seconds(DAY_LENGTH * 10));

        GIVEN("The student enters rfc") {
            ctx.joe().send(Message("rfc"));

            WHEN("The student enters 12345") {
                ctx.joe().send(Message("12345"));

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

SCENARIO("A student registers in a course that doesn't exist") {
    ScenarioContext ctx;

    GIVEN("We fastforward 10 days.") {
        std::this_thread::sleep_for(std::chrono::seconds(DAY_LENGTH * 10));

        GIVEN("The student enters rfc") {
            ctx.joe().send(Message("rfc"));

            WHEN("The student enters 2") {
                ctx.joe().send(Message("2"));

                std::this_thread::sleep_for(std::chrono::milliseconds(10));

                THEN("The course does not exist") {
                    CHECK(false == ctx.university().course(2).has_value());
                }
            }
        }
    }
}

SCENARIO("A student registers in a course before registration starts") {
    ScenarioContext ctx;

    GIVEN("The student enters rfc") {
        ctx.joe().send(Message("rfc"));

        WHEN("The student enters 12345") {
            ctx.joe().send(Message("12345"));

            std::this_thread::sleep_for(std::chrono::milliseconds(10));

            THEN("The student 123456789 is not registered in 12345") {
                CHECK(false ==
                      ctx.university().course(12345).value().has_student(
                          123456789));
            }
        }
    }
}

SCENARIO("A student registers in a course after registration ended") {
    ScenarioContext ctx;

    GIVEN("We fastforward 25 days.") {
        std::this_thread::sleep_for(std::chrono::seconds(DAY_LENGTH * 25));

        GIVEN("The student enters rfc") {
            ctx.joe().send(Message("rfc"));

            WHEN("The student enters 12345") {
                ctx.joe().send(Message("12345"));

                std::this_thread::sleep_for(std::chrono::milliseconds(10));

                THEN("The student 123456789 is not registered in 12345") {
                    CHECK(false ==
                          ctx.university().course(12345).value().has_student(
                              123456789));
                }
            }
        }
    }
}

SCENARIO("A student registers in a course after the term ended") {
    ScenarioContext ctx;

    GIVEN("We fastforward 115 days.") {
        std::this_thread::sleep_for(std::chrono::seconds(DAY_LENGTH * 115));

        GIVEN("The student enters rfc") {
            ctx.joe().send(Message("rfc"));

            WHEN("The student enters 12345") {
                ctx.joe().send(Message("12345"));

                std::this_thread::sleep_for(std::chrono::milliseconds(10));

                THEN("The student 123456789 is not registered in 12345") {
                    CHECK(false ==
                          ctx.university().course(12345).value().has_student(
                              123456789));
                }
            }
        }
    }
}

// clang-format off
SCENARIO("A student registers in a course that reached its capsize before registration ends") {
    // clang-format on
    ScenarioContext ctx;

    GIVEN("We fastforward 10 days.") {
        std::this_thread::sleep_for(std::chrono::seconds(DAY_LENGTH * 10));

        GIVEN("The student enters rfc") {
            ctx.joe().send(Message("rfc"));

            GIVEN("The student enters 12345") {
                ctx.joe().send(Message("12345"));

                GIVEN("The student has logged in as 123456788, murphy") {
                    the_student_has_logged_in_as(ctx.murphy(),
                                                 "123456788, murphy");

                    GIVEN("The student enters rfc") {
                        ctx.murphy().send(Message("rfc"));

                        WHEN("The student enters 12345") {
                            ctx.murphy().send(Message("12345"));

                            THEN("The student 123456788 is not registered in "
                                 "12345") {
                                std::this_thread::sleep_for(
                                    std::chrono::milliseconds(10));

                                THEN("The student 123456789 is not registered "
                                     "in 12345") {
                                    CHECK(false == ctx.university()
                                                       .course(12345)
                                                       .value()
                                                       .has_student(123456788));

                                    AND_THEN("The course 12345 is full") {
                                        CHECK(true == ctx.university()
                                                          .course(12345)
                                                          .value()
                                                          .is_full());
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

TEST_SUITE_END();
