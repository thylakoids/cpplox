#include "expr.hpp"
#include "AstPrinter.hpp"
#include <memory>
#include <iostream>

int main() {
    // Test all types of literals
    auto numLiteral = std::make_unique<LiteralExpr>(123.1);
    auto strLiteral = std::make_unique<LiteralExpr>("hello");
    auto trueLiteral = std::make_unique<LiteralExpr>(true);
    auto intLiteral = std::make_unique<LiteralExpr>(456);
    auto nilLiteral = std::make_unique<LiteralExpr>();  // nil

    // Create expression: (* (- 123.45) (group true))
    auto unary = std::make_unique<UnaryExpr>("-", *numLiteral);
    auto group = std::make_unique<GroupingExpr>(*trueLiteral);
    auto binary = std::make_unique<BinaryExpr>(*unary, "*", *group);

    // Print it
    AstPrinter printer;
    std::cout << "Expression: " << printer.print(*binary) << std::endl;

    // Print individual literals
    std::cout << "double: " << printer.print(*numLiteral) << std::endl;
    std::cout << "String: " << printer.print(*strLiteral) << std::endl;
    std::cout << "True: " << printer.print(*trueLiteral) << std::endl;
    std::cout << "int: " << printer.print(*intLiteral) << std::endl;
    std::cout << "Nil: " << printer.print(*nilLiteral) << std::endl;

    return 0;
}
