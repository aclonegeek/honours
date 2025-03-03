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
    asio::async_connect(
        this->socket, endpoints,
        [this](const std::error_code error_code, const tcp::endpoint) {
            if (!error_code) {
                this->read_header();
            }
        });
}

void Client::read_header() {
    this->previous_read_message.body_length(this->read_message.body_length());

    asio::async_read(
        this->socket, asio::buffer(this->read_message.data(), HEADER_LENGTH),
        [this](const std::error_code error_code, const std::size_t /*length*/) {
            if (error_code || !this->read_message.decode_header()) {
                std::cout << "ERROR - Read header fail.\n";
                std::cout << "Message: " << this->read_message.data() << "\n";

                this->socket.close();
                return;
            }

            this->read_body();
        });
}

void Client::read_body() {
    std::memset(this->previous_read_message.body(), 0, MAX_BODY_LENGTH);
    std::strncpy(this->previous_read_message.body(), this->read_message.body(),
                 this->previous_read_message.body_length());

    asio::async_read(
        this->socket,
        asio::buffer(this->read_message.body(),
                     this->read_message.body_length()),
        [this](const std::error_code error_code, const std::size_t /*length*/) {
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
        [this](const std::error_code error_code, const std::size_t /*length*/) {
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

const char* Client::previous_message() const {
    return this->previous_read_message.body();
}
