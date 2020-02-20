#pragma once

#include <asio.hpp>
#include <deque>

#include "message.hpp"

using asio::ip::tcp;

class Session {
public:
    Session(tcp::socket socket) : socket(std::move(socket)) {}

    virtual void start() { this->read_header(); }

    virtual void send(const Message& message) {
        bool can_write = this->write_messages.empty();
        this->write_messages.push_back(message);
        if (can_write) {
            this->write();
        }
    }

    virtual ~Session() {}

protected:
    // TODO: Find a way to actually implement these here, instead of having
    // duplicate code for each session.
    virtual void read_header()  = 0;
    virtual void read_body()    = 0;
    virtual void write()        = 0;
    virtual void handle_input() = 0;

    tcp::socket socket;
    Message read_message;
    std::deque<Message> write_messages;
};
