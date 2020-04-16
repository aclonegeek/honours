#include <doctest.h>

#include <iostream>

#include "../scenario_context.hpp"
#include "../step_definitions.hpp"

SCENARIO("EX1P1.0 - The server is initialized") {
    ScenarioContext ctx;

    GIVEN("We wait 5 days after registration starts") {
        wait(WaitUntil::CUSTOM, PREREGISTRATION_LENGTH + 5);

        THEN("S1 is registered in the course 123456") {
            CHECK(true == ctx.university().course(123456).value().has_student(
                              111111111));

            AND_THEN(
                "One of S2, S3, or S4 are registered in the course 123456") {
                CHECK(((true ==
                        ctx.university().course(123456).value().has_student(
                            222222222)) ||
                       (true ==
                        ctx.university().course(123456).value().has_student(
                            333333333)) ||
                       (true ==
                        ctx.university().course(123456).value().has_student(
                            444444444))));

                auto students =
                    ctx.university().course(123456).value().student_ids();

                std::cout << "\nStudents in 123456: " << students[0] << ", "
                          << students[1] << "\n";
            }
        }
    }
}

SCENARIO("EX1P1.1 - The clerk creates the course and students") {
    ParallelScenarioContext ctx;
    Client& clerk = ctx.client();

    GIVEN("The clerk connects to the server and logs in") {
        the_clerk_is_logged_in(clerk);

        GIVEN("The clerk creates the course 123456, C1, 2") {
            there_is_an_existing_course(clerk, "12345, C1, 2");

            GIVEN("The clerk creates the student 111111111, S1") {
                there_is_an_existing_student(clerk, "111111111, S1");

                GIVEN("The clerk creates the student 222222222, S2") {
                    there_is_an_existing_student(clerk, "222222222, S2");

                    GIVEN("The clerk creates the student 333333333, S3") {
                        there_is_an_existing_student(clerk, "333333333, S3");

                        GIVEN("The clerk creates the student 444444444, S4") {
                            there_is_an_existing_student(clerk,
                                                         "444444444, S4");

                            wait_for_action_to_finish();
                        }
                    }
                }
            }
        }
    }
}

SCENARIO("EX1P1.2 - S1 registers in C1") {
    ParallelScenarioContext ctx;
    Client& client = ctx.client();

    // Wait for the clerk to create the students...
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    GIVEN("S1 connects") {
        the_student_is_logged_in_as(client, "111111111, S1");

        GIVEN("We wait until registration starts") {
            wait(WaitUntil::REGISTRATION_STARTS);

            WHEN("S1 registers in the course 123456") {
                the_student_has_registered_in(client, "123456");
                wait_for_action_to_finish();
            }
        }
    }
}

SCENARIO("EX1P1.3 - S2 registers in C1") {
    ParallelScenarioContext ctx;
    Client& client = ctx.client();

    // Wait for the clerk to create the students...
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    GIVEN("S2 connects") {
        the_student_is_logged_in_as(client, "222222222, S2");

        GIVEN("We wait 1 day after registration starts") {
            wait(WaitUntil::CUSTOM, PREREGISTRATION_LENGTH + 1);

            WHEN("S2 registers in the course 123456") {
                the_student_has_registered_in(client, "123456");
                wait_for_action_to_finish();
            }
        }
    }
}

SCENARIO("EX1P1.4 - S3 registers in C1") {
    ParallelScenarioContext ctx;
    Client& client = ctx.client();

    // Wait for the clerk to create the students...
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    GIVEN("S3 connects") {
        the_student_is_logged_in_as(client, "333333333, S3");

        GIVEN("We wait 1 day after registration starts") {
            wait(WaitUntil::CUSTOM, PREREGISTRATION_LENGTH + 1);

            WHEN("S3 registers in the course 123456") {
                the_student_has_registered_in(client, "123456");
                wait_for_action_to_finish();
            }
        }
    }
}

SCENARIO("EX1P1.5 - S4 registers in C1") {
    ParallelScenarioContext ctx;
    Client& client = ctx.client();

    // Wait for the clerk to create the students...
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    GIVEN("S4 connects") {
        the_student_is_logged_in_as(client, "444444444, S4");

        GIVEN("We wait 1 day after registration starts") {
            wait(WaitUntil::CUSTOM, PREREGISTRATION_LENGTH + 1);

            WHEN("S4 registers in the course 123456") {
                the_student_has_registered_in(client, "123456");
                wait_for_action_to_finish();
            }
        }
    }
}
