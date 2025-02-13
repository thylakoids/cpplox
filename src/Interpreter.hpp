#ifndef INTERPRETER_H_
#define INTERPRETER_H_
#pragma once
#include <iostream>
#include "AstPrinter.hpp"
#include "Expr.hpp"
#include "Stmt.hpp"
#include "error.h"
#include "Environment.hpp"


class Interpreter : public ExprVisitor<LiteralValue>, public StmtVisitor<void> {
public:
    void interpret(const std::vector<Stmt*>& statements) {
        try {
            for (const Stmt* stmt : statements) {
                execute(*stmt);
            }
        } catch (const RuntimeError& error) {
            lox::error(error.m_token, error.what(), true);
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

        if (expr.op.lexeme == "-") {
            checkNumberOperand(expr.op, right);
            if (std::holds_alternative<double>(right)) {
                return -std::get<double>(right);
            }
            return -std::get<int>(right);
        } else if (expr.op.lexeme == "!") {
            return !isTruthy(right);
        }

        // Unreachable
        throw RuntimeError(expr.op, "Invalid unary operator");
    }

    LiteralValue visitVariableExpr(const VariableExpr &expr) override {
        return m_environment.get(expr.name);
    }

    LiteralValue visitBinaryExpr(const BinaryExpr &expr) override {
        LiteralValue left = evaluate(expr.left);
        LiteralValue right = evaluate(expr.right);

        if (expr.op.lexeme == "+") {
            if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right)) {
                return std::get<std::string>(left) + std::get<std::string>(right);
            }
            if (isNumber(left) && isNumber(right)) {
                return getNumberValue(left) + getNumberValue(right);
            }
            throw RuntimeError(expr.op, "Operands must be two numbers or two strings.");
        }
        else if (expr.op.lexeme == "-") {
            checkNumberOperand(expr.op, left, right);
            return getNumberValue(left) - getNumberValue(right);
        }
        else if (expr.op.lexeme == "*") {
            checkNumberOperand(expr.op, left, right);
            return getNumberValue(left) * getNumberValue(right);
        }
        else if (expr.op.lexeme == "/") {
            checkNumberOperand(expr.op, left, right);
            double rightNum = getNumberValue(right);
            if (rightNum == 0) {
                throw RuntimeError(expr.op, "Division by zero.");
            }
            return getNumberValue(left) / rightNum;
        }
        else if (expr.op.lexeme == ">") {
            checkNumberOperand(expr.op, left, right);
            return getNumberValue(left) > getNumberValue(right);
        }
        else if (expr.op.lexeme == ">=") {
            checkNumberOperand(expr.op, left, right);
            return getNumberValue(left) >= getNumberValue(right);
        }
        else if (expr.op.lexeme == "<") {
            checkNumberOperand(expr.op, left, right);
            return getNumberValue(left) < getNumberValue(right);
        }
        else if (expr.op.lexeme == "<=") {
            checkNumberOperand(expr.op, left, right);
            return getNumberValue(left) <= getNumberValue(right);
        }
        else if (expr.op.lexeme == "==") {
            return isEqual(left, right);
        }
        else if (expr.op.lexeme == "!=") {
            return !isEqual(left, right);
        }

        // Unreachable
        throw RuntimeError(expr.op, "Invalid binary operator");
    }

    void visitExpressionStmt(const ExpressionStmt &stmt) override {
        evaluate(stmt.expression);
    }

    void visitPrintStmt(const PrintStmt &stmt) override {
        LiteralValue value = evaluate(stmt.expression);
        AstPrinter printer;
        std::cout << printer.print(LiteralExpr(value)) << std::endl;
    }

    void visitVarStmt(const VarStmt &stmt) override {
        LiteralValue value = nullptr; // Set to nil if it isn't explicitly
                                      // initialized
        if (stmt.initializer) {
            value = evaluate(*stmt.initializer);
        }
        m_environment.define(stmt.name.lexeme, value);
    }

    LiteralValue visitAssignExpr(const AssignExpr &expr) override {
        LiteralValue value = evaluate(expr.value);
        m_environment.assign(expr.name, value);
        return value;
    }

    void visitBlockStmt(const BlockStmt &stmt) override {
        executeBlock(stmt.statements);
    }

    void executeBlock(const std::vector<Stmt*>& statements) {
        // error prone
        Environment previous = m_environment;
        Environment environment = Environment(&previous);
        m_environment = environment;

        try {
            for (const Stmt* statement : statements) {
                execute(*statement);
            }
        } catch (...) {
            m_environment = previous;
            throw;
        }

        m_environment = previous;
    }


private:
    Environment m_environment;

private:
    LiteralValue evaluate(const Expr& expr) {
        return expr.accept(*this);
    }

    void execute(const Stmt& stmt) {
        stmt.accept(*this);
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
        // Only call this function if isNumber(value) returns true
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        }
        if (std::holds_alternative<int>(value)) {
            return static_cast<double>(std::get<int>(value));
        }
        // Unreachable
        throw std::runtime_error("Value is not a number.");
    }

    void checkNumberOperand(const Token& op, const LiteralValue& operand) {
        if (!isNumber(operand)) {
            throw RuntimeError(op, "Operand must be a number.");
        }
    }

    void checkNumberOperand(const Token& op, const LiteralValue& left, const LiteralValue& right) {
        if (!isNumber(left) || !isNumber(right)) {
            throw RuntimeError(op, "Operands must be numbers.");
        }
    }
};

#endif // INTERPRETER_H_
