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
    this->courses.insert({11111, Course(11111, "Quack", 1)});
    this->students.insert({112233445, Student(112233445, "joe")});
    this->students.insert({111222334, Student(111222334, "murphy")});
}

ClerkResult University::create_course(const std::uint32_t id,
                                      const std::string& title,
                                      const std::uint8_t capsize) {
    if (this->state != State::PREREGISTRATION) {
        return ClerkResult::PREREGISTRATION_ENDED;
    }

    if (this->courses.find(id) != this->courses.end()) {
        return ClerkResult::COURSE_EXISTS;
    }

    this->courses.insert({id, Course(id, title, capsize)});
    return ClerkResult::SUCCESS;
}

ClerkResult University::delete_course(const std::uint32_t id) {
    if (this->state != State::PREREGISTRATION) {
        return ClerkResult::PREREGISTRATION_ENDED;
    }

    if (this->courses.find(id) == this->courses.end()) {
        return ClerkResult::COURSE_DOES_NOT_EXIST;
    }

    this->courses.erase(id);
    return ClerkResult::SUCCESS;
}

ClerkResult University::register_student(const std::uint32_t id,
                                         const std::string name) {
    if (this->state != State::PREREGISTRATION) {
        return ClerkResult::PREREGISTRATION_ENDED;
    }

    if (this->students.find(id) != this->students.end()) {
        return ClerkResult::STUDENT_EXISTS;
    }

    this->students.insert({id, Student(id, name)});
    return ClerkResult::SUCCESS;
}

ClerkResult University::delete_student(const std::uint32_t id) {
    if (this->state != State::PREREGISTRATION) {
        return ClerkResult::PREREGISTRATION_ENDED;
    }

    if (this->students.find(id) == this->students.end()) {
        return ClerkResult::STUDENT_DOES_NOT_EXIST;
    }

    this->students.erase(id);
    return ClerkResult::SUCCESS;
}

StudentResult
University::register_student_in_course(const std::uint32_t student_id,
                                       const std::uint32_t course_id) {
    if (this->courses.find(course_id) == this->courses.end()) {
        return StudentResult::COURSE_DOES_NOT_EXIST;
    }

    switch (this->state) {
    case State::PREREGISTRATION:
        return StudentResult::REGISTRATION_NOT_STARTED;
    case State::REGISTRATION:
        return this->courses.at(course_id).register_student(
            this->students.at(student_id));
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
    if (this->courses.find(course_id) == this->courses.end()) {
        return StudentResult::COURSE_DOES_NOT_EXIST;
    }

    switch (this->state) {
    case State::PREREGISTRATION:
        return StudentResult::REGISTRATION_NOT_STARTED;
    case State::REGISTRATION:
        return this->courses.at(course_id).deregister_student(student_id);
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
    if (this->courses.find(course_id) == this->courses.end()) {
        return StudentResult::COURSE_DOES_NOT_EXIST;
    }

    switch (this->state) {
    case State::PREREGISTRATION:
        return StudentResult::REGISTRATION_NOT_STARTED;
    case State::REGISTRATION:
        return StudentResult::REGISTRATION_NOT_ENDED;
    case State::TERM:
        return this->courses.at(course_id).deregister_student(student_id);
    case State::END:
        return StudentResult::TERM_ENDED;
    }

    unreachable();
}

const std::optional<Course> University::course(const std::uint32_t id) const {
    if (auto course = this->courses.find(id); course != this->courses.end()) {
        return course->second;
    }

    return {};
}

const std::optional<Student> University::student(const std::uint32_t id) const {
    if (auto student = this->students.find(id);
        student != this->students.end()) {
        return student->second;
    }

    return {};
}
