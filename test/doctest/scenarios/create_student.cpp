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

        // Give everything some time to start up.
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

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

            std::this_thread::sleep_for(std::chrono::milliseconds(5));

            THEN("Then Student created. is printed") {
                CHECK("Student created." ==
                      std::string_view(clerk.previous_message()));

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

                std::this_thread::sleep_for(std::chrono::milliseconds(5));

                THEN("Then Student already exists. is printed") {
                    CHECK("Student already exists." ==
                          std::string_view(clerk.previous_message()));
                }
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

                std::this_thread::sleep_for(std::chrono::milliseconds(5));

                THEN("Then Preregistration has ended. is printed") {
                    CHECK("Preregistration has ended." ==
                          std::string_view(clerk.previous_message()));
                }
            }
        }
    }
}

TEST_SUITE_END();
