#include <iostream>

#include "message.hpp"
#include "session.hpp"

Session::Session(tcp::socket socket) : socket(std::move(socket)) {}

void Session::start() { this->read_header(); }

void Session::send(const Message& message) {
    bool can_write = this->write_messages.empty();
    this->write_messages.push_back(message);
    if (can_write) {
        this->write();
    }
}

void Session::read_header() {
    auto self(this->shared_from_this());
    asio::async_read(
        this->socket, asio::buffer(this->read_message.data(), HEADER_LENGTH),
        [this, self](std::error_code error_code, std::size_t /*length*/) {
            if (error_code || !this->read_message.decode_header()) {
                return;
            }

            this->read_body();
        });
}

void Session::read_body() {
    auto self(this->shared_from_this());
    asio::async_read(
        this->socket,
        asio::buffer(this->read_message.body(),
                     this->read_message.body_length()),
        [this, self](std::error_code error_code, std::size_t /*length*/) {
            if (error_code) {
                return;
            }

            bool can_write = this->write_messages.empty();
            this->write_messages.push_back(this->read_message);
            if (can_write) {
                this->write();
            }

            this->read_header();
        });
}

void Session::write() {
    auto self(this->shared_from_this());
    asio::async_write(
        this->socket,
        asio::buffer(this->write_messages.front().data(),
                     this->write_messages.front().length()),
        [this, self](std::error_code error_code, std::size_t /*length*/) {
            if (error_code) {
                return;
            }

            // TODO: Don't use cerr for this...
            std::cerr << this->write_messages.front().data();

            this->write_messages.pop_front();
            if (!this->write_messages.empty()) {
                this->write();
            }
        });
}
