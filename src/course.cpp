#include "course.hpp"

Course::Course(const std::uint8_t id, const std::string title,
               const std::uint8_t capsize)
    : _id(id), title(title), capsize(capsize) {}

bool Course::register_student(Student& student) {
    if (this->students.size() >= this->capsize) {
        return false;
    }

    this->students.insert({student.id(), student});

    return true;
}

std::uint8_t Course::id() const { return this->_id; }
