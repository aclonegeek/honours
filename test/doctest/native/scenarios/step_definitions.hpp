#include <string>

class Client;

void the_clerk_is_logged_in(Client&);
void the_student_has_logged_in_as(Client&, const std::string&);
void there_is_an_existing_course(Client&, const std::string&);
void there_is_an_existing_student(Client&, const std::string&);
