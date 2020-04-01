#include <string>

class Client;

constexpr std::string_view host = "localhost";
constexpr std::string_view port = "5001";

enum class WaitUntil {
    REGISTRATION_STARTS,
    REGISTRATION_ENDS,
    TERM_ENDS,
    CUSTOM,
};

void send(Client&, const std::string&);

void wait(const WaitUntil, const uint8_t = 0);

void the_clerk_is_logged_in(Client&);
void the_student_has_logged_in_as(Client&, const std::string&);
void there_is_an_existing_course(Client&, const std::string&);
void there_is_an_existing_student(Client&, const std::string&);

void the_student_has_registered_in(Client&, const std::string&);
