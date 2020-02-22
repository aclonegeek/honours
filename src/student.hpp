#pragma once

#include <cstdint>
#include <string>

class Student {
public:
    Student(const std::uint8_t, const std::string);

    std::uint8_t id() const;

private:
    const std::uint8_t _id;
    const std::string _name;
};
