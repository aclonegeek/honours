#pragma once

#include <asio.hpp>
#include <deque>
#include <iostream>

#include "message.hpp"

using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket) : socket(std::move(socket)) {}

    virtual void start() { this->read_header(); }

    virtual void send(const Message& message) {
        bool can_write = this->write_messages.empty();
        this->write_messages.push_back(message);
        if (can_write) {
            this->write();
        }
    }

    virtual ~Session() {}

protected:
    virtual void read_header() {
        auto self(this->shared_from_this());
        asio::async_read(
            this->socket,
            asio::buffer(this->read_message.data(), HEADER_LENGTH),
            [this, self](std::error_code error_code, std::size_t /*length*/) {
                if (error_code || !this->read_message.decode_header()) {
                    return;
                }

                this->read_body();
            });
    };

    virtual void read_body() {
        auto self(this->shared_from_this());
        asio::async_read(
            this->socket,
            asio::buffer(this->read_message.body(),
                         this->read_message.body_length()),
            [this, self](std::error_code error_code, std::size_t /*length*/) {
                if (error_code) {
                    return;
                }

                // TODO: Remove this (log it instead?).
                std::cerr << this->read_message.body() << "\n";

                if (!this->handle_input()) {
                    return;
                }

                std::memset(this->read_message.data(), 0,
                            this->read_message.length());

                this->read_header();
            });
    }

    virtual void write() {
        auto self(this->shared_from_this());
        asio::async_write(
            this->socket,
            asio::buffer(this->write_messages.front().data(),
                         this->write_messages.front().length()),
            [this, self](std::error_code error_code, std::size_t /*length*/) {
                if (error_code) {
                    return;
                }

                this->write_messages.pop_front();
                if (!this->write_messages.empty()) {
                    this->write();
                }
            });
    }

    virtual bool handle_input() = 0;

    tcp::socket socket;
    Message read_message;
    std::deque<Message> write_messages;
};
