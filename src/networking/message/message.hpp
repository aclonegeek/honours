#pragma once

#include <cstdint>
#include <string>

constexpr std::uint8_t HEADER_LENGTH    = 3;
constexpr std::uint16_t MAX_BODY_LENGTH = 512;

class Message {
public:
    Message() : _body_length(0) {}
    Message(const std::string&);

    void encode_header();
    bool decode_header();

    const char* data() const;
    const char* body() const;

    char* data();
    char* body();

    std::uint16_t length() const;
    std::uint16_t body_length() const;
    void body_length(const std::uint16_t);

private:
    char _data[HEADER_LENGTH + MAX_BODY_LENGTH] = {};
    std::uint16_t _body_length;
};
