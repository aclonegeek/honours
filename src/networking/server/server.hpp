#pragma once

#include <asio.hpp>

class University;

using asio::ip::tcp;

class Server {
public:
    // Server(asio::io_context&, const tcp::endpoint&);
    Server(asio::io_context&, const tcp::endpoint&, University&);

private:
    void accept();

    tcp::acceptor acceptor;

    University& university;
};
