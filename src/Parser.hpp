#pragma once
#include "Token.h"
#include "error.h"
#include "Expr.hpp"
#include "Stmt.hpp"
#include <vector>


class ParseError : public std::exception {
public:
  [[nodiscard]] const char *what() const noexcept override {
    return "Parse error";
  }
};

class Parser {
public:
  [[nodiscard]] explicit Parser(const std::vector<Token> &tokens)
      : m_tokens(tokens){};

  // Destructor
  ~Parser() {
    // Clean up all allocated expressions and statements
    for (Expr* expr : m_allocated_exprs) {
      delete expr;
    }
    for (Stmt* stmt : m_allocated_stmts) {
      delete stmt;
    }
  }

  // Prevent copying and moving
  Parser(const Parser&) = delete;
  Parser& operator=(const Parser&) = delete;
  Parser(Parser&&) = delete;
  Parser& operator=(Parser&&) = delete;

  std::vector<Stmt*> parse() {
    std::vector<Stmt*> statements;
    while (!isAtEnd()) {
      try {
        statements.push_back(statement());
      } catch (const ParseError &error) {
        synchronize();
      }
    }
    return statements;
  }

private:
  // Helper function to track allocated expressions
  template<typename T, typename... Args>
  T* allocate(Args&&... args) {
    if constexpr (std::is_base_of_v<Expr, T>) {
      T* expr = new T(std::forward<Args>(args)...);
      m_allocated_exprs.push_back(expr);
      return expr;
    } else if constexpr (std::is_base_of_v<Stmt, T>) {
      T* stmt = new T(std::forward<Args>(args)...);
      m_allocated_stmts.push_back(stmt);
      return stmt;
    }
  }

  /* Stmt* declaration() { */
  /*   try { */
  /*     if (match({TokenType::VAR})) return varDeclaration(); */
  /*     return statement(); */
  /*   } catch (const ParseError& error) { */
  /*     synchronize(); */
  /*     return nullptr; */
  /*   } */
  /* } */

  /* Stmt* varDeclaration() { */
  /*   Token name = consume(TokenType::IDENTIFIER, "Expect variable name."); */

  /*   Expr* initializer = nullptr; */
  /*   if (match({TokenType::EQUAL})) { */
  /*     initializer = expression(); */
  /*   } */

  /*   consume(TokenType::SEMICOLON, "Expect ';' after variable declaration."); */
  /*   return allocate<VarStmt>(name, initializer); */
  /* } */

  Stmt* statement() {
    if (match({TokenType::PRINT})) return printStatement();
    return expressionStatement();
  }

  Stmt* printStatement() {
    Expr* value = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after value.");
    return allocate<PrintStmt>(*value);
  }

  Stmt* expressionStatement() {
    Expr* expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return allocate<ExpressionStmt>(*expr);
  }

  Expr *expression() {
    // expression -> equality
    return equality();
  };

  Expr *equality() {
    // equality -> comparison ( ( "!=" | "==" ) comparison )* ;
    Expr *exprptr = comparison();
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
      Token op = previous();
      Expr *right = comparison();
      exprptr = allocate<BinaryExpr>(*exprptr, op, *right);
    }
    return exprptr;
  }

  Expr *comparison() {
    // comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
    Expr *exprptr = term();
    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS,
                  TokenType::LESS_EQUAL})) {
      Token op = previous();
      Expr *right = term();
      exprptr = allocate<BinaryExpr>(*exprptr, op, *right);
    }
    return exprptr;
  }

  Expr *term() {
    // term -> factor ( ( "-" | "+" ) factor )* ;
    Expr *exprptr = factor();
    while (match({TokenType::MINUS, TokenType::PLUS})) {
      Token op = previous();
      Expr *right = factor();
      exprptr = allocate<BinaryExpr>(*exprptr, op, *right);
    }
    return exprptr;
  }

  Expr *factor() {
    // factor -> unary ( ( "/" | "*" ) unary )* ;
    Expr *exprptr = unary();
    while (match({TokenType::SLASH, TokenType::STAR})) {
      Token op = previous();
      Expr *right = unary();
      exprptr = allocate<BinaryExpr>(*exprptr, op, *right);
    }
    return exprptr;
  }

  Expr *unary() {
    // unary -> ( "!" | "-" ) unary | primary ;
    if (match({TokenType::BANG, TokenType::MINUS})) {
      Token op = previous();
      Expr *right = unary();
      return allocate<UnaryExpr>(op, *right);
    }
    return primary();
  }

  Expr *primary() {
    // primary -> NUMBER | STRING | "true" | "false" | "nil" | "(" expression
    // ")" ;
    if (match({TokenType::FALSE}))
      return allocate<LiteralExpr>(false);
    if (match({TokenType::TRUE}))
      return allocate<LiteralExpr>(true);
    if (match({TokenType::NIL}))
      return allocate<LiteralExpr>();
    if (match({TokenType::NUMBER})) {
      if (previous().lexeme.find('.') != std::string::npos)
        return allocate<LiteralExpr>(std::stod(previous().lexeme));
      return allocate<LiteralExpr>(std::stoi(previous().lexeme));

    }
    if (match({TokenType::STRING}))
      return allocate<LiteralExpr>(previous().lexeme);
    if (match({TokenType::LEFT_PAREN})) {
      Expr *exprptr = expression();
      consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
      return allocate<GroupingExpr>(*exprptr);
    }
    throw error(peek(), "Expect expression.");
  }

  ParseError error(const Token &token, const std::string &message) {
    lox::error(token, message);
    return ParseError();
  }

  void synchronize() {
    advance();
    while (!isAtEnd()) {
      if (previous().type == TokenType::SEMICOLON)
        return;
      switch (peek().type) {
      case TokenType::CLASS:
      case TokenType::FUN:
      case TokenType::VAR:
      case TokenType::FOR:
      case TokenType::IF:
      case TokenType::WHILE:
      case TokenType::PRINT:
      case TokenType::RETURN:
        return;
      default:
        break;
      }
      advance();
    }
  }

  bool match(const std::vector<TokenType> &types) {
    for (const TokenType &type : types) {
      if (check(type)) {
        advance();
        return true;
      }
    }
    return false;
  }

  Token consume(TokenType type, const std::string &message) {
    if (!check(type))
      throw error(peek(), message);
    return advance();
  }

  bool check(TokenType type) {
    if (isAtEnd())
      return false;
    return peek().type == type;
  }

  Token advance() {
    if (!isAtEnd()) {
      m_current++;
    }
    return previous();
  }

  Token peek() const { return m_tokens[m_current]; }

  Token previous() const { return m_tokens[m_current - 1]; }

  bool isAtEnd() const { return peek().type == TokenType::END_OF_FILE; }

private:
  std::vector<Token> m_tokens;
  std::vector<Expr*> m_allocated_exprs;  // Track allocated expressions
  std::vector<Stmt*> m_allocated_stmts;  // Track allocated statements
  int m_current = 0;
};
