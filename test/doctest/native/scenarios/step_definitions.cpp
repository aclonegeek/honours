#include "client.hpp"
#include "server.hpp"

void the_clerk_is_logged_in(Client& clerk) {
    clerk.send(Message("clerk"));
    clerk.send(Message("admin"));
}

void there_is_an_existing_student(Client& client, const std::string& message) {
    client.send(Message("cas"));
    client.send(Message(message));
}
