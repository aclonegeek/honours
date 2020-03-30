#include <chrono>
#include <thread>

#include "client.hpp"
#include "step_definitions.hpp"
#include "university.hpp"

void send(Client& client, const std::string& message) {
    client.send(Message(message));
}

void wait(const WaitUntil wait, const uint8_t days_to_wait) {
    switch (wait) {
    case WaitUntil::REGISTRATION_STARTS:
        std::this_thread::sleep_for(
            std::chrono::seconds(DAY_LENGTH * PREREGISTRATION_LENGTH));
        break;
    case WaitUntil::REGISTRATION_ENDS:
        std::this_thread::sleep_for(std::chrono::seconds(
            DAY_LENGTH * (PREREGISTRATION_LENGTH + REGISTRATION_LENGTH)));
        break;
    case WaitUntil::TERM_ENDS:
        std::this_thread::sleep_for(std::chrono::seconds(
            DAY_LENGTH *
            (PREREGISTRATION_LENGTH + REGISTRATION_LENGTH + TERM_LENGTH)));
        break;
    case WaitUntil::CUSTOM:
        std::this_thread::sleep_for(
            std::chrono::seconds(DAY_LENGTH * days_to_wait));
        break;
    }
}

void the_clerk_is_logged_in(Client& clerk) {
    clerk.send(Message("clerk"));
    clerk.send(Message("admin"));
}

void the_student_has_logged_in_as(Client& student, const std::string& message) {
    student.send(Message("student"));
    student.send(Message(message));
}

void there_is_an_existing_course(Client& clerk, const std::string& message) {
    clerk.send(Message("cac"));
    clerk.send(Message(message));
}

void there_is_an_existing_student(Client& client, const std::string& message) {
    client.send(Message("cas"));
    client.send(Message(message));
}
