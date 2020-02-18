#pragma once

#include <asio.hpp>
#include <deque>

#include "message.hpp"

using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket);

    void start();
    void send(const Message&);

private:
    void read_header();
    void read_body();

    void write();

    tcp::socket socket;
    Message read_message;
    std::deque<Message> message_queue;
};
