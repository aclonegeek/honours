#include "university.hpp"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <iostream>

#include "client.hpp"
#include "server.hpp"

#include "step_definitions.hpp"

class ScenarioContext {
public:
    ScenarioContext()
        : server_endpoint(tcp::endpoint(tcp::v4(), 5001)),
          server(Server(this->server_io_context, this->server_endpoint,
                        this->_university)),
          clerk_resolver(tcp::resolver(this->clerk_io_context)),
          joe_resolver(tcp::resolver(this->joe_io_context)),
          clerk_endpoints(this->clerk_resolver.resolve(host, port)),
          joe_endpoints(this->joe_resolver.resolve(host, port)),
          clerk(Client(this->clerk_io_context, this->clerk_endpoints)),
          _joe(Client(this->joe_io_context, this->joe_endpoints)) {
        this->server_thread =
            std::thread([&] { this->server_io_context.run(); });
        this->clerk_thread = std::thread([&] { this->clerk_io_context.run(); });
        this->joe_thread   = std::thread([&] { this->joe_io_context.run(); });
        this->background();
    }

    ~ScenarioContext() {
        this->clerk_io_context.stop();
        this->joe_io_context.stop();
        this->server_io_context.stop();

        this->server_thread.join();
        this->clerk_thread.join();
        this->joe_thread.join();
    }

    void background() {
        the_clerk_is_logged_in(this->clerk);
        there_is_an_existing_course(this->clerk, "12345, Witchcraft, 1");
        there_is_an_existing_student(this->clerk, "123456789, joe");

        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        the_student_has_logged_in_as(this->_joe, "123456789, joe");
    }

    const University& university() const { return this->_university; }
    Client& joe() { return this->_joe; }

private:
    asio::io_context server_io_context;
    asio::io_context clerk_io_context;
    asio::io_context joe_io_context;

    tcp::endpoint server_endpoint;
    tcp::resolver clerk_resolver;
    tcp::resolver joe_resolver;

    const tcp::resolver::results_type clerk_endpoints;
    const tcp::resolver::results_type joe_endpoints;

    std::thread server_thread;
    std::thread clerk_thread;
    std::thread joe_thread;

    University _university;

    Client clerk;
    Client _joe;
    Server server;
};

TEST_SUITE_BEGIN("Dropping a course");

SCENARIO("A student drops a course during the term") {
    ScenarioContext ctx;
    Client& joe = ctx.joe();

    GIVEN("We wait until registration starts.") {
        wait(WaitUntil::REGISTRATION_STARTS);

        GIVEN("The student has registered in 12345") {
            the_student_has_registered_in(joe, "12345");

            std::this_thread::sleep_for(std::chrono::milliseconds(5));

            CHECK(true == ctx.university().course(12345).value().has_student(
                              123456789));

            GIVEN("We wait until the term starts") {
                wait(WaitUntil::CUSTOM, REGISTRATION_LENGTH);

                GIVEN("The student enters dac") {
                    send(joe, "dac");

                    WHEN("The student enters 12345") {
                        send(joe, "12345");

                        std::this_thread::sleep_for(
                            std::chrono::milliseconds(5));

                        THEN("The student 123456789 is not registered in the "
                             "course "
                             "12345") {
                            CHECK(false == ctx.university()
                                               .course(12345)
                                               .value()
                                               .has_student(123456789));
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("A student drops from a course that doesn't exist") {
    ScenarioContext ctx;
    Client& joe = ctx.joe();

    GIVEN("We wait until the term starts.") {
        wait(WaitUntil::REGISTRATION_ENDS);

        GIVEN("The student enters dac") {
            send(joe, "dac");

            WHEN("The student enters 2") {
                send(joe, "2");

                std::this_thread::sleep_for(std::chrono::milliseconds(5));

                THEN("The course 2 does not exist") {
                    CHECK(false == ctx.university().course(2).has_value());
                }
            }
        }
    }
}

SCENARIO("A student drops a course during registration") {
    ScenarioContext ctx;
    Client& joe = ctx.joe();

    GIVEN("We wait until registration starts.") {
        wait(WaitUntil::REGISTRATION_STARTS);

        GIVEN("The student has registered in 12345") {
            the_student_has_registered_in(joe, "12345");

            std::this_thread::sleep_for(std::chrono::milliseconds(5));

            CHECK(true == ctx.university().course(12345).value().has_student(
                              123456789));

            GIVEN("The student enters dac") {
                send(joe, "dac");

                WHEN("The student enters 12345") {
                    send(joe, "12345");

                    std::this_thread::sleep_for(std::chrono::milliseconds(5));

                    THEN("The student 123456789 is registered in the course "
                         "12345") {
                        CHECK(
                            true ==
                            ctx.university().course(12345).value().has_student(
                                123456789));
                    }
                }
            }
        }
    }
}

SCENARIO("A student drops a course before registration has started") {
    ScenarioContext ctx;
    Client& joe = ctx.joe();

    GIVEN("The student enters dac") {
        send(joe, "dac");

        WHEN("The student enters 12345") {
            send(joe, "12345");

            std::this_thread::sleep_for(std::chrono::milliseconds(5));

            THEN("The student 123456789 is not registered in the course "
                 "12345") {
                CHECK(false ==
                      ctx.university().course(12345).value().has_student(
                          123456789));
            }
        }
    }
}

SCENARIO("A student drops a course after the term ends") {
    ScenarioContext ctx;
    Client& joe = ctx.joe();

    GIVEN("We wait until registration starts.") {
        wait(WaitUntil::REGISTRATION_STARTS);

        GIVEN("The student has registered in 12345") {
            the_student_has_registered_in(joe, "12345");

            std::this_thread::sleep_for(std::chrono::milliseconds(5));

            CHECK(true == ctx.university().course(12345).value().has_student(
                              123456789));

            GIVEN("We wait until the term ends") {
                wait(WaitUntil::CUSTOM, REGISTRATION_LENGTH + TERM_LENGTH);

                GIVEN("The student enters dac") {
                    send(joe, "dac");

                    WHEN("The student enters 12345") {
                        send(joe, "12345");

                        std::this_thread::sleep_for(
                            std::chrono::milliseconds(5));

                        THEN("The student 123456789 is registered in the "
                             "course 12345") {
                            CHECK(true == ctx.university()
                                              .course(12345)
                                              .value()
                                              .has_student(123456789));
                        }
                    }
                }
            }
        }
    }
}

TEST_SUITE_END();
