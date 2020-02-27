#pragma once

#include <optional>

#include "course.hpp"

class University {
public:
    University();

    std::optional<Course> course(const std::uint8_t) const;
    std::optional<Student> student(const std::uint8_t) const;

    void create_course(Course course);
    void delete_course(const std::uint8_t);

    void register_student(const std::uint8_t, const std::string);
    void delete_student(const std::uint8_t);

    bool register_student_in_course(const std::uint8_t, const std::uint8_t);

private:
    std::unordered_map<std::uint8_t, Course> courses;
    std::unordered_map<std::uint8_t, Student> students;
};
