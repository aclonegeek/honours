#pragma once

#include <unordered_map>

#include "student.hpp"

class Course {
public:
    Course(const std::uint8_t, const std::string, const std::uint8_t capsize);

    bool register_student(Student&);

    std::uint8_t id() const;

private:
    const std::uint8_t _id;
    const std::string title;
    const std::uint8_t capsize;

    std::unordered_map<std::uint8_t, Student&> students;
};
