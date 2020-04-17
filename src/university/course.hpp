#pragma once

#include <unordered_map>
#include <vector>

#include "student.hpp"

enum class StudentResult;

class Course {
public:
    Course(const std::uint32_t, const std::string, const std::uint8_t capsize);

    StudentResult register_student(Student&);
    StudentResult deregister_student(const std::uint32_t);

    bool has_student(const std::uint32_t) const;
    bool is_full() const;

    const std::vector<std::uint32_t> student_ids() const;

    std::uint32_t id() const;
    const std::string& title() const;

private:
    const std::uint32_t _id;
    const std::string _title;
    const std::uint8_t capsize;

    std::unordered_map<std::uint32_t, Student&> _students;
};
