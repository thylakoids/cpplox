#include "AstPrinter.hpp"
#include "Expr.hpp"
#include "Token.h"
#include <iostream>
#include <memory>

int main() {
  AstPrinter printer;

  // Test all types of literals
  auto numLiteral = std::make_unique<LiteralExpr>(123.0);
  auto strLiteral = std::make_unique<LiteralExpr>("hello");
  auto trueLiteral = std::make_unique<LiteralExpr>(true);
  auto intLiteral = std::make_unique<LiteralExpr>(456);
  auto nilLiteral = std::make_unique<LiteralExpr>(); // nil

  // Create expression: (* (- 123.45) (group true))
  auto unary =
      std::make_unique<UnaryExpr>(Token(TokenType::MINUS, "-", 1), *numLiteral);
  auto group = std::make_unique<GroupingExpr>(*trueLiteral);
  auto binary = std::make_unique<BinaryExpr>(
      *unary, Token(TokenType::STAR, "*", 0), *group);

  std::cout << "Binary expression: " << printer.print(*binary) << std::endl;

  // Variable and assignment: (assign x 42)
  Token xToken(TokenType::IDENTIFIER, "x", 1);
  auto xVar = std::make_unique<VariableExpr>(xToken);
  auto fortyTwo = std::make_unique<LiteralExpr>(42);
  auto assign = std::make_unique<AssignExpr>(xToken, *fortyTwo);

  std::cout << "Variable: " << printer.print(*xVar) << std::endl;
  std::cout << "Assign:   " << printer.print(*assign) << std::endl;

  // Logical expression: (or true nil)
  auto logical = std::make_unique<LogicalExpr>(
      *trueLiteral, Token(TokenType::OR, "or", 1), *nilLiteral);
  std::cout << "Logical:  " << printer.print(*logical) << std::endl;

  // Call expression: (call foo 456 123)
  Token fooToken(TokenType::IDENTIFIER, "foo", 1);
  auto fooVar = std::make_unique<VariableExpr>(fooToken);
  std::vector<Expr *> callArgs;
  callArgs.push_back(intLiteral.get());
  callArgs.push_back(numLiteral.get());
  auto call = std::make_unique<CallExpr>(
      *fooVar, Token(TokenType::RIGHT_PAREN, ")", 1), callArgs);
  std::cout << "Call:     " << printer.print(*call) << std::endl;

  // Get expression: (get obj field)
  Token objToken(TokenType::IDENTIFIER, "obj", 1);
  auto objVar = std::make_unique<VariableExpr>(objToken);
  Token fieldToken(TokenType::IDENTIFIER, "field", 1);
  auto get = std::make_unique<GetExpr>(*objVar, fieldToken);
  std::cout << "Get:      " << printer.print(*get) << std::endl;

  // Mixed: method call on object -> (call (get method obj) 456 123)
  Token methodToken(TokenType::IDENTIFIER, "method", 1);
  auto getMethod = std::make_unique<GetExpr>(*objVar, methodToken);
  std::vector<Expr *> methodArgs;
  methodArgs.push_back(intLiteral.get());
  methodArgs.push_back(numLiteral.get());
  auto methodCall = std::make_unique<CallExpr>(
      *getMethod, Token(TokenType::RIGHT_PAREN, ")", 1), methodArgs);
  std::cout << "Method:   " << printer.print(*methodCall) << std::endl;

  // Mixed: call result then get field -> (get result (call foo))
  auto fooCallNoArgs = std::make_unique<CallExpr>(
      *fooVar, Token(TokenType::RIGHT_PAREN, ")", 1), std::vector<Expr *>{});
  Token resField(TokenType::IDENTIFIER, "result", 1);
  auto getFromCall = std::make_unique<GetExpr>(*fooCallNoArgs, resField);
  std::cout << "CallGet:  " << printer.print(*getFromCall) << std::endl;

  // Print individual literals
  std::cout << "double:   " << printer.print(*numLiteral) << std::endl;
  std::cout << "string:   " << printer.print(*strLiteral) << std::endl;
  std::cout << "true:     " << printer.print(*trueLiteral) << std::endl;
  std::cout << "int:      " << printer.print(*intLiteral) << std::endl;
  std::cout << "nil:      " << printer.print(*nilLiteral) << std::endl;

  return 0;
}
