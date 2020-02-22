#pragma once

#include <cstdint>
#include <string>

class Student {
public:
    Student(const std::uint8_t, const std::string);

private:
    const std::uint8_t id;
    const std::string name;
};
