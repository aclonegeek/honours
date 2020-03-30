#pragma once

#include <unordered_map>

#include "student.hpp"

class Course {
public:
    Course(const std::uint16_t, const std::string, const std::uint8_t capsize);

    bool register_student(Student&);
    bool deregister_student(const std::uint32_t);

    bool has_student(const std::uint32_t) const;
    bool is_full() const;

    std::uint16_t id() const;

private:
    const std::uint16_t _id;
    const std::string title;
    const std::uint8_t capsize;

    std::unordered_map<std::uint32_t, Student&> students;
};
