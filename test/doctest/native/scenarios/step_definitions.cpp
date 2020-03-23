#include "client.hpp"

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
