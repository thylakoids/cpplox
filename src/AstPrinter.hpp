#pragma once
#include "Expr.hpp"
#include <initializer_list>
#include <sstream>
#include <memory>
#include "LoxCallable.h"
#include "LoxInstance.h"

class AstPrinter : public ExprVisitor<std::string> {
public:
    std::string print(const Expr& expr) {
        return expr.accept(*this);
    }

    std::string visitBinaryExpr(const BinaryExpr &expr) override {
        return parenthesize(expr.op.lexeme, {&expr.left, &expr.right});
    }

    std::string visitLogicalExpr(const LogicalExpr &expr) override {
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

    std::string visitVariableExpr(const VariableExpr &expr) override {
        return expr.name.lexeme; // Just print the variable name
    }

    std::string visitAssignExpr(const AssignExpr &expr) override {
        return parenthesize("assign " + expr.name.lexeme, {&expr.value});
    }

    std::string visitCallExpr(const CallExpr &expr) override {
        return parenthesize("call " + expr.callee.accept(*this), expr.arguments);
    }

    std::string visitGetExpr(const GetExpr &expr) override {
        return parenthesize("get " + expr.name.lexeme, {&expr.object});
    }

private:
    // Helper struct to print different literal types
    struct LiteralPrinter {
        std::string operator()(const std::string& s) const { return "\""+s+"\""; }
        std::string operator()(bool b) const { return b ? "true" : "false"; }
        std::string operator()(int d) const { return std::to_string(d); }
        std::string operator()(double d) const {
          std::string s = std::to_string(d);
          // trim trailing zeros
          s.erase(s.find_last_not_of('0') + 1);
          if (s.back() == '.') {
              s.erase(s.size() - 1);
          }
          return s;
        }
        std::string operator()(std::nullptr_t) const { return "nil"; }
        std::string operator()(const std::shared_ptr<LoxCallable>& callable) const {
            return callable->toString();
        }
        std::string operator()(const std::shared_ptr<LoxInstance>& instance) const {
            return instance->toString();
        }
    };

    template<typename Container>
    std::string parenthesize(const std::string& name, const Container& exprs) {
        std::stringstream builder;

        builder << "(" << name;
        for (const Expr* expr : exprs) {
            builder << " ";
            builder << expr->accept(*this);
        }
        builder << ")";

        return builder.str();
    }

    // Overload for initializer_list
    std::string parenthesize(const std::string& name, const std::initializer_list<const Expr*>& exprs) {
        return parenthesize<std::initializer_list<const Expr*>>(name, exprs);
    }
};
