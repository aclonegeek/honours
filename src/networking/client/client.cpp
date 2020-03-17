#include <cstring>
#include <iostream>

#include "client.hpp"

Client::Client(asio::io_context& io_context,
               const tcp::resolver::results_type& endpoints)
    : io_context(io_context), socket(io_context) {
    this->connect(endpoints);
}

void Client::send(const Message& message) {
    asio::post(this->io_context, [this, message]() {
        const bool can_write = this->write_messages.empty();
        this->write_messages.push_back(message);
        if (can_write) {
            this->write();
        }
    });
}

void Client::close() {
    asio::post(this->io_context, [this]() { this->socket.close(); });
}

void Client::connect(const tcp::resolver::results_type& endpoints) {
    asio::async_connect(this->socket, endpoints,
                        [this](std::error_code error_code, tcp::endpoint) {
                            if (!error_code) {
                                this->read_header();
                            }
                        });
}

void Client::read_header() {
    asio::async_read(
        this->socket, asio::buffer(this->read_message.data(), HEADER_LENGTH),
        [this](std::error_code error_code, std::size_t /*length*/) {
            if (error_code || !this->read_message.decode_header()) {
                // TODO: Log this.
                std::cout << "message: " << this->read_message.data() << "\n";
                std::cout << "read header fail\n";
                this->socket.close();
                return;
            }

            this->read_body();
        });
}

void Client::read_body() {
    asio::async_read(
        this->socket,
        asio::buffer(this->read_message.body(),
                     this->read_message.body_length()),
        [this](std::error_code error_code, std::size_t /*length*/) {
            if (error_code) {
                this->socket.close();
                return;
            }

            std::cout.write(this->read_message.body(),
                            this->read_message.body_length());
            std::cout << "\n";

            this->read_header();
        });
}

void Client::write() {
    asio::async_write(
        this->socket,
        asio::buffer(this->write_messages.front().data(),
                     this->write_messages.front().length()),
        [this](std::error_code error_code, std::size_t /*length*/) {
            if (error_code) {
                this->socket.close();
                return;
            }

            this->write_messages.pop_front();
            if (!this->write_messages.empty()) {
                this->write();
            }
        });
}

// TODO : More error handling, probably extract args parsing out into a function
// and return the port.
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: client <host> <port>\n";
        return 1;
    }

    try {
        asio::io_context io_context;

        tcp::resolver resolver{io_context};
        const auto endpoints = resolver.resolve(argv[1], argv[2]);
        Client client{io_context, endpoints};

        std::thread t([&io_context]() { io_context.run(); });

        char line[MAX_BODY_LENGTH + 1];
        while (std::cin.getline(line, MAX_BODY_LENGTH + 1)) {
            Message message(line);
            client.send(message);
        }

        client.close();
        t.join();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
