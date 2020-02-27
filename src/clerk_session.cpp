#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "clerk_session.hpp"
#include "util.hpp"

const Message OPTIONS =
    Message("Options: Create a Course (CAC), Create a Student (CAS), "
            "Delete a Course (DAC), Delete a Student (DAS).");

ClerkSession::ClerkSession(tcp::socket socket, University& university)
    : Session(std::move(socket)),
      state(State::WAITING_FOR_ACTION),
      university(university) {}

void ClerkSession::start() {
    this->greeting();
    this->read_header();
}

void ClerkSession::greeting() {
    this->write_messages.push_back(Message("Welcome Clerk!"));
    this->write_messages.push_back(OPTIONS);
    this->write();
}

bool ClerkSession::handle_input() {
    // TODO: We can do better here. Message itself can probably store a string
    // or something.
    std::string input(this->read_message.body());

    switch (this->state) {
    case State::WAITING_FOR_ACTION:
        std::transform(input.begin(), input.end(), input.begin(), ::tolower);

        if (input == "cac") {
            this->state = State::CREATING_COURSE;
            this->write_messages.push_back(Message(
                "Enter Course Information: ID (6 digits), Title, Capsize"));
        } else if (input == "cas") {
            this->state = State::CREATING_STUDENT;
            this->write_messages.push_back(
                Message("Enter Student Information: ID (9 digits), Name"));
        } else if (input == "dac") {
            this->state = State::DELETING_COURSE;
            this->write_messages.push_back(Message("Enter Course ID:"));
        } else if (input == "das") {
            this->state = State::DELETING_STUDENT;
            this->write_messages.push_back(Message("Enter Student ID:"));
        } else {
            this->write_messages.push_back(Message("Invalid command."));
            this->write_messages.push_back(OPTIONS);
        }

        break;
    case State::CREATING_COURSE:
        if (!this->create_course(input)) {
            break;
        }

        this->state = State::WAITING_FOR_ACTION;
        this->write_messages.push_back(Message("Course created."));
        this->write_messages.push_back(OPTIONS);

        break;
    case State::DELETING_COURSE:
        if (!this->delete_course(input)) {
            break;
        }

        this->state = State::WAITING_FOR_ACTION;
        this->write_messages.push_back(Message("Course deleted."));
        this->write_messages.push_back(OPTIONS);

        break;
    case State::CREATING_STUDENT:
        if (!this->create_student(input)) {
            break;
        }

        this->state = State::WAITING_FOR_ACTION;
        this->write_messages.push_back(Message("Student created."));
        this->write_messages.push_back(OPTIONS);

        break;
    case State::DELETING_STUDENT:
        break;
    }

    this->write();

    return true;
}

bool ClerkSession::create_course(const std::string& input) {
    auto tokens = split(input, ',');

    if (tokens.size() != 3) {
        this->write_messages.push_back(Message(
            "Invalid input. Expected course ID, course title, and capsize."));
        return false;
    }

    // TODO: Error handling.
    std::uint16_t id     = std::stoi(tokens[0]);
    std::string title    = tokens[1];
    std::uint8_t capsize = std::stoi(tokens[2]);

    this->university.create_course(id, title, capsize);

    return true;
}

bool ClerkSession::delete_course(const std::string& input) {
    std::uint16_t id = std::stoi(input);

    if (!this->university.course(id)) {
        this->write_messages.push_back(Message("Course does not exist."));
        return false;
    }

    this->university.delete_course(id);

    return true;
}

bool ClerkSession::create_student(const std::string& input) {
    auto tokens = split(input, ',');

    if (tokens.size() != 2) {
        this->write_messages.push_back(
            Message("Invalid input. Expected student ID and name."));
        return false;
    }

    if (tokens[0].length() != 9) {
        this->write_messages.push_back(
            Message("Invalid student ID. It must be 9 digits."));
        return false;
    }

    std::uint16_t id = std::stoi(tokens[0]);
    std::string name = tokens[1];

    this->university.register_student(id, name);

    return true;
}

bool ClerkSession::delete_student(const std::string& input) {
    if (input.length() != 9) {
        this->write_messages.push_back(
            Message("Invalid student ID. It must be 9 digits."));
        return false;
    }

    std::uint16_t id = std::stoi(input);

    if (!this->university.student(id)) {
        this->write_messages.push_back(Message("Student does not exist."));
        return false;
    }

    this->university.delete_student(id);

    return true;
}
