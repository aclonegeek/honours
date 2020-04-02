#include <memory>
#include <string_view>

#include "clerk_session.hpp"
#include "student_session.hpp"
#include "temporary_session.hpp"
#include "util.hpp"

TemporarySession::TemporarySession(tcp::socket socket, University& university)
    : Session(std::move(socket)),
      university(university),
      state(State::DETERMINING_SESSION_TYPE) {}

void TemporarySession::start() {
    this->greeting();
    this->read_header();
}

void TemporarySession::greeting() {
    this->write_messages.push_back(
        Message("Welcome!\n"
                "Please specify your role (clerk or student)."));
    this->write();
}

bool TemporarySession::handle_input() {
    const std::string_view input{this->read_message.body()};

    switch (this->state) {
    case State::DETERMINING_SESSION_TYPE:
        if (input == "clerk") {
            this->state = State::CLERK_LOGIN;
            // TODO: Add an attirbute to Message that specifies if we want a
            // newline printed or not for the corresponding Message.
            this->write_messages.push_back(Message("Password:"));
        } else if (input == "student") {
            this->state = State::STUDENT_LOGIN;
            this->write_messages.push_back(Message("Student ID, Name:"));
        } else {
            this->write_messages.push_back(
                Message("Invalid input. Enter clerk or student."));
        }

        break;
    case State::CLERK_LOGIN:
        if (input != "admin") {
            this->write_messages.push_back(
                Message("Invalid password. Try again."));
            this->write_messages.push_back(Message("Password:"));
            break;
        }

        std::make_shared<ClerkSession>(std::move(this->socket),
                                       this->university)
            ->start();

        return false;
    case State::STUDENT_LOGIN:
        const auto tokens = util::split(this->read_message.body(), ',');

        if (tokens.size() != 2) {
            this->write_messages.push_back(
                Message("Invalid input. Expected student ID and name."));
            this->write_messages.push_back(Message("Student ID, Name:"));
            break;
        }

        const std::uint32_t id = std::stoi(tokens[0]);
        const std::string name = tokens[1];

        if (!this->university.student(id)) {
            this->write_messages.push_back(Message("Student does not exist."));
            this->write_messages.push_back(Message("Student ID, Name:"));
            break;
        }

        std::make_shared<StudentSession>(std::move(this->socket),
                                         this->university, id)
            ->start();

        return false;
    }

    this->write();
    return true;
}
