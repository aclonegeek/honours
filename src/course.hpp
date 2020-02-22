#pragma once

#include <unordered_map>

#include "student.hpp"

class Course {
public:
    Course(const std::uint8_t, const std::string);

private:
    const std::uint8_t id;
    const std::string title;

    std::unordered_map<std::uint8_t, Student&> students;
};
