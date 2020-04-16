#include <doctest.h>
#include <iostream>

#include "../scenario_context.hpp"
#include "../step_definitions.hpp"

class Ex1ScenarioContext final : public ScenarioContext {
public:
    Ex1ScenarioContext()
        : clerk_resolver(tcp::resolver(this->clerk_io_context)),
          s1_resolver(tcp::resolver(this->s1_io_context)),
          s2_resolver(tcp::resolver(this->s2_io_context)),
          s3_resolver(tcp::resolver(this->s3_io_context)),
          s4_resolver(tcp::resolver(this->s4_io_context)),
          clerk_endpoints(this->clerk_resolver.resolve(host, port)),
          s1_endpoints(this->s1_resolver.resolve(host, port)),
          s2_endpoints(this->s2_resolver.resolve(host, port)),
          s3_endpoints(this->s3_resolver.resolve(host, port)),
          s4_endpoints(this->s4_resolver.resolve(host, port)),
          clerk(Client(this->clerk_io_context, this->clerk_endpoints)),
          _s1(Client(this->s1_io_context, this->s1_endpoints)),
          _s2(Client(this->s2_io_context, this->s2_endpoints)),
          _s3(Client(this->s3_io_context, this->s3_endpoints)),
          _s4(Client(this->s4_io_context, this->s4_endpoints)) {
        this->clerk_thread = std::thread([&] { this->clerk_io_context.run(); });
        this->s1_thread    = std::thread([&] { this->s1_io_context.run(); });
        this->s2_thread    = std::thread([&] { this->s2_io_context.run(); });
        this->s3_thread    = std::thread([&] { this->s3_io_context.run(); });
        this->s4_thread    = std::thread([&] { this->s4_io_context.run(); });

        wait_for_clients_to_load();

        this->background();
    }

    ~Ex1ScenarioContext() {
        this->clerk_io_context.stop();
        this->s1_io_context.stop();
        this->s2_io_context.stop();
        this->s3_io_context.stop();
        this->s4_io_context.stop();

        this->clerk_thread.join();
        this->s1_thread.join();
        this->s2_thread.join();
        this->s3_thread.join();
        this->s4_thread.join();
    }

    void background() {
        the_clerk_is_logged_in(this->clerk);
        there_is_an_existing_course(this->clerk, "123456, C1, 2");
        there_is_an_existing_student(this->clerk, "111111111, S1");
        there_is_an_existing_student(this->clerk, "222222222, S2");
        there_is_an_existing_student(this->clerk, "333333333, S3");
        there_is_an_existing_student(this->clerk, "444444444, S4");

        std::this_thread::sleep_for(std::chrono::milliseconds(5));

        the_student_is_logged_in_as(this->_s1, "111111111, S1");
        the_student_is_logged_in_as(this->_s2, "222222222, S2");
        the_student_is_logged_in_as(this->_s3, "333333333, S3");
        the_student_is_logged_in_as(this->_s4, "444444444, S4");

        wait(WaitUntil::REGISTRATION_STARTS);

        the_student_has_registered_in(this->_s1, "123456");
    }

    Client& s1() { return this->_s1; }
    Client& s2() { return this->_s2; }
    Client& s3() { return this->_s3; }
    Client& s4() { return this->_s4; }

private:
    asio::io_context clerk_io_context;
    asio::io_context s1_io_context;
    asio::io_context s2_io_context;
    asio::io_context s3_io_context;
    asio::io_context s4_io_context;

    tcp::resolver clerk_resolver;
    tcp::resolver s1_resolver;
    tcp::resolver s2_resolver;
    tcp::resolver s3_resolver;
    tcp::resolver s4_resolver;

    const tcp::resolver::results_type clerk_endpoints;
    const tcp::resolver::results_type s1_endpoints;
    const tcp::resolver::results_type s2_endpoints;
    const tcp::resolver::results_type s3_endpoints;
    const tcp::resolver::results_type s4_endpoints;

    std::thread clerk_thread;
    std::thread s1_thread;
    std::thread s2_thread;
    std::thread s3_thread;
    std::thread s4_thread;

    Client clerk;
    Client _s1;
    Client _s2;
    Client _s3;
    Client _s4;
};

// clang-format off
SCENARIO("EX1N - Three students attempt to simultaneously register in a course with 1 spot left") {
    // clang-format on
    Ex1ScenarioContext ctx;
    Client& s2 = ctx.s2();
    Client& s3 = ctx.s3();
    Client& s4 = ctx.s4();

    GIVEN("S2 registers in 123456") {
        the_student_has_registered_in(s2, "123456");

        GIVEN("S3 registers in 123456") {
            the_student_has_registered_in(s3, "123456");

            GIVEN("S4 registers in 123456") {
                the_student_has_registered_in(s4, "123456");

                WHEN("We wait for 1 day") {
                    wait(WaitUntil::CUSTOM, 1);

                    THEN("S1 and one of the other students has registered in "
                         "123456") {
                        CHECK(
                            true ==
                            ctx.university().course(123456).value().has_student(
                                111111111));

                        CHECK(((true == ctx.university()
                                            .course(123456)
                                            .value()
                                            .has_student(222222222)) ||
                               (true == ctx.university()
                                            .course(123456)
                                            .value()
                                            .has_student(333333333)) ||
                               (true == ctx.university()
                                            .course(123456)
                                            .value()
                                            .has_student(444444444))));

                        auto students = ctx.university()
                                            .course(123456)
                                            .value()
                                            .student_ids();

                        std::cout << "\nStudents in 123456: " << students[0]
                                  << ", " << students[1] << "\n";
                    }
                }
            }
        }
    }
}
