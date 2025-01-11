#pragma once
#include "expr.hpp"
#include <sstream>

class AstPrinter : public ExprVisitor<std::string> {
public:
    std::string print(const Expr& expr) {
        return expr.accept(*this);
    }

    std::string visitBinaryExpr(const BinaryExpr &expr) override {
        return parenthesize(expr.op, {&expr.left, &expr.right});
    }

    std::string visitGroupingExpr(const GroupingExpr &expr) override {
        return parenthesize("group", {&expr.expr});
    }

    std::string visitLiteralExpr(const LiteralExpr &expr) override {
        return expr.value;
    }

    std::string visitUnaryExpr(const UnaryExpr &expr) override {
        return parenthesize(expr.op, {&expr.right});
    }

private:
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
