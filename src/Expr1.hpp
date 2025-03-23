#ifndef EXPR_H_
#define EXPR_H_
#pragma once

#include <string>
#include <variant>
#include <memory>
#include "Token.h"
#include <vector>

class LoxCallable;
// Forward declaration
class BinaryExpr;
class LogicalExpr;
class UnaryExpr;
class LiteralExpr;
class GroupingExpr;
class VariableExpr;
class AssignExpr;
class CallExpr;

// Define literal value type that can hold any kind of literal
// todo: int is redundant
using LiteralValue = std::variant<std::string, int, double, bool, std::nullptr_t, std::shared_ptr<LoxCallable>>;

// Visitor pattern
template <typename R>
class ExprVisitor {
public:
  virtual R visitBinaryExpr(const BinaryExpr &expr) = 0;
  virtual R visitLogicalExpr(const LogicalExpr &expr) = 0;
  virtual R visitUnaryExpr(const UnaryExpr &expr) = 0;
  virtual R visitLiteralExpr(const LiteralExpr &expr) = 0;
  virtual R visitGroupingExpr(const GroupingExpr &expr) = 0;
  virtual R visitVariableExpr(const VariableExpr &expr) = 0;
  virtual R visitAssignExpr(const AssignExpr &expr) = 0;
  virtual R visitCallExpr(const CallExpr &expr) = 0;
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

// Binary expression
class LogicalExpr : public Expr {
public:
    LogicalExpr(const Expr &left, const Token &op, const Expr &right)
        : left(left), op(op), right(right) {}

    std::string accept(ExprVisitor<std::string> &visitor) const override {
        return visitor.visitLogicalExpr(*this);
    }

    LiteralValue accept(ExprVisitor<LiteralValue> &visitor) const override {
        return visitor.visitLogicalExpr(*this);
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

// Variable expression
class VariableExpr : public Expr {
public:
    VariableExpr(const Token &name) : name(name) {}

    std::string accept(ExprVisitor<std::string> &visitor) const override {
        return visitor.visitVariableExpr(*this);
    }

    LiteralValue accept(ExprVisitor<LiteralValue> &visitor) const override {
        return visitor.visitVariableExpr(*this);
    }

    const Token name;
};

class AssignExpr : public Expr {
public:
    AssignExpr(const Token &name, const Expr &value) : name(name), value(value) {}

    std::string accept(ExprVisitor<std::string> &visitor) const override {
        return visitor.visitAssignExpr(*this);
    }

    LiteralValue accept(ExprVisitor<LiteralValue> &visitor) const override {
        return visitor.visitAssignExpr(*this);
    }

    const Token name;
    const Expr &value;
};

class CallExpr : public Expr {
public:
  CallExpr(const Expr &callee, const Token &paren, const std::vector<Expr *> &arguments)
      : callee(callee), paren(paren), arguments(std::move(arguments)) {}

  std::string accept(ExprVisitor<std::string> &visitor) const override {
    return visitor.visitCallExpr(*this);
  }

  LiteralValue accept(ExprVisitor<LiteralValue> &visitor) const override {
    return visitor.visitCallExpr(*this);
  }

  const Expr &callee;
  const Token paren; // Use token location for error reporting
  const std::vector<Expr *> arguments;
};


#endif // EXPR_H_
