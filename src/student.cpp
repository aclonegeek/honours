#include "student.hpp"

Student::Student(const std::uint8_t id, const std::string name)
    : _id(id), _name(name) {}

std::uint8_t Student::id() const { return this->_id; }
