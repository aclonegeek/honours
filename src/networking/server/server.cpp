#include <iostream>

#include "server.hpp"
#include "session/temporary_session.hpp"

Server::Server(asio::io_context& io_context, const tcp::endpoint& endpoint)
    : acceptor(io_context, endpoint) {
    this->accept();
}

void Server::accept() {
    this->acceptor.async_accept([this](std::error_code error_code,
                                       tcp::socket socket) {
        if (!error_code) {
            // TODO: Don't use cerr for this...
            std::cerr << "Client connected.\n";
            std::make_shared<TemporarySession>(std::move(socket), university)
                ->start();
        }

        this->accept();
    });
}

// TODO: More error handling, probably extract args parsing out into a
// function and return the port.
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: server <port>\n";
        return 1;
    }

    try {
        asio::io_context io_context;
        tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[1]));
        Server server{io_context, endpoint};

        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
