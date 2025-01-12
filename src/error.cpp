#include "error.h"
#include <iostream>

namespace lox {
// Initialize error state
bool hadError = false;
bool hadRuntimeError = false;

void report(int line, const std::string &where,
                    const std::string &message) {
  std::cerr << "[line " << line << "] Error" << where << ": " << message
            << std::endl;
  hadError = true;
}

void error(int line, const std::string &message) {
  report(line, "", message);
}

void error(Token token, const std::string &message) {
  if (token.type == TokenType::END_OF_FILE) {
    report(token.line, " at end", message);
  } else {
    report(token.line, " at '" + token.lexeme + "'", message);
  }
}

void runtimeError(const std::string &message) {
  std::cerr << message << std::endl;
  hadRuntimeError = true;
}

void resetError() {
  hadError = false;
  hadRuntimeError = false;
}

} // namespace lox

