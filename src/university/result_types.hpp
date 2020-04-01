#pragma once

enum class StudentResult {
    SUCCESS,

    // Errors.
    // General.
    COURSE_DOES_NOT_EXIST,

    // Registration errors.
    REGISTRATION_NOT_STARTED,
    COURSE_FULL,

    // Deregistration errors.
    REGISTRATION_ENDED,
    STUDENT_DOES_NOT_EXIST,

    // Drop errors.
    REGISTRATION_NOT_ENDED,
    TERM_ENDED,
};
