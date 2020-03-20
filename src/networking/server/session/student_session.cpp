#include "student_session.hpp"

const Message OPTIONS =
    Message("Options: Register for Course (RFC), Deregister from Course (DFC), "
            "Drop a Course (DAC).");

StudentSession::StudentSession(tcp::socket socket,
                               University& university,
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
    // TODO: We can do better here. Message itself can probably store a string
    // or something.
    std::string input(this->read_message.body());

    switch (this->state) {
    case State::WAITING_FOR_ACTION:
        std::transform(input.begin(), input.end(), input.begin(), ::tolower);

        if (input == "rfc") {
            this->state = State::REGISTER_FOR_COURSE;
            this->write_messages.push_back(Message("Enter Course ID:"));
        } else if (input == "dfc") {
            this->state = State::DEREGISTER_FROM_COURSE;
            this->write_messages.push_back(Message("Enter Course ID:"));
        } else if (input == "dac") {
            this->state = State::DROP_COURSE;
            this->write_messages.push_back(Message("Enter Course ID:"));
        } else {
            this->write_messages.push_back(Message("Invalid command."));
            this->write_messages.push_back(OPTIONS);
        }

        break;
    case State::REGISTER_FOR_COURSE:
        if (!this->register_for_course(input)) {
            break;
        }

        this->state = State::WAITING_FOR_ACTION;
        this->write_messages.push_back(Message("Registered for course."));
        this->write_messages.push_back(OPTIONS);

        break;
    case State::DEREGISTER_FROM_COURSE:
        if (!this->deregister_from_course(input)) {
            break;
        }

        this->state = State::WAITING_FOR_ACTION;
        this->write_messages.push_back(Message("Deregistered from course."));
        this->write_messages.push_back(OPTIONS);

        break;
    case State::DROP_COURSE:
        if (!this->drop_course(input)) {
            break;
        }

        this->state = State::WAITING_FOR_ACTION;
        this->write_messages.push_back(Message("Course dropped."));
        this->write_messages.push_back(OPTIONS);

        break;
    }

    this->write();

    return true;
}

bool StudentSession::register_for_course(const std::string& input) {
    const std::uint16_t course_id = std::stoi(input);
    return this->university.register_student_in_course(this->id, course_id);
}

bool StudentSession::deregister_from_course(const std::string& input) {
    return true;
}

bool StudentSession::drop_course(const std::string& input) { return true; }
