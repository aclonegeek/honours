#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "message.hpp"

Message::Message(const std::string& message) {
    std::strncpy(_data + HEADER_LENGTH, message.data(), message.length());
    _body_length = message.length();
    this->encode_header();
}

void Message::encode_header() {
    char header[HEADER_LENGTH + 1] = "";
    std::sprintf(header, "%3d", _body_length);
    std::memcpy(_data, header, HEADER_LENGTH);
}

bool Message::decode_header() {
    char header[HEADER_LENGTH + 1] = "";
    std::strncat(header, _data, HEADER_LENGTH);
    _body_length = std::atoi(header);

    if (_body_length > MAX_BODY_LENGTH) {
        _body_length = 0;
        return false;
    }

    return true;
}

const char* Message::data() const { return _data; }

const char* Message::body() const { return _data + HEADER_LENGTH; }

char* Message::data() { return _data; }

char* Message::body() { return _data + HEADER_LENGTH; }

uint16_t Message::length() const { return HEADER_LENGTH + _body_length; }

uint16_t Message::body_length() const { return _body_length; }

void Message::body_length(const uint16_t length) {
    _body_length = length;
    if (_body_length > MAX_BODY_LENGTH) {
        _body_length = MAX_BODY_LENGTH;
    }
}
