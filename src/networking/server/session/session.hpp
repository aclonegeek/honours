#pragma once

#include <asio.hpp>
#include <deque>
#include <iostream>
#include <stdexcept>

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
    virtual void read_header() final {
        auto self(this->shared_from_this());
        asio::async_read(this->socket,
                         asio::buffer(this->read_message.data(), HEADER_LENGTH),
                         [this, self](const std::error_code error_code,
                                      const std::size_t /*length*/) {
                             if (error_code ||
                                 !this->read_message.decode_header()) {
                                 return;
                             }

                             this->read_body();
                         });
    };

    virtual void read_body() final {
        auto self(this->shared_from_this());
        asio::async_read(this->socket,
                         asio::buffer(this->read_message.body(),
                                      this->read_message.body_length()),
                         [this, self](const std::error_code error_code,
                                      const std::size_t /*length*/) {
                             if (error_code) {
                                 return;
                             }

                             if (!this->handle_input()) {
                                 std::memset(this->read_message.data(), 0,
                                             this->read_message.length());
                                 return;
                             }

                             std::memset(this->read_message.data(), 0,
                                         this->read_message.length());

                             this->read_header();
                         });
    }

    virtual void write() final {
        auto self(this->shared_from_this());
        asio::async_write(this->socket,
                          asio::buffer(this->write_messages.front().data(),
                                       this->write_messages.front().length()),
                          [this, self](const std::error_code error_code,
                                       const std::size_t /*length*/) {
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

    // The following functions are to simplify parsing integers. On failure,
    // they return 0, which is not a valid value for a course ID, student ID, or
    // capsize.

    std::uint32_t parse_course_id(const std::string& token) {
        int id;

        try {
            id = std::stoi(token);
        } catch (const std::invalid_argument& ia) {
            this->write_messages.push_back(
                Message("ERROR - Invalid input. Course ID must be a number."));
            return 0;
        } catch (const std::out_of_range& oor) {
            this->write_messages.push_back(
                Message("ERROR - Invalid input. Course ID must be 6 digits."));
            return 0;
        }

        if (id <= 99'999 || id > 999'999) {
            this->write_messages.push_back(
                Message("ERROR - Invalid input. Course ID must be 6 digits."));
            return 0;
        }

        return id;
    }

    std::uint8_t parse_course_capsize(const std::string& token) {
        int id;

        try {
            id = std::stoi(token);
        } catch (const std::invalid_argument& ia) {
            this->write_messages.push_back(
                Message("ERROR - Invalid input. Capsize must be a number."));
            return 0;
        } catch (const std::out_of_range& oor) {
            this->write_messages.push_back(
                Message("ERROR - Invalid input. Capsize must be between 1 and "
                        "255 (inclusive)."));
            return 0;
        }

        if (id < 1 || id > 255) {
            this->write_messages.push_back(
                Message("ERROR - Invalid input. Capsize must be between 1 and "
                        "255 (inclusive)."));
            return 0;
        }

        return id;
    }

    std::uint32_t parse_student_id(const std::string& token) {
        int id;

        try {
            id = std::stoi(token);
        } catch (const std::invalid_argument& ia) {
            this->write_messages.push_back(
                Message("ERROR - Invalid input. Student ID must be a number."));
            return 0;
        } catch (const std::out_of_range& oor) {
            this->write_messages.push_back(
                Message("ERROR - Invalid input. Student ID must be 9 digits."));
            return 0;
        }

        if (id <= 99'999'999 || id > 999'999'999) {
            this->write_messages.push_back(
                Message("ERROR - Invalid input. Student ID must be 9 digits."));
            return 0;
        }

        return id;
    }

    tcp::socket socket;
    Message read_message;
    std::deque<Message> write_messages;
};
