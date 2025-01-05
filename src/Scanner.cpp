#include "Scanner.h"
#include <fmt/core.h>
#include <iostream>
#include <map>

std::map<string, TokenType> keywords = {
    {"and", TokenType::AND},       {"class", TokenType::CLASS},
    {"else", TokenType::ELSE},     {"false", TokenType::FALSE},
    {"fun", TokenType::FUN},       {"for", TokenType::FOR},
    {"if", TokenType::IF},         {"nil", TokenType::NIL},
    {"or", TokenType::OR},         {"print", TokenType::PRINT},
    {"return", TokenType::RETURN}, {"super", TokenType::SUPER},
    {"this", TokenType::THIS},     {"true", TokenType::TRUE},
    {"var", TokenType::VAR},       {"while", TokenType::WHILE},
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
  m_tokens.push_back({.type=type, .lexeme=text, .line=m_line});
}

void Scanner::report(int line, const string &where,
                     const string &message) const {
  std::cerr << "[line " << line << "] ScannerError" << where << ": " << message
            << std::endl;
}

void Scanner::error(int line, const string &message) {
  report(line, "", message);
  m_hadError = true;
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
  case '0' ... '9':
    handleNumber();
    break;
  case 'a' ... 'z':
  case 'A' ... 'Z':
  case '_':
    handleIdentifier();
    break;
  default:
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

std::string tokenTypeToString(TokenType type) {
  switch (type) {
  case TokenType::LEFT_PAREN:
    return "LEFT_PAREN";
  case TokenType::RIGHT_PAREN:
    return "RIGHT_PAREN";
  case TokenType::LEFT_BRACE:
    return "LEFT_BRACE";
  case TokenType::RIGHT_BRACE:
    return "RIGHT_BRACE";
  case TokenType::COMMA:
    return "COMMA";
  case TokenType::DOT:
    return "DOT";
  case TokenType::MINUS:
    return "MINUS";
  case TokenType::PLUS:
    return "PLUS";
  case TokenType::SEMICOLON:
    return "SEMICOLON";
  case TokenType::SLASH:
    return "SLASH";
  case TokenType::STAR:
    return "STAR";
  case TokenType::BANG:
    return "BANG";
  case TokenType::BANG_EQUAL:
    return "BANG_EQUAL";
  case TokenType::EQUAL:
    return "EQUAL";
  case TokenType::EQUAL_EQUAL:
    return "EQUAL_EQUAL";
  case TokenType::GREATER:
    return "GREATER";
  case TokenType::GREATER_EQUAL:
    return "GREATER_EQUAL";
  case TokenType::LESS:
    return "LESS";
  case TokenType::LESS_EQUAL:
    return "LESS_EQUAL";
  case TokenType::IDENTIFIER:
    return "IDENTIFIER";
  case TokenType::STRING:
    return "STRING";
  case TokenType::NUMBER:
    return "NUMBER";
  case TokenType::AND:
    return "AND";
  case TokenType::CLASS:
    return "CLASS";
  case TokenType::ELSE:
    return "ELSE";
  case TokenType::FALSE:
    return "FALSE";
  case TokenType::FUN:
    return "FUN";
  case TokenType::FOR:
    return "FOR";
  case TokenType::IF:
    return "IF";
  case TokenType::NIL:
    return "NIL";
  case TokenType::OR:
    return "OR";
  case TokenType::PRINT:
    return "PRINT";
  case TokenType::RETURN:
    return "RETURN";
  case TokenType::SUPER:
    return "SUPER";
  case TokenType::THIS:
    return "THIS";
  case TokenType::TRUE:
    return "TRUE";
  case TokenType::VAR:
    return "VAR";
  case TokenType::WHILE:
    return "WHILE";
  case TokenType::END_OF_FILE:
    return "END_OF_FILE";
  }
}
