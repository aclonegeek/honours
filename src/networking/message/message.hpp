#pragma once

#include <cstdint>
#include <string>

constexpr uint8_t HEADER_LENGTH    = 3;
constexpr uint16_t MAX_BODY_LENGTH = 512;

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

    uint16_t length() const;
    uint16_t body_length() const;
    void body_length(const uint16_t);

private:
    char _data[HEADER_LENGTH + MAX_BODY_LENGTH] = {};
    uint16_t _body_length;
};
