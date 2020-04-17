#include <doctest.h>

#include "scenario_context.hpp"
#include "step_definitions.hpp"

class CreateCourseScenarioContext final : public ScenarioContext {
public:
    CreateCourseScenarioContext()
        : clerk_resolver(tcp::resolver(this->clerk_io_context)),
          clerk_endpoints(this->clerk_resolver.resolve(host, port)),
          _clerk(Client(this->clerk_io_context, this->clerk_endpoints)) {
        this->clerk_thread = std::thread([&] { this->clerk_io_context.run(); });

        wait_for_clients_to_load();

        this->background();
    }

    ~CreateCourseScenarioContext() {
        this->clerk_io_context.stop();
        this->clerk_thread.join();
    }

    void background() { the_clerk_is_logged_in(this->_clerk); }

    Client& clerk() { return this->_clerk; }

private:
    asio::io_context clerk_io_context;
    tcp::resolver clerk_resolver;
    const tcp::resolver::results_type clerk_endpoints;

    std::thread clerk_thread;

    Client _clerk;
};

TEST_SUITE_BEGIN("Creating a course");

SCENARIO("A clerk creates a course before registration starts") {
    CreateCourseScenarioContext ctx;
    Client& clerk = ctx.clerk();

    GIVEN("The clerk enters cac") {
        send(clerk, "cac");

        WHEN("The clerk enters 123456, Witchcraft, 1") {
            send(clerk, "123456, Witchcraft, 1");

            wait_for_action_to_finish();

            THEN("Course created. is printed") {
                CHECK("Course created." == clerk.previous_message());

                AND_THEN("The course 123456 exists") {
                    CHECK(true == ctx.university().course(123456).has_value());
                }
            }
        }
    }
}

SCENARIO("A clerk creates a duplicate course before registration starts") {
    CreateCourseScenarioContext ctx;
    Client& clerk = ctx.clerk();

    GIVEN("There is an existing course 123456, Witchcraft, 1") {
        there_is_an_existing_course(clerk, "123456, Witchcraft, 1");

        GIVEN("The clerk enters cac") {
            send(clerk, "cac");

            WHEN("The clerk enters 123456, Witchcraft, 1") {
                send(clerk, "123456, Witchcraft, 1");

                wait_for_action_to_finish();

                THEN("ERROR - Course already exists. is printed") {
                    CHECK("ERROR - Course already exists." ==
                          clerk.previous_message());

                    AND_THEN("The course 123456 exists") {
                        CHECK(true ==
                              ctx.university().course(123456).has_value());
                    }
                }
            }
        }
    }
}

SCENARIO("A clerk creates a course with invalid input") {
    CreateCourseScenarioContext ctx;
    Client& clerk = ctx.clerk();

    GIVEN("The clerk enters cac") {
        send(clerk, "cac");

        WHEN("The clerk enters quack, Witchcraft, 1") {
            send(clerk, "quack, Witchcraft, 1");

            wait_for_action_to_finish();

            THEN("ERROR - Invalid input. Course ID must be a number. is "
                 "printed") {
                CHECK("ERROR - Invalid input. Course ID must be a number." ==
                      clerk.previous_message());
            }
        }

        WHEN("The clerk enters 2, Witchcraft, 1") {
            send(clerk, "2, Witchcraft, 1");

            wait_for_action_to_finish();

            THEN("ERROR - Invalid input. Course ID must be 6 digits. is "
                 "printed") {
                CHECK("ERROR - Invalid input. Course ID must be 6 digits." ==
                      clerk.previous_message());
            }
        }

        WHEN("The clerk enters 123456, Witchcraft, quack") {
            send(clerk, "123456, Witchcraft, quack");

            wait_for_action_to_finish();

            THEN("ERROR - Invalid input. Capsize must be a number. is "
                 "printed") {
                CHECK("ERROR - Invalid input. Capsize must be a number." ==
                      clerk.previous_message());
            }
        }

        WHEN("The clerk enters 123456, Witchcraft, 300") {
            send(clerk, "123456, Witchcraft, 300");

            wait_for_action_to_finish();

            THEN("ERROR - Invalid input. Capsize must be between 1 and 255 "
                 "(inclusive). is printed") {
                CHECK("ERROR - Invalid input. Capsize must be between 1 and "
                      "255 (inclusive)." == clerk.previous_message());
            }
        }
    }
}

SCENARIO("A clerk creates a course after registration starts") {
    CreateCourseScenarioContext ctx;
    Client& clerk = ctx.clerk();

    GIVEN("We wait until registration starts") {
        wait(WaitUntil::REGISTRATION_STARTS);

        GIVEN("The clerk enters cac") {
            send(clerk, "cac");

            WHEN("The clerk enters 123456, Witchcraft, 1") {
                send(clerk, "123456, Witchcraft, 1");

                wait_for_action_to_finish();

                THEN("ERROR - Pregistration has ended. is printed") {
                    CHECK("ERROR - Preregistration has ended." ==
                          clerk.previous_message());

                    AND_THEN("The course 123456 does not exist") {
                        CHECK(false ==
                              ctx.university().course(123456).has_value());
                    }
                }
            }
        }
    }
}

SCENARIO("A clerk creates a course after the term ends") {
    CreateCourseScenarioContext ctx;
    Client& clerk = ctx.clerk();

    GIVEN("We wait until the term ends") {
        wait(WaitUntil::TERM_ENDS);

        GIVEN("The clerk enters cac") {
            send(clerk, "cac");

            WHEN("The clerk enters 123456, Witchcraft, 1") {
                send(clerk, "123456, Witchcraft, 1");

                wait_for_action_to_finish();

                THEN("ERROR - Pregistration has ended. is printed") {
                    CHECK("ERROR - Preregistration has ended." ==
                          clerk.previous_message());

                    AND_THEN("The course 123456 does not exist") {
                        CHECK(false ==
                              ctx.university().course(123456).has_value());
                    }
                }
            }
        }
    }
}

TEST_SUITE_END();
