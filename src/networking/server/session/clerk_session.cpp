#include "clerk_session.hpp"
#include "result_types.hpp"
#include "university.hpp"
#include "util.hpp"

static const Message OPTIONS =
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
    switch (this->state) {
    case State::WAITING_FOR_ACTION:
        this->set_state();
        break;
    case State::CREATING_COURSE:
        this->create_course();
        this->state = State::WAITING_FOR_ACTION;
        break;
    case State::DELETING_COURSE:
        this->delete_course();
        this->state = State::WAITING_FOR_ACTION;
        break;
    case State::CREATING_STUDENT:
        this->create_student();
        this->state = State::WAITING_FOR_ACTION;
        break;
    case State::DELETING_STUDENT:
        this->delete_student();
        this->state = State::WAITING_FOR_ACTION;
        break;
    }

    this->write_options();
    this->write();

    return true;
}

void ClerkSession::write_options() {
    switch (this->state) {
    case State::WAITING_FOR_ACTION:
        this->write_messages.push_back(OPTIONS);
        break;
    case State::CREATING_COURSE:
        this->write_messages.push_back(
            Message("Enter Course Information: ID (6 digits), Title, Capsize"));
        break;
    case State::DELETING_COURSE:
        this->write_messages.push_back(Message("Enter Course ID:"));
        break;
    case State::CREATING_STUDENT:
        this->write_messages.push_back(
            Message("Enter Student Information: ID (9 digits), Name"));
        break;
    case State::DELETING_STUDENT:
        this->write_messages.push_back(Message("Enter Student ID:"));
        break;
    }
}

void ClerkSession::create_course() {
    const auto tokens = util::split(this->read_message.body(), ',');

    if (tokens.size() != 3) {
        this->write_messages.push_back(Message(
            "Invalid input. Expected course ID, course title, and capsize."));
        return;
    }

    // TODO: Error handling.
    const std::uint16_t id     = std::stoi(tokens[0]);
    const std::string title    = tokens[1];
    const std::uint8_t capsize = std::stoi(tokens[2]);

    ClerkResult result = this->university.create_course(id, title, capsize);

    switch (result) {
    case ClerkResult::COURSE_EXISTS:
        this->write_messages.push_back(Message("Course already exists."));
        return;
    case ClerkResult::PREREGISTRATION_ENDED:
        this->write_messages.push_back(Message("Preregistration has ended."));
        return;
    default:
        break;
    }

    this->write_messages.push_back(Message("Course created."));
}

void ClerkSession::delete_course() {
    const std::uint16_t id = std::stoi(this->read_message.body());

    ClerkResult result = this->university.delete_course(id);

    switch (result) {
    case ClerkResult::COURSE_DOES_NOT_EXIST:
        this->write_messages.push_back(Message("Course does not exist."));
        return;
    case ClerkResult::PREREGISTRATION_ENDED:
        this->write_messages.push_back(Message("Preregistration has ended."));
        return;
    default:
        break;
    }

    this->write_messages.push_back(Message("Course deleted."));
}

void ClerkSession::create_student() {
    const auto tokens = util::split(this->read_message.body(), ',');

    if (tokens.size() != 2) {
        this->write_messages.push_back(
            Message("Invalid input. Expected student ID and name."));
        return;
    }

    if (tokens[0].length() != 9) {
        this->write_messages.push_back(
            Message("Invalid student ID. It must be 9 digits."));
        return;
    }

    const std::uint32_t id = std::stoi(tokens[0]);
    const std::string name = tokens[1];

    ClerkResult result = this->university.register_student(id, name);

    switch (result) {
    case ClerkResult::STUDENT_EXISTS:
        this->write_messages.push_back(Message("Student already exists."));
        return;
    case ClerkResult::PREREGISTRATION_ENDED:
        this->write_messages.push_back(Message("Preregistration has ended."));
        return;
    default:
        break;
    }

    this->write_messages.push_back(Message("Student created."));
}

void ClerkSession::delete_student() {
    if (this->read_message.body_length() != 9) {
        this->write_messages.push_back(
            Message("Invalid student ID. It must be 9 digits."));
        return;
    }

    const std::uint32_t id = std::stoi(this->read_message.body());

    ClerkResult result = this->university.delete_student(id);

    switch (result) {
    case ClerkResult::STUDENT_DOES_NOT_EXIST:
        this->write_messages.push_back(Message("Student does not exist."));
        return;
    case ClerkResult::PREREGISTRATION_ENDED:
        this->write_messages.push_back(Message("Preregistration has ended."));
        return;
    default:
        break;
    }

    this->write_messages.push_back(Message("Student deleted."));
}

void ClerkSession::set_state() {
    const std::string_view input{this->read_message.body()};

    if (input == "cac" || input == "CAC") {
        this->state = State::CREATING_COURSE;
    } else if (input == "cas" || input == "CAS") {
        this->state = State::CREATING_STUDENT;
    } else if (input == "dac" || input == "DAC") {
        this->state = State::DELETING_COURSE;
    } else if (input == "das" || input == "DAS") {
        this->state = State::DELETING_STUDENT;
    } else {
        this->write_messages.push_back(Message("Invalid command."));
    }
}
