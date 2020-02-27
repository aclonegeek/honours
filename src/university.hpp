#pragma once

#include <optional>

#include "course.hpp"

class University {
public:
    University();

    std::optional<Course> course(const std::uint32_t) const;
    std::optional<Student> student(const std::uint32_t) const;

    void create_course(const std::uint16_t, const std::string&,
                       const std::uint8_t);
    void delete_course(const std::uint16_t);

    void register_student(const std::uint32_t, const std::string);
    void delete_student(const std::uint32_t);

    bool register_student_in_course(const std::uint16_t, const std::uint32_t);

private:
    std::unordered_map<std::uint16_t, Course> courses;
    std::unordered_map<std::uint32_t, Student> students;
};
