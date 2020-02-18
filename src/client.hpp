#pragma once

#include <asio.hpp>
#include <deque>

#include "message.hpp"

using asio::ip::tcp;

class Client {
public:
    Client(asio::io_context&, const tcp::resolver::results_type&);

    void send(const Message&);
    void close();

private:
    void connect(const tcp::resolver::results_type&);
    void read_header();
    void read_body();
    void write();

    asio::io_context& io_context;
    tcp::socket socket;

    std::deque<Message> message_queue;
    Message read_message;
};
