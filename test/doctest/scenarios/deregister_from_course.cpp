#include <doctest.h>

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

        wait_for_clients_to_load();

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
        there_is_an_existing_course(this->clerk, "123456, Witchcraft, 1");
        there_is_an_existing_student(this->clerk, "123456789, joe");

        wait_for_action_to_finish();

        the_student_is_logged_in_as(this->_joe, "123456789, joe");
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

        GIVEN("The student has registered in 123456") {
            the_student_has_registered_in(joe, "123456");

            wait_for_action_to_finish();

            CHECK(true == ctx.university().course(123456).value().has_student(
                              123456789));

            GIVEN("The student enters dfc") {
                send(joe, "dfc");

                wait_for_action_to_finish();

                WHEN("The student enters 123456") {
                    send(joe, "123456");

                    wait_for_action_to_finish();

                    THEN("Deregistered from course. is printed") {
                        CHECK("Deregistered from course." ==
                              joe.previous_message());

                        AND_THEN("The student 123456789 is not registered in "
                                 "the course 123456") {
                            CHECK(false == ctx.university()
                                               .course(123456)
                                               .value()
                                               .has_student(123456789));
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("A student deregisters from a course with invalid input") {
    DeregisterFromCourseScenarioContext ctx;
    Client& joe = ctx.joe();

    GIVEN("The student enters dfc") {
        send(joe, "dfc");

        wait_for_action_to_finish();

        WHEN("The student enters quack") {
            send(joe, "quack");

            wait_for_action_to_finish();

            THEN("ERROR - Invalid input. Course ID must be a number. is "
                 "printed") {
                CHECK("ERROR - Invalid input. Course ID must be a number." ==
                      joe.previous_message());
            }
        }

        WHEN("The student enters 123456789123") {
            send(joe, "123456789123");

            wait_for_action_to_finish();

            THEN("ERROR - Invalid input. Course ID must be 6 digits. is "
                 "printed") {
                CHECK("ERROR - Invalid input. Course ID must be 6 digits." ==
                      joe.previous_message());
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

            wait_for_action_to_finish();

            WHEN("The student enters 999999") {
                send(joe, "999999");

                wait_for_action_to_finish();

                THEN("ERROR - Course does not exist. is printed") {
                    CHECK("ERROR - Course does not exist." ==
                          joe.previous_message());

                    AND_THEN("The course 999999 does not exist") {
                        CHECK(false ==
                              ctx.university().course(999999).has_value());
                    }
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

            wait_for_action_to_finish();

            WHEN("The student enters 123456") {
                send(joe, "123456");

                wait_for_action_to_finish();

                THEN(
                    "ERROR - Student is not registered in course. is printed") {
                    CHECK("ERROR - Student is not registered in course." ==
                          joe.previous_message());

                    AND_THEN(
                        "The student 123456789 is not registered in the course "
                        "123456") {
                        CHECK(
                            false ==
                            ctx.university().course(123456).value().has_student(
                                123456789));
                    }
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

        wait_for_action_to_finish();

        WHEN("The student enters 123456") {
            send(joe, "123456");

            wait_for_action_to_finish();

            THEN("ERROR - Can only deregister from a course during "
                 "registration. is printed") {
                CHECK("ERROR - Can only deregister from a course during "
                      "registration." == joe.previous_message());

                AND_THEN(
                    "The student 123456789 is not registered in the course "
                    "123456") {
                    CHECK(false ==
                          ctx.university().course(123456).value().has_student(
                              123456789));
                }
            }
        }
    }
}

SCENARIO("A student deregisters from a course after registration ends") {
    DeregisterFromCourseScenarioContext ctx;
    Client& joe = ctx.joe();

    GIVEN("We wait until registration starts") {
        wait(WaitUntil::REGISTRATION_STARTS);

        GIVEN("The student has registered in 123456") {
            the_student_has_registered_in(joe, "123456");

            wait_for_action_to_finish();

            CHECK(true == ctx.university().course(123456).value().has_student(
                              123456789));

            GIVEN("We wait until registration ends") {
                wait(WaitUntil::CUSTOM, REGISTRATION_LENGTH);

                GIVEN("The student enters dfc") {
                    send(joe, "dfc");

                    wait_for_action_to_finish();

                    WHEN("The student enters 123456") {
                        send(joe, "123456");

                        wait_for_action_to_finish();

                        THEN("ERROR - Can only deregister from a course during "
                             "registration. is printed") {
                            CHECK("ERROR - Can only deregister from a course "
                                  "during registration." ==
                                  joe.previous_message());

                            AND_THEN("The student 123456789 is still "
                                     "registered in the "
                                     "course 123456") {
                                CHECK(true == ctx.university()
                                                  .course(123456)
                                                  .value()
                                                  .has_student(123456789));
                            }
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

        GIVEN("The student has registered in 123456") {
            the_student_has_registered_in(joe, "123456");

            wait_for_action_to_finish();

            CHECK(true == ctx.university().course(123456).value().has_student(
                              123456789));

            GIVEN("We wait until the term ends") {
                wait(WaitUntil::CUSTOM, REGISTRATION_LENGTH + TERM_LENGTH);

                GIVEN("The student enters dfc") {
                    send(joe, "dfc");

                    wait_for_action_to_finish();

                    WHEN("The student enters 123456") {
                        send(joe, "123456");

                        wait_for_action_to_finish();

                        THEN("ERROR - Can only deregister from a course during "
                             "registration. is printed") {
                            CHECK("ERROR - Can only deregister from a course "
                                  "during registration." ==
                                  joe.previous_message());

                            AND_THEN("The student 123456789 is still "
                                     "registered in the course 123456") {
                                CHECK(true == ctx.university()
                                                  .course(123456)
                                                  .value()
                                                  .has_student(123456789));
                            }
                        }
                    }
                }
            }
        }
    }
}

TEST_SUITE_END();
