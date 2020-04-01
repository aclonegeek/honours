#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include "client.hpp"
#include "server.hpp"

#include "scenario_context.hpp"
#include "step_definitions.hpp"

class DeregisterFromCourseScenarioContext final : public ScenarioContext {
public:
    DeregisterFromCourseScenarioContext()
        : clerk_resolver(tcp::resolver(this->clerk_io_context)),
          joe_resolver(tcp::resolver(this->joe_io_context)),
          clerk_endpoints(this->clerk_resolver.resolve(host, port)),
          joe_endpoints(this->joe_resolver.resolve(host, port)),
          clerk(Client(this->clerk_io_context, this->clerk_endpoints)),
          _joe(Client(this->joe_io_context, this->joe_endpoints)) {
        this->clerk_thread = std::thread([&] { this->clerk_io_context.run(); });
        this->joe_thread   = std::thread([&] { this->joe_io_context.run(); });

        this->background();
    }

    ~DeregisterFromCourseScenarioContext() {
        this->clerk_io_context.stop();
        this->joe_io_context.stop();

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

    Client& joe() { return this->_joe; }

private:
    asio::io_context clerk_io_context;
    asio::io_context joe_io_context;

    tcp::resolver clerk_resolver;
    tcp::resolver joe_resolver;

    const tcp::resolver::results_type clerk_endpoints;
    const tcp::resolver::results_type joe_endpoints;

    std::thread clerk_thread;
    std::thread joe_thread;

    Client clerk;
    Client _joe;
};

TEST_SUITE_BEGIN("Deregistering from a course");

SCENARIO("A student deregisters from a course during the registration period") {
    DeregisterFromCourseScenarioContext ctx;
    Client& joe = ctx.joe();

    GIVEN("We wait until registration starts.") {
        wait(WaitUntil::REGISTRATION_STARTS);

        GIVEN("The student has registered in 12345") {
            the_student_has_registered_in(joe, "12345");

            std::this_thread::sleep_for(std::chrono::milliseconds(5));

            CHECK(true == ctx.university().course(12345).value().has_student(
                              123456789));

            GIVEN("The student enters dfc") {
                send(joe, "dfc");

                WHEN("The student enters 12345") {
                    send(joe, "12345");

                    std::this_thread::sleep_for(std::chrono::milliseconds(5));

                    THEN(
                        "The student 123456789 is not registered in the course "
                        "12345") {
                        CHECK(
                            false ==
                            ctx.university().course(12345).value().has_student(
                                123456789));
                    }
                }
            }
        }
    }
}

SCENARIO("A student deregisters from a course that doesn't exist") {
    DeregisterFromCourseScenarioContext ctx;
    Client& joe = ctx.joe();

    GIVEN("We wait until registration starts.") {
        wait(WaitUntil::REGISTRATION_STARTS);

        GIVEN("The student enters dfc") {
            send(joe, "dfc");

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

SCENARIO("A student deregisters from a course they aren't registered in") {
    DeregisterFromCourseScenarioContext ctx;
    Client& joe = ctx.joe();

    GIVEN("We wait until registration starts.") {
        wait(WaitUntil::REGISTRATION_STARTS);

        GIVEN("The student enters dfc") {
            send(joe, "dfc");

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
}

SCENARIO("A student deregisters from a course before registration starts") {
    DeregisterFromCourseScenarioContext ctx;
    Client& joe = ctx.joe();

    GIVEN("The student enters dfc") {
        send(joe, "dfc");

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

SCENARIO("A student deregisters from a course after registration ends") {
    DeregisterFromCourseScenarioContext ctx;
    Client& joe = ctx.joe();

    GIVEN("We wait until registration starts") {
        wait(WaitUntil::REGISTRATION_STARTS);

        GIVEN("The student has registered in 12345") {
            the_student_has_registered_in(joe, "12345");

            std::this_thread::sleep_for(std::chrono::milliseconds(5));

            CHECK(true == ctx.university().course(12345).value().has_student(
                              123456789));

            GIVEN("We wait until registration ends") {
                wait(WaitUntil::CUSTOM, REGISTRATION_LENGTH);

                GIVEN("The student enters dfc") {
                    send(joe, "dfc");

                    WHEN("The student enters 12345") {
                        send(joe, "12345");

                        std::this_thread::sleep_for(
                            std::chrono::milliseconds(5));

                        THEN("The student 123456789 is still registered in the "
                             "course "
                             "12345") {
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

SCENARIO("A student deregisters from a course after the term ends") {
    DeregisterFromCourseScenarioContext ctx;
    Client& joe = ctx.joe();

    GIVEN("We wait until registration starts") {
        wait(WaitUntil::REGISTRATION_STARTS);

        GIVEN("The student has registered in 12345") {
            the_student_has_registered_in(joe, "12345");

            std::this_thread::sleep_for(std::chrono::milliseconds(5));

            CHECK(true == ctx.university().course(12345).value().has_student(
                              123456789));

            GIVEN("We wait until the term ends") {
                wait(WaitUntil::CUSTOM, REGISTRATION_LENGTH + TERM_LENGTH);

                GIVEN("The student enters dfc") {
                    send(joe, "dfc");

                    WHEN("The student enters 12345") {
                        send(joe, "12345");

                        std::this_thread::sleep_for(
                            std::chrono::milliseconds(5));

                        THEN("The student 123456789 is still registered in the "
                             "course "
                             "12345") {
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
