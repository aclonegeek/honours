#pragma once

#include <asio.hpp>

using asio::ip::tcp;

class Server {
public:
    Server(asio::io_context&, const tcp::endpoint&);

private:
    void accept();

    tcp::acceptor acceptor;
};
