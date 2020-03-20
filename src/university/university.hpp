#pragma once

#include <optional>

#include "course.hpp"

constexpr uint8_t DAY_LENGTH             = 1;  // seconds
constexpr uint8_t PREREGISTRATION_LENGTH = 10; // days
constexpr uint8_t REGISTRATION_LENGTH    = 14; // days
constexpr uint8_t TERM_LENGTH            = 90; // days

class University {
public:
    University();

    std::optional<Course> course(const std::uint16_t) const;
    std::optional<Student> student(const std::uint32_t) const;

    void create_course(const std::uint16_t, const std::string&,
                       const std::uint8_t);
    void delete_course(const std::uint16_t);

    void register_student(const std::uint32_t, const std::string);
    void delete_student(const std::uint32_t);

    bool register_student_in_course(const std::uint32_t, const std::uint16_t);
    bool deregister_student_from_course(const std::uint32_t,
                                        const std::uint16_t);

private:
    void start_timers();

    enum class State {
        REGISTRATION_NOT_STARTED,
        REGISTRATION_STARTED,
        REGISTRATIONED_ENDED,
        TERM_ENDED,
    };

    State state;

    std::unordered_map<std::uint16_t, Course> courses;
    std::unordered_map<std::uint32_t, Student> students;
};
