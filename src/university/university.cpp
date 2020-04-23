#include "university.hpp"
#include "result_types.hpp"

#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    #define unreachable() __builtin_unreachable()
#elif defined(_MSC_VER)
    #define unreachable() __assume(0)
#endif

University::University()
    : registration_timer(
          this->state, State::REGISTRATION,
          std::chrono::seconds(DAY_LENGTH * PREREGISTRATION_LENGTH)),
      term_timer(this->state, State::TERM,
                 std::chrono::seconds(DAY_LENGTH * (PREREGISTRATION_LENGTH +
                                                    REGISTRATION_LENGTH))),
      end_timer(this->state, State::END,
                std::chrono::seconds(DAY_LENGTH *
                                     (PREREGISTRATION_LENGTH +
                                      REGISTRATION_LENGTH + TERM_LENGTH))),
      state(State::PREREGISTRATION) {
    // For testing purposes.
    // this->_courses.insert({111111, Course(111111, "Quack", 1)});
    // this->_courses.insert({123321, Course(111111, "Fiddle Worth", 1)});
    // this->_courses.insert({777777, Course(111111, "Hoi Noi Boi", 1)});
    // this->_students.insert({112233445, Student(112233445, "joe")});
    // this->_students.insert({111222334, Student(111222334, "murphy")});

    // this->_courses.at(111111).register_student(this->_students.at(112233445));
    // this->_courses.at(111111).register_student(this->_students.at(111222334));

    // this->_courses.at(123321).register_student(this->_students.at(112233445));
}

ClerkResult University::create_course(const std::uint32_t id,
                                      const std::string& title,
                                      const std::uint8_t capsize) {
    if (this->state != State::PREREGISTRATION) {
        return ClerkResult::PREREGISTRATION_ENDED;
    }

    if (this->_courses.find(id) != this->_courses.end()) {
        return ClerkResult::COURSE_EXISTS;
    }

    this->_courses.insert({id, Course(id, title, capsize)});
    return ClerkResult::SUCCESS;
}

ClerkResult University::delete_course(const std::uint32_t id) {
    if (this->state != State::PREREGISTRATION) {
        return ClerkResult::PREREGISTRATION_ENDED;
    }

    if (this->_courses.find(id) == this->_courses.end()) {
        return ClerkResult::COURSE_DOES_NOT_EXIST;
    }

    this->_courses.erase(id);
    return ClerkResult::SUCCESS;
}

ClerkResult University::register_student(const std::uint32_t id,
                                         const std::string name) {
    if (this->state != State::PREREGISTRATION) {
        return ClerkResult::PREREGISTRATION_ENDED;
    }

    if (this->_students.find(id) != this->_students.end()) {
        return ClerkResult::STUDENT_EXISTS;
    }

    this->_students.insert({id, Student(id, name)});
    return ClerkResult::SUCCESS;
}

ClerkResult University::delete_student(const std::uint32_t id) {
    if (this->state != State::PREREGISTRATION) {
        return ClerkResult::PREREGISTRATION_ENDED;
    }

    if (this->_students.find(id) == this->_students.end()) {
        return ClerkResult::STUDENT_DOES_NOT_EXIST;
    }

    this->_students.erase(id);
    return ClerkResult::SUCCESS;
}

StudentResult
University::register_student_in_course(const std::uint32_t student_id,
                                       const std::uint32_t course_id) {
    if (this->_courses.find(course_id) == this->_courses.end()) {
        return StudentResult::COURSE_DOES_NOT_EXIST;
    }

    switch (this->state) {
    case State::PREREGISTRATION:
        return StudentResult::REGISTRATION_NOT_STARTED;
    case State::REGISTRATION:
        return this->_courses.at(course_id).register_student(
            this->_students.at(student_id));
    case State::TERM:
        [[fallthrough]];
    case State::END:
        return StudentResult::REGISTRATION_ENDED;
    }

    unreachable();
}

StudentResult
University::deregister_student_from_course(const std::uint32_t student_id,
                                           const std::uint32_t course_id) {
    if (this->_courses.find(course_id) == this->_courses.end()) {
        return StudentResult::COURSE_DOES_NOT_EXIST;
    }

    switch (this->state) {
    case State::PREREGISTRATION:
        return StudentResult::REGISTRATION_NOT_STARTED;
    case State::REGISTRATION:
        return this->_courses.at(course_id).deregister_student(student_id);
    case State::TERM:
        [[fallthrough]];
    case State::END:
        return StudentResult::REGISTRATION_ENDED;
    }

    unreachable();
}

StudentResult
University::drop_student_from_course(const std::uint32_t student_id,
                                     const std::uint32_t course_id) {
    if (this->_courses.find(course_id) == this->_courses.end()) {
        return StudentResult::COURSE_DOES_NOT_EXIST;
    }

    switch (this->state) {
    case State::PREREGISTRATION:
        return StudentResult::REGISTRATION_NOT_STARTED;
    case State::REGISTRATION:
        return StudentResult::REGISTRATION_NOT_ENDED;
    case State::TERM:
        return this->_courses.at(course_id).deregister_student(student_id);
    case State::END:
        return StudentResult::TERM_ENDED;
    }

    unreachable();
}

const std::optional<Course> University::course(const std::uint32_t id) const {
    if (auto course = this->_courses.find(id); course != this->_courses.end()) {
        return course->second;
    }

    return {};
}

const std::optional<Student> University::student(const std::uint32_t id) const {
    if (auto student = this->_students.find(id);
        student != this->_students.end()) {
        return student->second;
    }

    return {};
}

const std::unordered_map<std::uint32_t, Course>& University::courses() {
    return this->_courses;
}

const std::unordered_map<std::uint32_t, Student>& University::students() {
    return this->_students;
}
