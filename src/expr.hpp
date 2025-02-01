#pragma once
#include <string>
#include <variant>
#include "Token.h"

// Forward declaration
class BinaryExpr;
class UnaryExpr;
class LiteralExpr;
class GroupingExpr;

// Define literal value type that can hold any kind of literal
// todo: int is redundant
using LiteralValue = std::variant<std::string, int, double, bool, std::nullptr_t>;

// Visitor pattern
template <typename R>
class ExprVisitor {
public:
    virtual R visitBinaryExpr(const BinaryExpr &expr) = 0;
    virtual R visitUnaryExpr(const UnaryExpr &expr) = 0;
    virtual R visitLiteralExpr(const LiteralExpr &expr) = 0;
    virtual R visitGroupingExpr(const GroupingExpr &expr) = 0;
    virtual ~ExprVisitor() = default;
};

// Base Expr class
class Expr {
public:
    virtual ~Expr() = default;
    virtual std::string accept(ExprVisitor<std::string> &visitor) const = 0;
    virtual LiteralValue accept(ExprVisitor<LiteralValue> &visitor) const = 0;
};

// Binary expression
class BinaryExpr : public Expr {
public:
    BinaryExpr(const Expr &left, const Token &op, const Expr &right)
        : left(left), op(op), right(right) {}

    std::string accept(ExprVisitor<std::string> &visitor) const override {
        return visitor.visitBinaryExpr(*this);
    }

    LiteralValue accept(ExprVisitor<LiteralValue> &visitor) const override {
        return visitor.visitBinaryExpr(*this);
    }

    const Expr &left;
    const Token op;
    const Expr &right;
};

// Unary expression
class UnaryExpr : public Expr {
public:
    UnaryExpr(const Token &op, const Expr &right) 
        : op(op), right(right) {}

    std::string accept(ExprVisitor<std::string> &visitor) const override {
        return visitor.visitUnaryExpr(*this);
    }

    LiteralValue accept(ExprVisitor<LiteralValue> &visitor) const override {
        return visitor.visitUnaryExpr(*this);
    }

    const Token op;
    const Expr &right;
};

// Literal expression
class LiteralExpr : public Expr {
public:
    LiteralExpr(const LiteralValue &value) : value(value) {}
    LiteralExpr() : value(nullptr) {}  // for nil

    std::string accept(ExprVisitor<std::string> &visitor) const override {
        return visitor.visitLiteralExpr(*this);
    }

    LiteralValue accept(ExprVisitor<LiteralValue> &visitor) const override {
        return visitor.visitLiteralExpr(*this);
    }

    const LiteralValue value;
};

// Grouping expression
class GroupingExpr : public Expr {
public:
    GroupingExpr(const Expr &expr) : expr(expr) {}

    std::string accept(ExprVisitor<std::string> &visitor) const override {
        return visitor.visitGroupingExpr(*this);
    }

    LiteralValue accept(ExprVisitor<LiteralValue> &visitor) const override {
        return visitor.visitGroupingExpr(*this);
    }

    const Expr &expr;
};
