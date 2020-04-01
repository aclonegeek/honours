#include <string_view>

#include "result_types.hpp"
#include "student_session.hpp"
#include "university.hpp"

const Message OPTIONS =
    Message("Options: Register for Course (RFC), Deregister from Course (DFC), "
            "Drop a Course (DAC).");

StudentSession::StudentSession(tcp::socket socket, University& university,
                               const std::uint32_t id)
    : Session(std::move(socket)),
      state(State::WAITING_FOR_ACTION),
      university(university),
      id(id) {}

void StudentSession::start() {
    this->greeting();
    this->read_header();
}

void StudentSession::greeting() {
    this->write_messages.push_back(Message("Welcome Student!"));
    this->write_messages.push_back(OPTIONS);
    this->write();
}

bool StudentSession::handle_input() {
    switch (this->state) {
    case State::WAITING_FOR_ACTION:
        this->set_state();
        this->write_options();
        break;
    case State::REGISTER_FOR_COURSE:
        this->register_for_course();
        this->state = State::WAITING_FOR_ACTION;
        this->write_options();
        break;
    case State::DEREGISTER_FROM_COURSE:
        this->deregister_from_course();
        this->state = State::WAITING_FOR_ACTION;
        this->write_options();
        break;
    case State::DROP_COURSE:
        this->drop_course();
        this->state = State::WAITING_FOR_ACTION;
        this->write_options();
        break;
    }

    this->write();

    return true;
}

void StudentSession::write_options() {
    switch (this->state) {
    case State::WAITING_FOR_ACTION:
        this->write_messages.push_back(OPTIONS);
        break;
    case State::REGISTER_FOR_COURSE:
        [[fallthrough]];
    case State::DEREGISTER_FROM_COURSE:
        [[fallthrough]];
    case State::DROP_COURSE:
        this->write_messages.push_back(Message("Enter Course ID:"));
        break;
    }
}

void StudentSession::register_for_course() {
    const std::uint16_t course_id = std::stoi(this->read_message.body());

    StudentResult result =
        this->university.register_student_in_course(this->id, course_id);

    switch (result) {
    case StudentResult::SUCCESS:
        this->write_messages.push_back(Message("Registered for course."));
        break;
    case StudentResult::COURSE_DOES_NOT_EXIST:
        this->write_messages.push_back(Message("Course does not exist."));
        break;
    case StudentResult::REGISTRATION_NOT_STARTED:
        this->write_messages.push_back(
            Message("Registration has not started."));
        break;
    case StudentResult::COURSE_FULL:
        this->write_messages.push_back(Message("Course is full."));
        break;
    default:
        // TODO: Do something here.
        break;
    }
}

void StudentSession::deregister_from_course() {
    const std::uint16_t course_id = std::stoi(this->read_message.body());

    StudentResult result =
        this->university.deregister_student_from_course(this->id, course_id);

    switch (result) {
    case StudentResult::SUCCESS:
        this->write_messages.push_back(Message("Deregistered from course."));
        break;
    case StudentResult::COURSE_DOES_NOT_EXIST:
        this->write_messages.push_back(Message("Course does not exist."));
        break;
    case StudentResult::STUDENT_NOT_REGISTERED:
        this->write_messages.push_back(
            Message("Student is not registered in course."));
        break;
    case StudentResult::REGISTRATION_NOT_STARTED:
    case StudentResult::REGISTRATION_ENDED:
        this->write_messages.push_back(
            Message("Can only deregister from a course during registration."));
        break;
    default:
        // TODO: Do something here.
        break;
    }
}

void StudentSession::drop_course() {
    this->write_messages.push_back(Message("Course dropped."));
}

void StudentSession::set_state() {
    std::string_view input{this->read_message.body()};

    if (input == "rfc" || input == "RFC") {
        this->state = State::REGISTER_FOR_COURSE;
    } else if (input == "dfc" || input == "DFC") {
        this->state = State::DEREGISTER_FROM_COURSE;
    } else if (input == "dac" || input == "DAC") {
        this->state = State::DROP_COURSE;
    } else {
        this->write_messages.push_back(Message("Invalid command."));
    }
}
