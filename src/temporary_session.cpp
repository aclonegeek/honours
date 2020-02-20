#include <iostream>

#include "temporary_session.hpp"

TemporarySession::TemporarySession(tcp::socket socket)
    : Session(std::move(socket)) {}

void TemporarySession::handle_input() {
    std::string input(this->write_messages.front().data());
    this->write_messages.pop_front();
}
