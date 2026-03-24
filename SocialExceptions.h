#ifndef SOCIAL_EXCEPTIONS_H
#define SOCIAL_EXCEPTIONS_H

#include <stdexcept>
#include <string>

class InvalidContentException : public std::runtime_error {
public:
    InvalidContentException(const std::string& msg) : std::runtime_error(msg) {}
};

class FileIOException : public std::runtime_error {
public:
    FileIOException(const std::string& msg) : std::runtime_error(msg) {}
};

#endif