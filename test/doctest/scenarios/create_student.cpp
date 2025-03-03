#include <doctest.h>

#include "scenario_context.hpp"
#include "step_definitions.hpp"

class CreateStudentScenarioContext final : public ScenarioContext {
public:
    CreateStudentScenarioContext()
        : clerk_resolver(tcp::resolver(this->clerk_io_context)),
          clerk_endpoints(this->clerk_resolver.resolve(host, port)),
          _clerk(Client(this->clerk_io_context, this->clerk_endpoints)) {
        this->clerk_thread = std::thread([&] { this->clerk_io_context.run(); });

        wait_for_clients_to_load();

        this->background();
    }

    ~CreateStudentScenarioContext() {
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

TEST_SUITE_BEGIN("Creating a student");

SCENARIO("A clerk creates a student before registration starts") {
    CreateStudentScenarioContext ctx;
    Client& clerk = ctx.clerk();

    GIVEN("The clerk enters cas") {
        send(clerk, "cas");

        WHEN("The clerk enters 123456789, joe") {
            send(clerk, "123456789, joe");

            wait_for_action_to_finish();

            THEN("Student created. is printed") {
                CHECK("Student created." == clerk.previous_message());

                AND_THEN("The student 123456789 exists") {
                    CHECK(true ==
                          ctx.university().student(123456789).has_value());
                }
            }
        }
    }
}

SCENARIO("A clerk creates a duplicate student before registration starts") {
    CreateStudentScenarioContext ctx;
    Client& clerk = ctx.clerk();

    GIVEN("There is an existing student 123456789, joe") {
        there_is_an_existing_student(clerk, "123456789, joe");

        GIVEN("The clerk enters cas") {
            send(clerk, "cas");

            WHEN("The clerk enters 123456789, joe") {
                send(clerk, "123456789, joe");

                wait_for_action_to_finish();

                THEN("ERROR - Student already exists. is printed") {
                    CHECK("ERROR - Student already exists." ==
                          clerk.previous_message());
                }
            }
        }
    }
}

SCENARIO("A clerk creates a student with invalid input") {
    CreateStudentScenarioContext ctx;
    Client& clerk = ctx.clerk();

    GIVEN("The clerk enters cas") {
        send(clerk, "cas");

        WHEN("The clerk enters joe") {
            send(clerk, "joe");

            wait_for_action_to_finish();

            THEN("ERROR - Invalid input. Expected student ID (9 digits) and name. is "
                 "printed") {
                CHECK("ERROR - Invalid input. Expected student ID (9 digits) and name." ==
                      clerk.previous_message());
            }
        }

        WHEN("The clerk enters quack, joe") {
            send(clerk, "quack, joe");

            wait_for_action_to_finish();

            THEN("ERROR - Invalid input. Student ID must be a number. is "
                 "printed") {
                CHECK("ERROR - Invalid input. Student ID must be a number." ==
                      clerk.previous_message());
            }
        }

        WHEN("The clerk enters 123, joe") {
            send(clerk, "123, joe");

            wait_for_action_to_finish();

            THEN("ERROR - Invalid input. Student ID must be 9 digits. is "
                 "printed") {
                CHECK("ERROR - Invalid input. Student ID must be 9 digits." ==
                      clerk.previous_message());
            }
        }
    }
}

SCENARIO("A clerk creates a student after registration starts") {
    CreateStudentScenarioContext ctx;
    Client& clerk = ctx.clerk();

    GIVEN("We wait until registration starts") {
        wait(WaitUntil::REGISTRATION_STARTS);

        GIVEN("The clerk enters cas") {
            send(clerk, "cas");

            WHEN("The clerk enters 123456789, joe") {
                send(clerk, "123456789, joe");

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
