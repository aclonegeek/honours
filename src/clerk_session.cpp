#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <memory>

#include "clerk_session.hpp"
#include "util.hpp"

ClerkSession::ClerkSession(tcp::socket socket, University& university)
    : Session(std::move(socket)),
      state(State::WAITING_FOR_ACTION),
      university(university) {}

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

    switch (this->state) {
    case State::WAITING_FOR_ACTION:
        std::transform(input.begin(), input.end(), input.begin(), ::tolower);

        std::cerr << input << "L\n";

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
        }
        break;
    case State::CREATING_COURSE:
        if (!this->create_course(input)) {
            break;
        }

        this->state = State::WAITING_FOR_ACTION;
        this->write_messages.push_back(Message("Course created."));

        break;
    case State::DELETING_COURSE:
        if (!this->delete_course(input)) {
            break;
        }

        this->state = State::WAITING_FOR_ACTION;
        this->write_messages.push_back(Message("Course deleted."));

        break;
    case State::CREATING_STUDENT:
        break;
    case State::DELETING_STUDENT:
        break;
    }

    this->write();

    return true;
}

bool ClerkSession::create_course(const std::string& input) {
    std::vector<std::string> tokens = split(input, ',');

    if (tokens.size() != 3) {
        this->write_messages.push_front(Message(
            "Invalid input. Expected course ID, course title, and capsize."));
        return false;
    }

    // TODO: Error handling.
    std::uint16_t id     = std::stoi(tokens[0]);
    std::string title    = tokens[1];
    std::uint8_t capsize = std::stoi(tokens[2]);

    this->university.create_course(Course(id, title, capsize));

    return true;
}

bool ClerkSession::delete_course(const std::string& input) {
    std::uint16_t id = std::stoi(input);

    this->university.delete_course(id);

    return true;
}
