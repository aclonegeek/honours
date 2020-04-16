#include "student_session.hpp"
#include "result_types.hpp"
#include "university.hpp"

static const Message OPTIONS =
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
        break;
    case State::REGISTERING_FOR_COURSE:
        this->register_for_course();
        this->state = State::WAITING_FOR_ACTION;
        break;
    case State::DEREGISTERING_FROM_COURSE:
        this->deregister_from_course();
        this->state = State::WAITING_FOR_ACTION;
        break;
    case State::DROPPING_COURSE:
        this->drop_course();
        this->state = State::WAITING_FOR_ACTION;
        break;
    }

    this->write_options();

    return true;
}

void StudentSession::write_options() {
    switch (this->state) {
    case State::WAITING_FOR_ACTION:
        this->write_messages.push_back(OPTIONS);
        break;
    case State::REGISTERING_FOR_COURSE:
        [[fallthrough]];
    case State::DEREGISTERING_FROM_COURSE:
        [[fallthrough]];
    case State::DROPPING_COURSE:
        this->write_messages.push_back(Message("Enter Course ID:"));
        break;
    }

    this->write();
}

void StudentSession::register_for_course() {
    const std::uint32_t course_id =
        this->parse_course_id(this->read_message.body());

    const StudentResult result =
        this->university.register_student_in_course(this->id, course_id);

    switch (result) {
    case StudentResult::SUCCESS:
        this->write_messages.push_back(Message("Registered for course."));
        break;
    case StudentResult::COURSE_DOES_NOT_EXIST:
        this->write_messages.push_back(
            Message("ERROR - Course does not exist."));
        break;
    case StudentResult::REGISTRATION_NOT_STARTED:
        [[fallthrough]];
    case StudentResult::REGISTRATION_ENDED:
        this->write_messages.push_back(Message(
            "ERROR - Can only register for a course during registration."));
        break;
    case StudentResult::COURSE_FULL:
        this->write_messages.push_back(Message("ERROR - Course is full."));
        break;
    default:
        // TODO: Do something here.
        break;
    }
}

void StudentSession::deregister_from_course() {
    const std::uint32_t course_id =
        this->parse_course_id(this->read_message.body());

    const StudentResult result =
        this->university.deregister_student_from_course(this->id, course_id);

    switch (result) {
    case StudentResult::SUCCESS:
        this->write_messages.push_back(Message("Deregistered from course."));
        break;
    case StudentResult::COURSE_DOES_NOT_EXIST:
        this->write_messages.push_back(
            Message("ERROR - Course does not exist."));
        break;
    case StudentResult::STUDENT_NOT_REGISTERED:
        this->write_messages.push_back(
            Message("ERROR - Student is not registered in course."));
        break;
    case StudentResult::REGISTRATION_NOT_STARTED:
    case StudentResult::REGISTRATION_ENDED:
        this->write_messages.push_back(Message(
            "ERROR - Can only deregister from a course during registration."));
        break;
    default:
        // TODO: Do something here.
        break;
    }
}

void StudentSession::drop_course() {
    const std::uint32_t course_id =
        this->parse_course_id(this->read_message.body());

    const StudentResult result =
        this->university.drop_student_from_course(this->id, course_id);

    switch (result) {
    case StudentResult::SUCCESS:
        this->write_messages.push_back(Message("Dropped course."));
        break;
    case StudentResult::COURSE_DOES_NOT_EXIST:
        this->write_messages.push_back(
            Message("ERROR - Course does not exist."));
        break;
    case StudentResult::STUDENT_NOT_REGISTERED:
        this->write_messages.push_back(
            Message("ERROR - Student is not registered in the course."));
        break;
    case StudentResult::REGISTRATION_NOT_STARTED:
        [[fallthrough]];
    case StudentResult::TERM_ENDED:
        [[fallthrough]];
    case StudentResult::REGISTRATION_NOT_ENDED:
        this->write_messages.push_back(
            Message("ERROR - Can only drop a course during the term."));
        break;
    default:
        // TODO: Do something here.
        break;
    }
}

void StudentSession::set_state() {
    const std::string_view input{this->read_message.body()};

    if (input == "rfc" || input == "RFC") {
        this->state = State::REGISTERING_FOR_COURSE;
    } else if (input == "dfc" || input == "DFC") {
        this->state = State::DEREGISTERING_FROM_COURSE;
    } else if (input == "dac" || input == "DAC") {
        this->state = State::DROPPING_COURSE;
    } else {
        this->write_messages.push_back(Message("Invalid command."));
    }
}
