#include <doctest.h>

#include "scenario_context.hpp"
#include "step_definitions.hpp"

class RegisterInCourseScenarioContext final : public ScenarioContext {
public:
    RegisterInCourseScenarioContext()
        : clerk_resolver(tcp::resolver(this->clerk_io_context)),
          joe_resolver(tcp::resolver(this->joe_io_context)),
          murphy_resolver(tcp::resolver(this->murphy_io_context)),
          clerk_endpoints(this->clerk_resolver.resolve(host, port)),
          joe_endpoints(this->joe_resolver.resolve(host, port)),
          murphy_endpoints(this->murphy_resolver.resolve(host, port)),
          clerk(Client(this->clerk_io_context, this->clerk_endpoints)),
          _joe(Client(this->joe_io_context, this->joe_endpoints)),
          _murphy(Client(this->murphy_io_context, this->murphy_endpoints)) {
        this->clerk_thread = std::thread([&] { this->clerk_io_context.run(); });
        this->joe_thread   = std::thread([&] { this->joe_io_context.run(); });
        this->murphy_thread =
            std::thread([&] { this->murphy_io_context.run(); });

        this->background();
    }

    ~RegisterInCourseScenarioContext() {
        this->clerk_io_context.stop();
        this->joe_io_context.stop();
        this->murphy_io_context.stop();

        this->clerk_thread.join();
        this->joe_thread.join();
        this->murphy_thread.join();
    }

    void background() {
        the_clerk_is_logged_in(this->clerk);
        there_is_an_existing_course(this->clerk, "12345, Witchcraft, 1");
        there_is_an_existing_student(this->clerk, "123456789, joe");
        there_is_an_existing_student(this->clerk, "123456788, murphy");

        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        the_student_has_logged_in_as(this->_joe, "123456789, joe");
    }

    Client& joe() { return this->_joe; }
    Client& murphy() { return this->_murphy; }

private:
    asio::io_context clerk_io_context;
    asio::io_context joe_io_context;
    asio::io_context murphy_io_context;

    tcp::resolver clerk_resolver;
    tcp::resolver joe_resolver;
    tcp::resolver murphy_resolver;

    const tcp::resolver::results_type clerk_endpoints;
    const tcp::resolver::results_type joe_endpoints;
    const tcp::resolver::results_type murphy_endpoints;

    std::thread clerk_thread;
    std::thread joe_thread;
    std::thread murphy_thread;

    Client clerk;
    Client _joe;
    Client _murphy;
};

TEST_SUITE_BEGIN("Registering in a course");

// clang-format off
SCENARIO("A student registers in a course after registration starts and before registration ends") {
    // clang-format on
    RegisterInCourseScenarioContext ctx;
    Client& joe = ctx.joe();

    GIVEN("We wait until registration starts.") {
        wait(WaitUntil::REGISTRATION_STARTS);

        GIVEN("The student enters rfc") {
            send(joe, "rfc");

            WHEN("The student enters 12345") {
                send(joe, "12345");

                std::this_thread::sleep_for(std::chrono::milliseconds(5));

                THEN("Registered for course. is printed") {
                    CHECK("Registered for course." ==
                          std::string_view(joe.previous_message()));

                    AND_THEN("The student 123456789 is registered in the "
                             "course 12345") {
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

SCENARIO("A student registers in a course that doesn't exist") {
    RegisterInCourseScenarioContext ctx;
    Client& joe = ctx.joe();

    GIVEN("We wait until registration starts.") {
        wait(WaitUntil::REGISTRATION_STARTS);

        GIVEN("The student enters rfc") {
            send(joe, "rfc");

            WHEN("The student enters 2") {
                send(joe, "2");

                std::this_thread::sleep_for(std::chrono::milliseconds(5));

                THEN("Course does not exist. is printed") {
                    CHECK("Course does not exist." ==
                          std::string_view(joe.previous_message()));

                    AND_THEN("The course does not exist") {
                        CHECK(false == ctx.university().course(2).has_value());
                    }
                }
            }
        }
    }
}

SCENARIO("A student registers in a course before registration starts") {
    RegisterInCourseScenarioContext ctx;
    Client& joe = ctx.joe();

    GIVEN("The student enters rfc") {
        send(joe, "rfc");

        WHEN("The student enters 12345") {
            send(joe, "12345");

            std::this_thread::sleep_for(std::chrono::milliseconds(5));

            THEN("Registration has not started. is printed") {
                CHECK("Registration has not started." ==
                      std::string_view(joe.previous_message()));

                AND_THEN("The student 123456789 is not registered in 12345") {
                    CHECK(false ==
                          ctx.university().course(12345).value().has_student(
                              123456789));
                }
            }
        }
    }
}

SCENARIO("A student registers in a course after registration ended") {
    RegisterInCourseScenarioContext ctx;
    Client& joe = ctx.joe();

    GIVEN("We wait until registration ends.") {
        wait(WaitUntil::REGISTRATION_ENDS);

        GIVEN("The student enters rfc") {
            send(joe, "rfc");

            WHEN("The student enters 12345") {
                send(joe, "12345");

                std::this_thread::sleep_for(std::chrono::milliseconds(5));

                THEN("Registration has ended. is printed") {
                    CHECK("Registration has ended." ==
                          std::string_view(joe.previous_message()));

                    AND_THEN(
                        "The student 123456789 is not registered in 12345") {
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

SCENARIO("A student registers in a course after the term ended") {
    RegisterInCourseScenarioContext ctx;
    Client& joe = ctx.joe();

    GIVEN("We wait until the term ends.") {
        wait(WaitUntil::TERM_ENDS);

        GIVEN("The student enters rfc") {
            send(joe, "rfc");

            WHEN("The student enters 12345") {
                send(joe, "12345");

                std::this_thread::sleep_for(std::chrono::milliseconds(5));

                THEN("Registration has ended. is printed") {
                    CHECK("Registration has ended." ==
                          std::string_view(joe.previous_message()));

                    AND_THEN(
                        "The student 123456789 is not registered in 12345") {
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

// clang-format off
SCENARIO("A student registers in a course that reached its capsize before registration ends") {
    // clang-format on
    RegisterInCourseScenarioContext ctx;
    Client& joe    = ctx.joe();
    Client& murphy = ctx.murphy();

    GIVEN("We wait until registration starts.") {
        wait(WaitUntil::REGISTRATION_STARTS);

        GIVEN("The student enters rfc") {
            send(joe, "rfc");

            GIVEN("The student enters 12345") {
                send(joe, "12345");

                GIVEN("The student has logged in as 123456788, murphy") {
                    the_student_has_logged_in_as(ctx.murphy(),
                                                 "123456788, murphy");

                    GIVEN("The student enters rfc") {
                        send(murphy, "rfc");

                        WHEN("The student enters 12345") {
                            send(murphy, "12345");

                            std::this_thread::sleep_for(
                                std::chrono::milliseconds(5));

                            THEN("Course is full. is printed") {
                                CHECK("Course is full." ==
                                      std::string_view(
                                          murphy.previous_message()));

                                AND_THEN("The student 123456788 is not "
                                         "registered in "
                                         "12345") {
                                    CHECK(false == ctx.university()
                                                       .course(12345)
                                                       .value()
                                                       .has_student(123456788));

                                    AND_THEN("The student 123456789 is "
                                             "registered in 12345") {
                                        CHECK(true ==
                                              ctx.university()
                                                  .course(12345)
                                                  .value()
                                                  .has_student(123456789));

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
}

TEST_SUITE_END();
