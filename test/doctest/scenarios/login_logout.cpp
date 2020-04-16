#include <doctest.h>

#include "scenario_context.hpp"
#include "step_definitions.hpp"

class LoginLogoutScenarioContext final : public ScenarioContext {
public:
    LoginLogoutScenarioContext()
        : clerk_resolver(tcp::resolver(this->clerk_io_context)),
          joe_resolver(tcp::resolver(this->joe_io_context)),
          clerk_endpoints(this->clerk_resolver.resolve(host, port)),
          joe_endpoints(this->joe_resolver.resolve(host, port)),
          _clerk(Client(this->clerk_io_context, this->clerk_endpoints)),
          _joe(Client(this->joe_io_context, this->joe_endpoints)) {
        this->clerk_thread = std::thread([&] { this->clerk_io_context.run(); });
        this->joe_thread   = std::thread([&] { this->joe_io_context.run(); });

        wait_for_clients_to_load();
    }

    ~LoginLogoutScenarioContext() {
        this->clerk_io_context.stop();
        this->joe_io_context.stop();

        this->clerk_thread.join();
        this->joe_thread.join();
    }

    void background() {}

    Client& clerk() { return this->_clerk; }
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

    Client _clerk;
    Client _joe;
};

TEST_SUITE_BEGIN("Logging in and out");

SCENARIO("A clerk logs in with a valid password") {
    LoginLogoutScenarioContext ctx;
    Client& clerk = ctx.clerk();

    GIVEN("The clerk enters clerk") {
        send(clerk, "clerk");

        WHEN("The clerk enters admin") {
            send(clerk, "admin");

            wait_for_action_to_finish();

            THEN("Welcome Clerk! is printed") {
                CHECK("Welcome Clerk!" == clerk.previous_message());
            }
        }
    }
}

SCENARIO("A clerk logs in with an invalid password") {
    LoginLogoutScenarioContext ctx;
    Client& clerk = ctx.clerk();

    GIVEN("The clerk enters clerk") {
        send(clerk, "clerk");

        WHEN("The clerk enters quack") {
            send(clerk, "quack");

            wait_for_action_to_finish();

            THEN("ERROR - Invalid password. Try again. is printed") {
                CHECK("ERROR - Invalid password. Try again." ==
                      clerk.previous_message());
            }
        }
    }
}

SCENARIO("A student logs in with valid information") {
    LoginLogoutScenarioContext ctx;
    Client& clerk = ctx.clerk();
    Client& joe   = ctx.joe();

    GIVEN("There is an existing student 123456789, joe") {
        the_clerk_is_logged_in(clerk);
        there_is_an_existing_student(clerk, "123456789, joe");

        wait_for_action_to_finish();

        GIVEN("The student enters student") {
            send(joe, "student");

            WHEN("The student enters 123456789, joe") {
                send(joe, "123456789, joe");

                wait_for_action_to_finish();

                THEN("Welcome Student! is printed") {
                    CHECK("Welcome Student!" == joe.previous_message());
                }
            }
        }
    }
}

SCENARIO("A student logs in with invalid information") {
    LoginLogoutScenarioContext ctx;
    Client& clerk = ctx.clerk();
    Client& joe   = ctx.joe();

    GIVEN("There is an existing student 123456789, joe") {
        the_clerk_is_logged_in(clerk);
        there_is_an_existing_student(clerk, "123456789, joe");

        wait_for_action_to_finish();

        GIVEN("The student enters student") {
            send(joe, "student");

            WHEN("The student enters 123456788, joe") {
                send(joe, "123456788, joe");

                wait_for_action_to_finish();

                THEN("ERROR - Student does not exist. is printed") {
                    CHECK("ERROR - Student does not exist." ==
                          joe.previous_message());
                }
            }
        }
    }
}

TEST_SUITE_END();
