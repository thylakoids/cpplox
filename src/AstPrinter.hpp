#pragma once
#include "expr.hpp"
#include <sstream>

class AstPrinter : public ExprVisitor<std::string> {
public:
    std::string print(const Expr& expr) {
        return expr.accept(*this);
    }

    std::string visitBinaryExpr(const BinaryExpr &expr) override {
        return parenthesize(expr.op.lexeme, {&expr.left, &expr.right});
    }

    std::string visitGroupingExpr(const GroupingExpr &expr) override {
        return parenthesize("group", {&expr.expr});
    }

    std::string visitLiteralExpr(const LiteralExpr &expr) override {
        return std::visit(LiteralPrinter{}, expr.value);
    }

    std::string visitUnaryExpr(const UnaryExpr &expr) override {
        return parenthesize(expr.op.lexeme, {&expr.right});
    }

private:
    // Helper struct to print different literal types
    struct LiteralPrinter {
        std::string operator()(const std::string& s) const { return s; }
        std::string operator()(bool b) const { return b ? "true" : "false"; }
        std::string operator()(int d) const { return std::to_string(d); }
        std::string operator()(double d) const {
          std::string s = std::to_string(d);
          // trim trailing zeros
          s.erase(s.find_last_not_of('0') + 1);
          if (s.back() == '.') {
              s.push_back('0');
          }
          return s;
        }
        std::string operator()(std::nullptr_t) const { return "nil"; }
    };

    std::string parenthesize(const std::string& name, const std::initializer_list<const Expr*>& exprs) {
        std::stringstream builder;

        builder << "(" << name;
        for (const Expr* expr : exprs) {
            builder << " ";
            builder << expr->accept(*this);
        }
        builder << ")";

        return builder.str();
    }
};
