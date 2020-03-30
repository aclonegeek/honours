#include <chrono>
#include <thread>

#include "university.hpp"

University::University() : state(State::REGISTRATION_NOT_STARTED) {
    this->start_timers();

    // For testing purposes.
    this->courses.insert({11111, Course(11111, "Quack", 1)});
    this->students.insert({111111111, Student(111111111, "joe")});
    this->students.insert({222222222, Student(222222222, "murphy")});
}

void University::start_timers() {
    std::thread([&]() {
        std::this_thread::sleep_for(
            std::chrono::seconds(DAY_LENGTH * PREREGISTRATION_LENGTH));
        this->state = State::REGISTRATION_STARTED;
    }).detach();

    std::thread([&]() {
        std::this_thread::sleep_for(std::chrono::seconds(
            DAY_LENGTH * (PREREGISTRATION_LENGTH + REGISTRATION_LENGTH)));
        this->state = State::REGISTRATIONED_ENDED;
    }).detach();

    std::thread([&]() {
        std::this_thread::sleep_for(std::chrono::seconds(
            DAY_LENGTH *
            (PREREGISTRATION_LENGTH + REGISTRATION_LENGTH + TERM_LENGTH)));
        this->state = State::TERM_ENDED;
    }).detach();
}

void University::create_course(const std::uint16_t id, const std::string& title,
                               const std::uint8_t capsize) {
    this->courses.insert({id, Course(id, title, capsize)});
}

void University::delete_course(const std::uint16_t id) {
    this->courses.erase(id);
}

void University::register_student(const std::uint32_t id,
                                  const std::string name) {
    this->students.insert({id, Student(id, name)});
}

void University::delete_student(const std::uint32_t id) {
    this->students.erase(id);
}

bool University::register_student_in_course(const std::uint32_t student_id,
                                            const std::uint16_t course_id) {
    if (this->courses.find(course_id) == this->courses.end()) {
        return false;
    }

    if (this->state != State::REGISTRATION_STARTED) {
        return false;
    }

    return this->courses.at(course_id).register_student(
        this->students.at(student_id));
}

bool University::deregister_student_from_course(const std::uint32_t student_id,
                                                const std::uint16_t course_id) {
    return this->courses.at(course_id).deregister_student(student_id);
}

const std::optional<Course> University::course(const std::uint16_t id) const {
    if (auto course = this->courses.find(id); course != this->courses.end()) {
        return course->second;
    }

    return {};
}

const std::optional<Student> University::student(const std::uint32_t id) const {
    if (auto student = this->students.find(id);
        student != this->students.end()) {
        return student->second;
    }

    return {};
}

const std::string University::get_state() const {
    switch (this->state) {
    case State::REGISTRATION_NOT_STARTED:
        return "REGISTRATION_NOT_STARTED";
    case State::REGISTRATION_STARTED:
        return "REGISTRATION_STARTED";
    case State::REGISTRATIONED_ENDED:
        return "REGISTRATION_ENDED";
    case State::TERM_ENDED:
        return "TERM_ENDED";
    }
}
