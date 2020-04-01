#pragma once

#include <optional>

#include "course.hpp"

enum class StudentResult;

constexpr uint8_t DAY_LENGTH             = 1;  // seconds
constexpr uint8_t PREREGISTRATION_LENGTH = 1;  // days
constexpr uint8_t REGISTRATION_LENGTH    = 10; // days
constexpr uint8_t TERM_LENGTH            = 10; // days

class University {
public:
    University();

    void create_course(const std::uint16_t, const std::string&,
                       const std::uint8_t);
    void delete_course(const std::uint16_t);

    void register_student(const std::uint32_t, const std::string);
    void delete_student(const std::uint32_t);

    StudentResult register_student_in_course(const std::uint32_t,
                                             const std::uint16_t);
    StudentResult deregister_student_from_course(const std::uint32_t,
                                                 const std::uint16_t);

    const std::optional<Course> course(const std::uint16_t) const;
    const std::optional<Student> student(const std::uint32_t) const;

    const std::string get_state() const;

private:
    void start_timers();

    enum class State {
        PREREGISTRATION,
        REGISTRATION,
        TERM,
        END,
    };

    State state;

    std::unordered_map<std::uint16_t, Course> courses;
    std::unordered_map<std::uint32_t, Student> students;
};
