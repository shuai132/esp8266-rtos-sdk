#include <cstring>
#include "String.h"

String::String(const char* str) {
    auto len = strlen(str) + 1;
    _data.resize(len);
    memcpy(_data.data(), str, len);
}

String::String(const char* str, size_t n) {
    auto len = n + 1;
    _data.resize(len);
    memcpy(_data.data(), str, n);
    _data[n] = '\0';
}

const char* String::c_str() const {
    return _data.data();
}

size_t String::length() const  {
    return _data.size() - 1;
}

const char* String::data() const {
    return _data.data();
}

size_t String::size() const  {
    return _data.size() - 1;
}
