#ifndef INTERPRETER_H_
#define INTERPRETER_H_
#pragma once
#include <stdexcept>
#include "AstPrinter.hpp"
#include "expr.hpp"
#include "error.h"

class RuntimeError : public std::runtime_error {
public:
    RuntimeError(const std::string& message) : std::runtime_error(message) {}
};

class Interpreter : public ExprVisitor<LiteralValue> {
public:
    std::string interpret(const Expr& expr) {
        try {
            LiteralValue value = evaluate(expr);
            auto literal = LiteralExpr(value);
            AstPrinter printer;
            return printer.print(literal);
        } catch (const RuntimeError& error) {
            lox::runtimeError(error.what());
            lox::hadRuntimeError = true;
            return "";
        }
    }

    LiteralValue visitLiteralExpr(const LiteralExpr &expr) override {
        return expr.value;
    }

    LiteralValue visitGroupingExpr(const GroupingExpr &expr) override {
        return evaluate(expr.expr);
    }

    LiteralValue visitUnaryExpr(const UnaryExpr &expr) override {
        LiteralValue right = evaluate(expr.right);

        if (expr.op == "-") {
            checkNumberOperand(right);
            if (std::holds_alternative<double>(right)) {
                return -std::get<double>(right);
            }
            return -std::get<int>(right);
        } else if (expr.op == "!") {
            return !isTruthy(right);
        }

        // Unreachable
        throw RuntimeError("Invalid unary operator");
    }

    LiteralValue visitBinaryExpr(const BinaryExpr &expr) override {
        LiteralValue left = evaluate(expr.left);
        LiteralValue right = evaluate(expr.right);

        if (expr.op == "+") {
            if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
                return std::get<std::string>(left) + std::get<std::string>(right);
            }
            if (isNumber(left) && isNumber(right)) {
                return getNumberValue(left) + getNumberValue(right);
            }
            throw RuntimeError("Operands must be two numbers or two strings.");
        }
        else if (expr.op == "-") {
            checkNumberOperands(left, right);
            return getNumberValue(left) - getNumberValue(right);
        }
        else if (expr.op == "*") {
            checkNumberOperands(left, right);
            return getNumberValue(left) * getNumberValue(right);
        }
        else if (expr.op == "/") {
            checkNumberOperands(left, right);
            double rightNum = getNumberValue(right);
            if (rightNum == 0) {
                throw RuntimeError("Division by zero.");
            }
            return getNumberValue(left) / rightNum;
        }
        else if (expr.op == ">") {
            checkNumberOperands(left, right);
            return getNumberValue(left) > getNumberValue(right);
        }
        else if (expr.op == ">=") {
            checkNumberOperands(left, right);
            return getNumberValue(left) >= getNumberValue(right);
        }
        else if (expr.op == "<") {
            checkNumberOperands(left, right);
            return getNumberValue(left) < getNumberValue(right);
        }
        else if (expr.op == "<=") {
            checkNumberOperands(left, right);
            return getNumberValue(left) <= getNumberValue(right);
        }
        else if (expr.op == "==") {
            return isEqual(left, right);
        }
        else if (expr.op == "!=") {
            return !isEqual(left, right);
        }

        // Unreachable
        throw RuntimeError("Invalid binary operator");
    }
private:
    LiteralValue evaluate(const Expr& expr) {
        return expr.accept(*this);
    }

    bool isTruthy(const LiteralValue& value) {
        if (std::holds_alternative<std::nullptr_t>(value)) return false;
        if (std::holds_alternative<bool>(value)) return std::get<bool>(value);
        return true;
    }

    bool isEqual(const LiteralValue& a, const LiteralValue& b) {
        // Compare numbers regardless of their exact type (int or double)
        if (isNumber(a) && isNumber(b)) {
            return getNumberValue(a) == getNumberValue(b);
        }
        return a == b;
    }

    bool isNumber(const LiteralValue& value) {
        return std::holds_alternative<double>(value) || std::holds_alternative<int>(value);
    }

    double getNumberValue(const LiteralValue& value) {
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        }
        if (std::holds_alternative<int>(value)) {
            return static_cast<double>(std::get<int>(value));
        }
        throw RuntimeError("Operand must be a number.");
    }

    void checkNumberOperand(const LiteralValue& operand) {
        if (!isNumber(operand)) {
            throw RuntimeError("Operand must be a number.");
        }
    }

    void checkNumberOperands(const LiteralValue& left, const LiteralValue& right) {
        if (!isNumber(left) || !isNumber(right)) {
            throw RuntimeError("Operands must be numbers.");
        }
    }
};

#endif // INTERPRETER_H_
