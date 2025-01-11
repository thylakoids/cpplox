#include "AstPrinter.hpp"
#include "expr.hpp"
#include <iostream>
#include <memory>

int main() {
  std::unique_ptr<LiteralExpr> literal1 = std::make_unique<LiteralExpr>("123");

  // Create expression: (* (- 123) (group 45.67))
  /* auto literal1 = std::make_unique<LiteralExpr>("123"); */
  auto unary = std::make_unique<UnaryExpr>("-", *literal1);

  auto literal2 = std::make_unique<LiteralExpr>("45.67");
  auto group = std::make_unique<GroupingExpr>(*literal2);

  auto binary = std::make_unique<BinaryExpr>(*unary, "*", *group);

  // Print it
  AstPrinter printer;
  std::cout << printer.print(*binary) << std::endl;

  return 0;
}
