#include "course.hpp"
#include "result_types.hpp"

Course::Course(const std::uint16_t id, const std::string title,
               const std::uint8_t capsize)
    : _id(id), title(title), capsize(capsize) {}

StudentResult Course::register_student(Student& student) {
    if (this->students.size() >= this->capsize) {
        return StudentResult::COURSE_FULL;
    }

    this->students.insert({student.id(), student});

    return StudentResult::SUCCESS;
}

StudentResult Course::deregister_student(const std::uint32_t id) {
    if (this->students.find(id) == this->students.end()) {
        return StudentResult::STUDENT_NOT_REGISTERED;
    }

    this->students.erase(id);

    return StudentResult::SUCCESS;
}

bool Course::has_student(const std::uint32_t id) const {
    return this->students.find(id) != this->students.end();
}

bool Course::is_full() const { return this->students.size() == this->capsize; }

std::uint16_t Course::id() const { return this->_id; }
