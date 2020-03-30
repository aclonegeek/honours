#include "course.hpp"

Course::Course(const std::uint16_t id, const std::string title,
               const std::uint8_t capsize)
    : _id(id), title(title), capsize(capsize) {}

bool Course::register_student(Student& student) {
    if (this->students.size() >= this->capsize) {
        return false;
    }

    this->students.insert({student.id(), student});

    return true;
}

bool Course::deregister_student(const std::uint32_t id) {
    if (this->students.find(id) == this->students.end()) {
        return false;
    }

    this->students.erase(id);

    return true;
}

bool Course::has_student(const std::uint32_t id) const {
    return this->students.find(id) != this->students.end();
}

bool Course::is_full() const { return this->students.size() == this->capsize; }

std::uint16_t Course::id() const { return this->_id; }
