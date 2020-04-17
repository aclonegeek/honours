#include <doctest.h>

#include "scenario_context.hpp"
#include "step_definitions.hpp"

class DeleteStudentScenarioContext final : public ScenarioContext {
public:
    DeleteStudentScenarioContext()
        : clerk_resolver(tcp::resolver(this->clerk_io_context)),
          clerk_endpoints(this->clerk_resolver.resolve(host, port)),
          _clerk(Client(this->clerk_io_context, this->clerk_endpoints)) {
        this->clerk_thread = std::thread([&] { this->clerk_io_context.run(); });

        wait_for_clients_to_load();

        this->background();
    }

    ~DeleteStudentScenarioContext() {
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

TEST_SUITE_BEGIN("Deleting a student");

SCENARIO("A clerk deletes a student before registration starts") {
    DeleteStudentScenarioContext ctx;
    Client& clerk = ctx.clerk();

    GIVEN("There is an existing student 123456789, joe") {
        there_is_an_existing_student(clerk, "123456789, joe");

        GIVEN("The clerk enters das") {
            send(clerk, "das");

            wait_for_action_to_finish();

            WHEN("The clerk enters 123456789") {
                send(clerk, "123456789");

                wait_for_action_to_finish();

                THEN("Student deleted. is printed") {
                    CHECK("Student deleted." == clerk.previous_message());

                    AND_THEN("The student 123456789 does not exist") {
                        CHECK(false ==
                              ctx.university().student(123456789).has_value());
                    }
                }
            }
        }
    }
}

SCENARIO("A clerk deletes a non-existant student before registration starts") {
    DeleteStudentScenarioContext ctx;
    Client& clerk = ctx.clerk();

    GIVEN("The clerk enters das") {
        send(clerk, "das");

        wait_for_action_to_finish();

        WHEN("The clerk enters 123456789") {
            send(clerk, "123456789");

            wait_for_action_to_finish();

            THEN("ERROR - Student does not exist. is printed") {
                CHECK("ERROR - Student does not exist." ==
                      clerk.previous_message());
            }
        }
    }
}

SCENARIO("A clerk deletes a student with invalid input") {
    DeleteStudentScenarioContext ctx;
    Client& clerk = ctx.clerk();

    GIVEN("The clerk enters das") {
        send(clerk, "das");

        wait_for_action_to_finish();

        WHEN("The clerk enters quack") {
            send(clerk, "quack");

            wait_for_action_to_finish();

            THEN("ERROR - Invalid input. Student ID must be a number. is "
                 "printed") {
                CHECK("ERROR - Invalid input. Student ID must be a number." ==
                      clerk.previous_message());
            }
        }

        WHEN("The clerk enters 123") {
            send(clerk, "123");

            wait_for_action_to_finish();

            THEN("ERROR - Invalid input. Student ID must be 9 digits. is "
                 "printed") {
                CHECK("ERROR - Invalid input. Student ID must be 9 digits." ==
                      clerk.previous_message());
            }
        }
    }
}

SCENARIO("A clerk deletes a student after registration starts") {
    DeleteStudentScenarioContext ctx;
    Client& clerk = ctx.clerk();

    GIVEN("We wait until registration starts") {
        wait(WaitUntil::REGISTRATION_STARTS);

        GIVEN("The clerk enters das") {
            send(clerk, "das");

            wait_for_action_to_finish();

            WHEN("The clerk enters 123456789") {
                send(clerk, "123456789");

                wait_for_action_to_finish();

                THEN("ERROR - Preregistration has ended. is printed") {
                    CHECK("ERROR - Preregistration has ended." ==
                          clerk.previous_message());
                }
            }
        }
    }
}

TEST_SUITE_END();
