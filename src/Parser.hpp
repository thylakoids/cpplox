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
      : m_tokens(tokens), m_loop_depth(0) {};

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
      statements.push_back(declaration());
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

  Stmt* declaration() {
    try {
      if (match({TokenType::FUN})) return function("function");
      if (match({TokenType::VAR})) return varDeclaration();
      return statement();
    } catch (const ParseError& error) {
      synchronize();
      return nullptr;
    }
  }

  Stmt* function(const std::string& kind) {
    Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
    consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
    std::vector<Token> parameters;
    if (!check(TokenType::RIGHT_PAREN)) {
      do {
        if (parameters.size() >= 255) {
          error(peek(), "Cannot have more than 255 parameters.");
        }
        parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
      } while (match({TokenType::COMMA}));
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
    consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
    std::vector<Stmt*> body = dynamic_cast<BlockStmt*>(block())->statements;
    // RIGHT_BRACE is consumed by block()
    return allocate<FunctionStmt>(name, parameters, body);
  }

  Stmt* varDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

    Expr* initializer = nullptr;
    if (match({TokenType::EQUAL})) {
      initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
    return allocate<VarStmt>(name, initializer);
  }

  Stmt* statement() {
    if (match({TokenType::FOR})) return forStatement();
    if (match({TokenType::IF})) return ifStatement();
    if (match({TokenType::WHILE})) return whileStatement();
    if (match({TokenType::PRINT})) return printStatement();
    if (match({TokenType::LEFT_BRACE})) return block();
    if (match({TokenType::BREAK})) return breakStatement();
    if (match({TokenType::CONTINUE})) return continueStatement();
    return expressionStatement();
  }

  Stmt* breakStatement() {
    if (m_loop_depth == 0) {
      error(previous(), "Cannot use 'break' outside of a loop.");
    }
    Token keyword = previous();
    consume(TokenType::SEMICOLON, "Expect ';' after 'break'.");
    return allocate<BreakStmt>(keyword);
  }

  Stmt* continueStatement() {
    if (m_loop_depth == 0) {
      error(previous(), "Cannot use 'continue' outside of a loop.");
    }
    Token keyword = previous();
    consume(TokenType::SEMICOLON, "Expect ';' after 'continue'.");
    return allocate<ContinueStmt>(keyword, m_continue_increment);
  }

  /*
   * for (var i = 0; i < 10; i = i + 1){ print i; }
   * {
   *   var i = 0;
   *   while (i < 10) {
   *     print i;
   *     i = i + 1;
   *   }
   * }
   */
  Stmt *forStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");
    Stmt *initializer;
    if (match({TokenType::SEMICOLON})) {
      initializer = nullptr;
    } else if (match({TokenType::VAR})) {
      initializer = varDeclaration();
    } else {
      initializer = expressionStatement();
    }
    Expr *condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
      condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");
    Expr *increment = nullptr;
    if (!check(TokenType::RIGHT_PAREN)) {
      increment = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

    m_loop_depth++;
    // Store the increment expression to use in any continue statements
    m_continue_increment = increment;
    Stmt *body = statement();
    m_continue_increment = nullptr;
    m_loop_depth--;

    if (increment != nullptr) {
      body = allocate<BlockStmt>(std::vector<Stmt*>{body, allocate<ExpressionStmt>(*increment)});
    }

    if (condition == nullptr) {
      condition = allocate<LiteralExpr>(true);
    }

    body = allocate<WhileStmt>(*condition, *body);

    if (initializer != nullptr) {
      body = allocate<BlockStmt>(std::vector<Stmt*>{initializer, body});
    }
    return body;
  }

  Stmt* ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    Expr* condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

    Stmt* thenBranch = statement();
    Stmt* elseBranch = nullptr;
    if (match({TokenType::ELSE})) {
      elseBranch = statement();
    }

    return allocate<IfStmt>(*condition, *thenBranch, elseBranch);
  }

  Stmt* whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    Expr* condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
    
    m_loop_depth++;
    Stmt* body = statement();
    m_loop_depth--;
    
    return allocate<WhileStmt>(*condition, *body);
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

  Stmt* block() {
    std::vector<Stmt*> statements;
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
      statements.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return allocate<BlockStmt>(statements);
  }

  Expr *expression() {
    // expression -> assignment ;
    return assignment();
  };

  Expr *assignment() {
    // assignment -> IDENTIFIER "=" assignment | logic_or ;
    Expr *exprptr = logic_or();
    if (match({TokenType::EQUAL})) {
      Token equals = previous();
      Expr *value = assignment();
      if (VariableExpr* ve = dynamic_cast<VariableExpr *>(exprptr)) {
        Token name = ve->name;
        return allocate<AssignExpr>(name, *value);
      }
      // Don't throw it because the parser isn't in a confused state
      // where we need to go into panic mode and synchronize.
      error(equals, "Invalid assignment target.");
    }
    return exprptr;
  }

  Expr *logic_or() {
    // logic_or -> logic_and ( "or" logic_and )* ;
    Expr *exprptr = logic_and();
    while (match({TokenType::OR})) {
      Token op = previous();
      Expr *right = logic_and();
      exprptr = allocate<LogicalExpr>(*exprptr, op, *right);
    }
    return exprptr;
  }

  Expr *logic_and() {
    // logic_and      → equality ( "and" equality )* ;
    Expr *exprptr = equality();
    while(match({TokenType::AND})) {
      Token op = previous();
      Expr *right = equality();
      exprptr = allocate<LogicalExpr>(*exprptr, op, *right);
    }
    return exprptr;
  }

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
    return call();
  }

  Expr *call() {
    // call -> primary ( "(" arguments? ")" )* ;
    Expr *exprptr = primary();
    while (true) {
      if (match({TokenType::LEFT_PAREN})) {
        exprptr = finishCall(exprptr);
      } else {
        break;
      }
    }
    return exprptr;
  }

  Expr *finishCall(Expr *callee) {
    // arguments -> expression ( "," expression )* ;
    std::vector<Expr *> arguments;
    if (!check(TokenType::RIGHT_PAREN)) {
      do {
        if (arguments.size() >= 255) {
          error(peek(), "Cannot have more than 255 arguments.");
        }
        arguments.push_back(expression());
      } while (match({TokenType::COMMA}));
    }
    Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
    return allocate<CallExpr>(*callee, paren, arguments);
  }

  Expr *primary() {
    // primary -> NUMBER | STRING | "true" | "false" | "nil" | "(" expression
    // ")" | IDENTIFIER ;
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
      return allocate<LiteralExpr>(previous().lexeme.substr(1, previous().lexeme.size() - 2));
    if (match({TokenType::IDENTIFIER}))
      return allocate<VariableExpr>(previous());
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
  int m_loop_depth = 0;  // Track loop nesting level
  Expr* m_continue_increment = nullptr;  // Current for-loop increment for continue statements
};
