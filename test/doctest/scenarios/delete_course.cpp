#include <doctest.h>

#include "scenario_context.hpp"
#include "step_definitions.hpp"

class DeleteCourseScenarioContext final : public ScenarioContext {
public:
    DeleteCourseScenarioContext()
        : clerk_resolver(tcp::resolver(this->clerk_io_context)),
          clerk_endpoints(this->clerk_resolver.resolve(host, port)),
          _clerk(Client(this->clerk_io_context, this->clerk_endpoints)) {
        this->clerk_thread = std::thread([&] { this->clerk_io_context.run(); });

        wait_for_clients_to_load();

        this->background();
    }

    ~DeleteCourseScenarioContext() {
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

TEST_SUITE_BEGIN("Deleting a course");

SCENARIO("A clerk deletes a course before registration starts") {
    DeleteCourseScenarioContext ctx;
    Client& clerk = ctx.clerk();

    GIVEN("There is an existing course 12345, Witchcraft, 1") {
        there_is_an_existing_course(clerk, "12345, Witchcraft, 1");

        GIVEN("The clerk enters dac") {
            send(clerk, "dac");

            WHEN("The clerk enters 12345") {
                send(clerk, "12345");

                wait_for_action_to_finish();

                THEN("Then Course deleted. is printed") {
                    CHECK("Course deleted." == clerk.previous_message());

                    AND_THEN("The course 12345 does not exist") {
                        CHECK(false ==
                              ctx.university().course(12345).has_value());
                    }
                }
            }
        }
    }
}

SCENARIO("A clerk deletes a course that doesn't exist") {
    DeleteCourseScenarioContext ctx;
    Client& clerk = ctx.clerk();

    GIVEN("The clerk enters dac") {
        send(clerk, "dac");

        WHEN("The clerk enters 12345") {
            send(clerk, "12345");

            wait_for_action_to_finish();

            THEN("Then Course does not exist. is printed") {
                CHECK("Course does not exist." == clerk.previous_message());

                AND_THEN("The course 12345 does not exist") {
                    CHECK(false == ctx.university().course(12345).has_value());
                }
            }
        }
    }
}

SCENARIO("A clerk deletes a course after registration starts") {
    DeleteCourseScenarioContext ctx;
    Client& clerk = ctx.clerk();

    GIVEN("There is an existing course 12345, Witchcraft, 1") {
        there_is_an_existing_course(clerk, "12345, Witchcraft, 1");

        GIVEN("We wait until registration starts") {
            wait(WaitUntil::REGISTRATION_STARTS);

            GIVEN("The clerk enters dac") {
                send(clerk, "dac");

                WHEN("The clerk enters 12345") {
                    send(clerk, "12345");

                    wait_for_action_to_finish();

                    THEN("Then Preregistration has ended. is printed") {
                        CHECK("Preregistration has ended." ==
                              clerk.previous_message());

                        AND_THEN("The course 12345 exists") {
                            CHECK(true ==
                                  ctx.university().course(12345).has_value());
                        }
                    }
                }
            }
        }
    }
}

TEST_SUITE_END();
