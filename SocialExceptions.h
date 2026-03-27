#ifndef SOCIAL_EXCEPTIONS_H
#define SOCIAL_EXCEPTIONS_H

#include <stdexcept>
#include <string>

// ── Base exception ────────────────────────────────────────────────────────────

class SocialException : public std::runtime_error {
public:
    explicit SocialException(const std::string& msg) : std::runtime_error(msg) {}
};

// ── Existing exceptions (now inherit from SocialException) ───────────────────

class InvalidContentException : public SocialException {
public:
    InvalidContentException(const std::string& msg) : SocialException(msg) {}
};

class FileIOException : public SocialException {
public:
    FileIOException(const std::string& msg) : SocialException(msg) {}
};

// ── New exceptions ────────────────────────────────────────────────────────────

class UserNotFoundException : public std::runtime_error {
public:
    explicit UserNotFoundException(const std::string& userId)
        : std::runtime_error("User not found: " + userId) {}
};

class UnauthorizedException : public std::runtime_error {
public:
    explicit UnauthorizedException(const std::string& msg)
        : std::runtime_error(msg) {}
};

class ChatNotFoundException : public std::runtime_error {
public:
    explicit ChatNotFoundException(const std::string& chatId)
        : std::runtime_error("Chat not found: " + chatId) {}
};

#endif // SOCIAL_EXCEPTIONS_H
