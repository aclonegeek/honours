#include "course.hpp"
#include "result_types.hpp"

Course::Course(const std::uint16_t id, const std::string title,
               const std::uint8_t capsize)
    : _id(id), title(title), capsize(capsize) {}

StudentResult Course::register_student(Student& student) {
    if (this->_students.size() >= this->capsize) {
        return StudentResult::COURSE_FULL;
    }

    this->_students.insert({student.id(), student});

    return StudentResult::SUCCESS;
}

StudentResult Course::deregister_student(const std::uint32_t id) {
    if (this->_students.find(id) == this->_students.end()) {
        return StudentResult::STUDENT_NOT_REGISTERED;
    }

    this->_students.erase(id);

    return StudentResult::SUCCESS;
}

bool Course::has_student(const std::uint32_t id) const {
    return this->_students.find(id) != this->_students.end();
}

bool Course::is_full() const { return this->_students.size() == this->capsize; }

const std::vector<std::uint32_t> Course::student_ids() const {
    std::vector<std::uint32_t> student_ids;
    student_ids.reserve(this->_students.size());

    for (const auto& student : this->_students) {
        student_ids.push_back(student.second.id());
    }

    return student_ids;
}

std::uint16_t Course::id() const { return this->_id; }
