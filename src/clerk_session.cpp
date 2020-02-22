#include <iostream>
#include <memory>

#include "clerk_session.hpp"

ClerkSession::ClerkSession(tcp::socket socket)
    : Session(std::move(socket)) /*, state()*/ {}

void ClerkSession::start() {
    this->greeting();
    this->read_header();
}

void ClerkSession::greeting() {
    this->write_messages.push_back(
        Message("Welcome Clerk!\n"
                "Options: Create a Course (CAC), Create a Student (CAS), "
                "Delete a Course (DAC), Delete a Student (DAS)."));
    this->write();
}

bool ClerkSession::handle_input() {
    // TODO: We can do better here. Message itself can probably store a string
    // or something.
    std::string input(this->read_message.body());

    // this->write();

    return true;
}
