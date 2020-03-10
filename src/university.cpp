#include <chrono>
#include <thread>

#include "university.hpp"

University::University() : state(State::REGISTRATION_NOT_STARTED) {
    this->start_timers();
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

std::optional<Course> University::course(const std::uint32_t id) const {
    if (auto course = this->courses.find(id); course != this->courses.end()) {
        return course->second;
    }

    return {};
}

std::optional<Student> University::student(const std::uint32_t id) const {
    if (auto student = this->students.find(id);
        student != this->students.end()) {
        return student->second;
    }

    return {};
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
    return this->courses.at(course_id).register_student(
        this->students.at(student_id));
}
