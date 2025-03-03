#include <stdexcept>

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
        this->list_courses();
        this->write_messages.push_back(Message("Enter Course ID:"));
        break;
    case State::CREATING_STUDENT:
        this->write_messages.push_back(
            Message("Enter Student Information: ID (9 digits), Name"));
        break;
    case State::DELETING_STUDENT:
        this->list_students();
        this->write_messages.push_back(Message("Enter Student ID:"));
        break;
    }

    this->write();
}

void ClerkSession::create_course() {
    const auto tokens = util::split(this->read_message.body(), ',');

    if (tokens.size() != 3) {
        this->write_messages.push_back(
            Message("ERROR - Invalid input. Expected course ID, course title, "
                    "and capsize."));
        return;
    }

    const std::uint32_t id = this->parse_course_id(tokens[0]);
    if (id == 0) {
        return;
    }

    const std::string title = tokens[1];

    const std::uint8_t capsize = this->parse_course_capsize(tokens[2]);
    if (capsize == 0) {
        return;
    }

    const ClerkResult result =
        this->university.create_course(id, title, capsize);

    switch (result) {
    case ClerkResult::SUCCESS:
        this->write_messages.push_back(Message("Course created."));
        return;
    case ClerkResult::COURSE_EXISTS:
        this->write_messages.push_back(
            Message("ERROR - Course already exists."));
        return;
    case ClerkResult::PREREGISTRATION_ENDED:
        this->write_messages.push_back(
            Message("ERROR - Preregistration has ended."));
        return;
    default:
        break;
    }
}

void ClerkSession::delete_course() {
    std::uint32_t id = this->parse_course_id(this->read_message.body());
    if (id == 0) {
        return;
    }

    const ClerkResult result = this->university.delete_course(id);

    switch (result) {
    case ClerkResult::SUCCESS:
        this->write_messages.push_back(Message("Course deleted."));
        return;
    case ClerkResult::COURSE_DOES_NOT_EXIST:
        this->write_messages.push_back(
            Message("ERROR - Course does not exist."));
        return;
    case ClerkResult::PREREGISTRATION_ENDED:
        this->write_messages.push_back(
            Message("ERROR - Preregistration has ended."));
        return;
    default:
        break;
    }
}

void ClerkSession::create_student() {
    const auto tokens = util::split(this->read_message.body(), ',');

    if (tokens.size() != 2) {
        this->write_messages.push_back(Message(
            "ERROR - Invalid input. Expected student ID (9 digits) and name."));
        return;
    }

    const std::uint32_t id = this->parse_student_id(tokens[0]);
    if (id == 0) {
        return;
    }

    const std::string name = tokens[1];

    const ClerkResult result = this->university.register_student(id, name);

    switch (result) {
    case ClerkResult::SUCCESS:
        this->write_messages.push_back(Message("Student created."));
        return;
    case ClerkResult::STUDENT_EXISTS:
        this->write_messages.push_back(
            Message("ERROR - Student already exists."));
        return;
    case ClerkResult::PREREGISTRATION_ENDED:
        this->write_messages.push_back(
            Message("ERROR - Preregistration has ended."));
        return;
    default:
        break;
    }
}

void ClerkSession::delete_student() {
    const std::uint32_t id = this->parse_student_id(this->read_message.body());
    if (id == 0) {
        return;
    }

    const ClerkResult result = this->university.delete_student(id);

    switch (result) {
    case ClerkResult::SUCCESS:
        this->write_messages.push_back(Message("Student deleted."));
        return;
    case ClerkResult::STUDENT_DOES_NOT_EXIST:
        this->write_messages.push_back(
            Message("ERROR - Student does not exist."));
        return;
    case ClerkResult::PREREGISTRATION_ENDED:
        this->write_messages.push_back(
            Message("ERROR - Preregistration has ended."));
        return;
    default:
        break;
    }
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
        this->write_messages.push_back(Message("ERROR - Invalid command."));
    }
}

void ClerkSession::list_courses() {
    this->write_messages.push_back(Message("Courses:"));

    for (const auto& course : university.courses()) {
        const std::string course_info =
            "\t" + std::to_string(course.first) + " - " + course.second.title();
        this->write_messages.push_back(Message(course_info));
    }
}

void ClerkSession::list_students() {
    this->write_messages.push_back(Message("Students:"));

    for (const auto& student : university.students()) {
        const std::string student_info = "\t" + std::to_string(student.first) +
                                         " - " + student.second.name();
        this->write_messages.push_back(Message(student_info));
    }
}
