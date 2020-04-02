#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
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

        // Give everything some time to start up.
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

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

        WHEN("The clerk enters 12345, Witchcraft, 1") {
            send(clerk, "12345, Witchcraft, 1");

            std::this_thread::sleep_for(std::chrono::milliseconds(5));

            THEN("Course created. is printed") {
                CHECK("Course created." ==
                      std::string_view(clerk.previous_message()));

                AND_THEN("The course 12345 exists") {
                    CHECK(true == ctx.university().course(12345).has_value());
                }
            }
        }
    }
}

SCENARIO("A clerk creates a duplicate course before registration starts") {
    CreateCourseScenarioContext ctx;
    Client& clerk = ctx.clerk();

    GIVEN("There is an existing course 12345, Witchcraft, 1") {
        there_is_an_existing_course(clerk, "12345, Witchcraft, 1");

        GIVEN("The clerk enters cac") {
            send(clerk, "cac");

            WHEN("The clerk enters 12345, Witchcraft, 1") {
                send(clerk, "12345, Witchcraft, 1");

                std::this_thread::sleep_for(std::chrono::milliseconds(5));

                THEN("Course already exists. is printed") {
                    CHECK("Course already exists." ==
                          std::string_view(clerk.previous_message()));

                    AND_THEN("The course 12345 exists") {
                        CHECK(true ==
                              ctx.university().course(12345).has_value());
                    }
                }
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

            WHEN("The clerk enters 12345, Witchcraft, 1") {
                send(clerk, "12345, Witchcraft, 1");

                std::this_thread::sleep_for(std::chrono::milliseconds(5));

                THEN("Pregistration has ended. is printed") {
                    CHECK("Preregistration has ended." ==
                          std::string_view(clerk.previous_message()));

                    AND_THEN("The course 12345 does not exist") {
                        CHECK(false ==
                              ctx.university().course(12345).has_value());
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

            WHEN("The clerk enters 12345, Witchcraft, 1") {
                send(clerk, "12345, Witchcraft, 1");

                std::this_thread::sleep_for(std::chrono::milliseconds(5));

                THEN("Pregistration has ended. is printed") {
                    CHECK("Preregistration has ended." ==
                          std::string_view(clerk.previous_message()));

                    AND_THEN("The course 12345 does not exist") {
                        CHECK(false ==
                              ctx.university().course(12345).has_value());
                    }
                }
            }
        }
    }
}

TEST_SUITE_END();
