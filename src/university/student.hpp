#pragma once

#include <cstdint>
#include <string>

class Student {
public:
    Student(const std::uint32_t, const std::string);

    std::uint32_t id() const;

private:
    const std::uint32_t _id;
    const std::string _name;
};
