#include "university.hpp"

University::University() {}

std::optional<Course> University::course(const std::uint8_t) { return {}; }

std::optional<Student> University::student(const std::uint8_t) { return {}; }

bool University::create_course() { return true; }

bool University::register_student(const std::uint8_t id,
                                  const std::string name) {
    return true;
}

bool University::register_student_in_course(const std::uint8_t course_id,
                                            const std::uint8_t student_id) {
    return true;
}
