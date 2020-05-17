#include <cstring>
#include "String.h"

String::String(const char* str) {
    auto len = strlen(str) + 1;
    _data.resize(len);
    memcpy(_data.data(), str, len);
}

const char* String::c_str() const {
    return (_data.data());
}

size_t String::length() const  {
    return _data.size() - 1;
}
