#pragma once

#include <asio.hpp>
#include <deque>

#include "message.hpp"

using asio::ip::tcp;

class TemporarySession : public std::enable_shared_from_this<TemporarySession> {
public:
    TemporarySession(tcp::socket);

    void start();
    void send(const Message&);

private:
    void read_header();
    void read_body();

    void handle_input();

    void write();

    tcp::socket socket;
    Message read_message;
    std::deque<Message> write_messages;
};
