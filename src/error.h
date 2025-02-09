#pragma once
#include <stdexcept>
#include "Token.h"


class RuntimeError : public std::runtime_error {
public:
    Token m_token;
    RuntimeError(const Token& token, const std::string& message) : m_token(token), std::runtime_error(message) {}
};

namespace lox {
// Global error state
extern bool hadError;
extern bool hadRuntimeError;

// Error reporting functions
void report(int line, const std::string &where, const std::string &message);
void error(int line, const std::string &message);
void error(const Token& token, const std::string &message, bool isRuntime = false);

// Error state management
void resetError();
} // namespace lox
