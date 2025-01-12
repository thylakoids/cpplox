#pragma once

#include <string>
#include <vector>
#include "Token.h"

using std::string;
using std::vector;


class Scanner {
public:
  [[nodiscard]] explicit Scanner(const string &source);
  vector<Token> scanTokens();

private:
  bool isAtEnd() const;
  char advance();
  char peek(const int offset = 0) const;
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
};
