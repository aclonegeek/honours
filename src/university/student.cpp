#include "student.hpp"

Student::Student(const std::uint32_t id, const std::string name)
    : _id(id), _name(name) {}

std::uint32_t Student::id() const { return this->_id; }
