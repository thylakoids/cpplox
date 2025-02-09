#ifndef STMT_H_
#define STMT_H_
#pragma once

#include <string>
#include "Token.h"
#include "Expr.hpp"

// Forward declarations of all statement types we'll need
class ExpressionStmt;
class PrintStmt;
class VarStmt;

/**
 * The visitor pattern for statements. Unlike expressions which can return
 * different types (string for printing, LiteralValue for interpreting),
 * statements are executed solely for their side effects, so they don't
 * return anything.
 */
template <typename R>
class StmtVisitor {
public:
    virtual R visitExpressionStmt(const ExpressionStmt &stmt) = 0;
    virtual R visitPrintStmt(const PrintStmt &stmt) = 0;
    virtual R visitVarStmt(const VarStmt &stmt) = 0;
    virtual ~StmtVisitor() = default;
};

/**
 * Base class for all statement types.
 * Statements don't produce values like expressions do.
 * They are executed solely for their side effects -- printing things,
 * creating variables, or modifying state.
 */
class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void accept(StmtVisitor<void> &visitor) const = 0;
};

/**
 * A statement that wraps a single expression.
 * These are expressions that are used for their side effects rather than their value.
 * For example: "print 'hi';" or "getLine();"
 */
class ExpressionStmt : public Stmt {
public:
    ExpressionStmt(const Expr &expression) : expression(expression) {}

    void accept(StmtVisitor<void> &visitor) const override {
        visitor.visitExpressionStmt(*this);
    }

    const Expr &expression;
};

/**
 * A statement that prints the value of its expression.
 * While most languages implement print as a regular function call,
 * Lox has it as a built-in statement to avoid dealing with native functions
 * before we're ready.
 */
class PrintStmt : public Stmt {
public:
    PrintStmt(const Expr &expression) : expression(expression) {}

    void accept(StmtVisitor<void> &visitor) const override {
        visitor.visitPrintStmt(*this);
    }

    const Expr &expression;
};

/**
 * A variable declaration statement.
 * Creates a new variable with an optional initializer.
 * Examples:
 *   var a;        // Declares with implicit nil initializer
 *   var a = 123;  // Declares with explicit initializer
 */
class VarStmt : public Stmt {
public:
    VarStmt(const Token &name, const Expr* initializer) 
        : name(name), initializer(initializer) {}

    void accept(StmtVisitor<void> &visitor) const override {
        visitor.visitVarStmt(*this);
    }

    const Token name;        // The name of the variable being declared
    const Expr* initializer; // The initializer expression, or nullptr if not initialized
};

#endif // STMT_H_

