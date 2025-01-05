#pragma once

#include <fmt/core.h>
#include <string>
#include <vector>

using std::string;
using std::vector;

enum class TokenType {
  // Single-character tokens.
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  DOT,
  MINUS,
  PLUS,
  SEMICOLON,
  SLASH,
  STAR,

  // One or two character tokens.
  BANG,
  BANG_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,

  // Literals.
  IDENTIFIER,
  STRING,
  NUMBER,

  // Keywords.
  AND,
  CLASS,
  ELSE,
  FALSE,
  FUN,
  FOR,
  IF,
  NIL,
  OR,
  PRINT,
  RETURN,
  SUPER,
  THIS,
  TRUE,
  VAR,
  WHILE,

  END_OF_FILE
};

std::string tokenTypeToString(TokenType type);

struct Token {
  TokenType type;
  string lexeme;
  int line;

  inline string toString() const {
    return fmt::format("{} {}", tokenTypeToString(type), lexeme);
  }
};

class Scanner {
public:
  [[nodiscard]] explicit Scanner(const string &source);
  vector<Token> scanTokens();

private:
  bool isAtEnd() const;
  char advance();
  char peek(const int offset = 0) const;
  void error(int, const string &);
  void report(int, const string &, const string &) const;
  void scanToken();
  void addToken(TokenType);
  void handleString();
  void handleNumber();
  void handleIdentifier();

private:
  string m_source;
  vector<Token> m_tokens;
  int m_start = 0;
  int m_current = 0;
  int m_line = 1;
  int m_hadError = false;
};
