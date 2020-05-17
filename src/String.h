#pragma once

#include <vector>
#include <cstddef>

class String {
public:
    String(const char* str = "");

    const char* c_str() const;

    size_t length() const;

    std::vector<char> _data;
};
