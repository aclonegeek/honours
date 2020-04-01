#pragma once

enum class ClerkResult {
    SUCCESS,

    // Errors.
    COURSE_EXISTS,
    COURSE_DOES_NOT_EXIST,

    STUDENT_EXISTS,
    STUDENT_DOES_NOT_EXIST,
};

enum class StudentResult {
    SUCCESS,

    // Errors.
    COURSE_DOES_NOT_EXIST,
    COURSE_FULL,

    REGISTRATION_NOT_STARTED,
    REGISTRATION_NOT_ENDED,
    REGISTRATION_ENDED,

    STUDENT_NOT_REGISTERED,

    TERM_ENDED,
};
