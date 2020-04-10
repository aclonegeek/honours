#include <doctest.h>

#include <iostream>

#include "../scenario_context.hpp"
#include "../step_definitions.hpp"

SCENARIO("EX2P1.0 - The server is initialized") {
    ScenarioContext ctx;

    GIVEN("We wait 5 days after registration starts") {
        wait(WaitUntil::CUSTOM, PREREGISTRATION_LENGTH + 5);

        THEN("S1 is registered in course 12345") {
            CHECK(true == ctx.university().course(12345).value().has_student(
                              111111111));

            AND_THEN("S2 is not registered in course 12345") {
                CHECK(false ==
                      ctx.university().course(12345).value().has_student(
                          222222222));

                AND_THEN("S3 and/or S4 are registered in course 12345") {
                    CHECK(((true ==
                            ctx.university().course(12345).value().has_student(
                                333333333)) ||
                           (true ==
                            ctx.university().course(12345).value().has_student(
                                444444444))));

                    auto students =
                        ctx.university().course(12345).value().student_ids();

                    std::cout << "\nStudents in 12345: ";
                    for (const auto& s : students) {
                        std::cout << s << ", ";
                    }
                    std::cout << "\n";
                }
            }
        }
    }
}

SCENARIO("EX2P1.1 - The clerk creates the course and students") {
    asio::io_context io_context;

    tcp::resolver resolver{io_context};
    const tcp::resolver::results_type endpoints = resolver.resolve(host, port);
    Client clerk{io_context, endpoints};

    std::thread t([&io_context]() { io_context.run(); });

    wait_for_clients_to_load();

    GIVEN("The clerk connects to the server and logs in") {
        the_clerk_is_logged_in(clerk);

        GIVEN("The clerk creates the course 12345, C1, 3") {
            there_is_an_existing_course(clerk, "12345, C1, 3");

            GIVEN("The clerk creates the student 111111111, S1") {
                there_is_an_existing_student(clerk, "111111111, S1");

                GIVEN("The clerk creates the student 222222222, S2") {
                    there_is_an_existing_student(clerk, "222222222, S2");

                    GIVEN("The clerk creates the student 333333333, S3") {
                        there_is_an_existing_student(clerk, "333333333, S3");

                        GIVEN("The clerk creates the student 444444444, S4") {
                            there_is_an_existing_student(clerk,
                                                         "444444444, S4");

                            std::this_thread::sleep_for(
                                std::chrono::milliseconds(5));
                        }
                    }
                }
            }
        }
    }

    io_context.stop();
    t.join();
}

SCENARIO("EX2P1.2 - S1 registers in C1") {
    asio::io_context io_context;

    tcp::resolver resolver{io_context};
    const tcp::resolver::results_type endpoints = resolver.resolve(host, port);
    Client client{io_context, endpoints};

    std::thread t([&io_context]() { io_context.run(); });

    wait_for_clients_to_load();
    // Wait for the clerk to create the students...
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    GIVEN("S1 connects") {
        the_student_has_logged_in_as(client, "111111111, S1");

        GIVEN("We wait until registration starts") {
            wait(WaitUntil::REGISTRATION_STARTS);

            WHEN("S1 registers in the course 12345") {
                the_student_has_registered_in(client, "12345");
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }
    }

    io_context.stop();
    t.join();
}

SCENARIO("EX2P1.3 - S2 registers in C1") {
    asio::io_context io_context;

    tcp::resolver resolver{io_context};
    const tcp::resolver::results_type endpoints = resolver.resolve(host, port);
    Client client{io_context, endpoints};

    std::thread t([&io_context]() { io_context.run(); });

    wait_for_clients_to_load();
    // Wait for the clerk to create the students...
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    GIVEN("S2 connects") {
        the_student_has_logged_in_as(client, "222222222, S2");

        GIVEN("We wait until registration starts") {
            wait(WaitUntil::REGISTRATION_STARTS);

            WHEN("S2 registers in the course 12345") {
                the_student_has_registered_in(client, "12345");
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }
    }

    io_context.stop();
    t.join();
}

SCENARIO("EX2P1.4 - S2 deregisters from C1") {
    asio::io_context io_context;

    tcp::resolver resolver{io_context};
    const tcp::resolver::results_type endpoints = resolver.resolve(host, port);
    Client client{io_context, endpoints};

    std::thread t([&io_context]() { io_context.run(); });

    wait_for_clients_to_load();
    // Wait for the clerk to create the students...
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    GIVEN("S2 connects") {
        the_student_has_logged_in_as(client, "222222222, S2");

        GIVEN("We wait until 1 day after registration starts") {
            wait(WaitUntil::CUSTOM, PREREGISTRATION_LENGTH + 1);

            WHEN("S2 registers in the course 12345") {
                the_student_has_deregistered_from(client, "12345");
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }
    }

    io_context.stop();
    t.join();
}

SCENARIO("EX2P1.5 - S3 registers in C1") {
    asio::io_context io_context;

    tcp::resolver resolver{io_context};
    const tcp::resolver::results_type endpoints = resolver.resolve(host, port);
    Client client{io_context, endpoints};

    std::thread t([&io_context]() { io_context.run(); });

    wait_for_clients_to_load();
    // Wait for the clerk to create the students...
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    GIVEN("S3 connects") {
        the_student_has_logged_in_as(client, "333333333, S3");

        GIVEN("We wait 1 day after registration starts") {
            wait(WaitUntil::CUSTOM, PREREGISTRATION_LENGTH + 1);

            WHEN("S3 registers in the course 12345") {
                the_student_has_registered_in(client, "12345");
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }
    }

    io_context.stop();
    t.join();
}

SCENARIO("EX2P1.6 - S4 registers in C1") {
    asio::io_context io_context;

    tcp::resolver resolver{io_context};
    const tcp::resolver::results_type endpoints = resolver.resolve(host, port);
    Client client{io_context, endpoints};

    std::thread t([&io_context]() { io_context.run(); });

    wait_for_clients_to_load();
    // Wait for the clerk to create the students...
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    GIVEN("S4 connects") {
        the_student_has_logged_in_as(client, "444444444, S4");

        GIVEN("We wait 1 day after registration starts") {
            wait(WaitUntil::CUSTOM, PREREGISTRATION_LENGTH + 1);

            WHEN("S4 registers in the course 12345") {
                the_student_has_registered_in(client, "12345");
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            }
        }
    }

    io_context.stop();
    t.join();
}
