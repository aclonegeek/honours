#include "university.hpp"

University::University() {}

std::optional<Course> University::course(const std::uint8_t id) const {
    if (auto course = this->courses.find(id); course != this->courses.end()) {
        return course->second;
    }

    return {};
}

std::optional<Student> University::student(const std::uint8_t id) const {
    if (auto student = this->students.find(id);
        student != this->students.end()) {
        return student->second;
    }

    return {};
}

void University::create_course(Course course) {
    this->courses.insert({course.id(), course});
}

void University::delete_course(const std::uint8_t id) {
    this->courses.erase(id);
}

void University::register_student(const std::uint8_t id,
                                  const std::string name) {
    this->students.insert({id, Student(id, name)});
}

void University::delete_student(const std::uint8_t id) {
    this->students.erase(id);
}

bool University::register_student_in_course(const std::uint8_t course_id,
                                            const std::uint8_t student_id) {
    return this->courses.at(course_id).register_student(
        this->students.at(student_id));
}
