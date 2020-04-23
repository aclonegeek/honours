#pragma once

#include <optional>

#include "course.hpp"
#include "state_change_timer_task.hpp"

enum class ClerkResult;
enum class StudentResult;

constexpr std::uint8_t DAY_LENGTH             = 1;  // seconds
constexpr std::uint8_t PREREGISTRATION_LENGTH = 5;  // DAY_LENGTHs
constexpr std::uint8_t REGISTRATION_LENGTH    = 10; // DAY_LENGTHs
constexpr std::uint8_t TERM_LENGTH            = 10; // DAY_LENGTHs

enum class State {
    PREREGISTRATION,
    REGISTRATION,
    TERM,
    END,
};

class University {
public:
    University();

    ClerkResult create_course(const std::uint32_t, const std::string&,
                              const std::uint8_t);
    ClerkResult delete_course(const std::uint32_t);
    ClerkResult register_student(const std::uint32_t, const std::string);
    ClerkResult delete_student(const std::uint32_t);

    StudentResult register_student_in_course(const std::uint32_t,
                                             const std::uint32_t);
    StudentResult deregister_student_from_course(const std::uint32_t,
                                                 const std::uint32_t);
    StudentResult drop_student_from_course(const std::uint32_t,
                                           const std::uint32_t);

    const std::optional<Course> course(const std::uint32_t) const;
    const std::optional<Student> student(const std::uint32_t) const;

    const std::unordered_map<std::uint32_t, Course>& courses();
    const std::unordered_map<std::uint32_t, Student>& students();

private:
    StateChangeTimerTask registration_timer;
    StateChangeTimerTask term_timer;
    StateChangeTimerTask end_timer;

    State state;

    std::unordered_map<std::uint32_t, Course> _courses;
    std::unordered_map<std::uint32_t, Student> _students;
};
