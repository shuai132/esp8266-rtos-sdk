#pragma once

#include <vector>
#include <cstddef>

class String {
public:
    String(const char* str = "");

    String(const char* str, size_t n);

    const char* c_str() const;
    size_t length() const;

    const char* data() const;
    size_t size() const;

    std::vector<char> _data;
};
