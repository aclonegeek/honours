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

    const char* const previous_message() const;

private:
    void connect(const tcp::resolver::results_type&);
    void read_header();
    void read_body();
    void write();

    asio::io_context& io_context;
    tcp::socket socket;

    Message read_message;
    Message previous_read_message;
    std::deque<Message> write_messages;
};
