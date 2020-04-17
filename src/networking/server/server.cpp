#include <iostream>

#include "server.hpp"
#include "session/temporary_session.hpp"

class University;

Server::Server(asio::io_context& io_context, const tcp::endpoint& endpoint,
               University& university)
    : acceptor(io_context, endpoint), university(university) {
    this->accept();
}

void Server::accept() {
    this->acceptor.async_accept([this](const std::error_code error_code,
                                       tcp::socket socket) {
        if (!error_code) {
            std::cout << "Client connected.\n";
            std::make_shared<TemporarySession>(std::move(socket), university)
                ->start();
        }

        this->accept();
    });
}
