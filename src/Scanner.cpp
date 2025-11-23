#include "Scanner.h"
#include "error.h"
#include <fmt/core.h>
#include <map>

using lox::error;

std::map<string, TokenType> keywords = {
    {"and", TokenType::AND},       {"class", TokenType::CLASS},
    {"else", TokenType::ELSE},     {"false", TokenType::FALSE},
    {"fun", TokenType::FUN},       {"for", TokenType::FOR},
    {"if", TokenType::IF},         {"nil", TokenType::NIL},
    {"or", TokenType::OR},         {"print", TokenType::PRINT},
    {"return", TokenType::RETURN}, {"super", TokenType::SUPER},
    {"this", TokenType::THIS},     {"true", TokenType::TRUE},
    {"var", TokenType::VAR},       {"while", TokenType::WHILE},
    {"break", TokenType::BREAK},   {"continue", TokenType::CONTINUE},
};

Scanner::Scanner(const string &source) : m_source(source) {}

vector<Token> Scanner::scanTokens() {
  while (!isAtEnd()) {
    m_start = m_current;
    scanToken();
  }
  m_tokens.push_back(Token(TokenType::END_OF_FILE, "", m_line));
  return m_tokens;
}

void Scanner::addToken(TokenType type) {
  string text = m_source.substr(m_start, m_current - m_start);
  m_tokens.push_back({.type = type, .lexeme = text, .line = m_line});
}

bool Scanner::isAtEnd() const { return m_current >= m_source.size(); }

char Scanner::peek(const int offset) const {
  if (m_current + offset >= m_source.size())
    return '\0';
  return m_source[m_current + offset];
}

char Scanner::advance() {
  // only call advance if not at end
  return m_source[m_current++];
}

void Scanner::scanToken() {
  char c = advance();
  switch (c) {
  case '(':
    addToken(TokenType::LEFT_PAREN);
    break;
  case ')':
    addToken(TokenType::RIGHT_PAREN);
    break;
  case '{':
    addToken(TokenType::LEFT_BRACE);
    break;
  case '}':
    addToken(TokenType::RIGHT_BRACE);
    break;
  case ',':
    addToken(TokenType::COMMA);
    break;
  case '.':
    addToken(TokenType::DOT);
    break;
  case '-':
    addToken(TokenType::MINUS);
    break;
  case '+':
    addToken(TokenType::PLUS);
    break;
  case ';':
    addToken(TokenType::SEMICOLON);
    break;
  case '*':
    addToken(TokenType::STAR);
    break;
  case '!':
    if (peek() == '=')
      advance(), addToken(TokenType::BANG_EQUAL);
    else
      addToken(TokenType::BANG);
    break;
  case '=':
    if (peek() == '=')
      advance(), addToken(TokenType::EQUAL_EQUAL);
    else
      addToken(TokenType::EQUAL);
    break;
  case '<':
    if (peek() == '=')
      advance(), addToken(TokenType::LESS_EQUAL);
    else
      addToken(TokenType::LESS);
    break;
  case '>':
    if (peek() == '=')
      advance(), addToken(TokenType::GREATER_EQUAL);
    else
      addToken(TokenType::GREATER);
    break;
  case '/':
    if (peek() == '/') {
      advance();
      while (peek() != '\n' && !isAtEnd())
        advance();
    } else if (peek() == '*') {
      advance();
      while (peek() != '*' && peek(1) != '/' && !isAtEnd()) {
        if (peek() == '\n')
          m_line++;
        advance();
      }
      if (isAtEnd())
        error(m_line, "Unterminated comment.");
      advance();
      advance();
    } else
      addToken(TokenType::SLASH);
    break;
  case ' ':
  case '\r':
  case '\t':
    break;
  case '\n':
    m_line++;
    break;
  case '"':
    handleString();
    break;
  default:
    if (std::isdigit(c))
      handleNumber();
    else if (std::isalpha(c) || c == '_')
      handleIdentifier();
    else
      error(m_line, fmt::format("Unexpected character \"{}\"", c));
    break;
  }
}

void Scanner::handleString() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n')
      m_line++;
    advance();
  }
  if (isAtEnd()) {
    error(m_line, "Unterminated string.");
    return;
  }
  advance(); // consume closing "
  // trim the surrounding quotes ??
  addToken(TokenType::STRING);
}

void Scanner::handleNumber() {
  while (std::isdigit(peek()))
    advance();
  if (peek() == '.' && std::isdigit(peek(1))) {
    advance(); // consume the .
    advance(); // consume the digit after the .
    while (std::isdigit(peek()))
      advance();
  }
  addToken(TokenType::NUMBER);
}

void Scanner::handleIdentifier() {
  while (std::isalnum(peek()) || peek() == '_')
    advance();
  string text = m_source.substr(m_start, m_current - m_start);
  if (keywords.find(text) != keywords.end())
    addToken(keywords.at(text));
  else
    addToken(TokenType::IDENTIFIER);
}
