#ifndef STMT_H_
#define STMT_H_
#pragma once

#include <string>
#include <vector>
#include "Token.h"
#include "Expr.hpp"

// Forward declarations of all statement types we'll need
class ExpressionStmt;
class IfStmt;
class PrintStmt;
class VarStmt;
class WhileStmt;
class BlockStmt;
class BreakStmt;
class ContinueStmt;

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
    virtual R visitIfStmt(const IfStmt &stmt) = 0;
    virtual R visitPrintStmt(const PrintStmt &stmt) = 0;
    virtual R visitVarStmt(const VarStmt &stmt) = 0;
    virtual R visitWhileStmt(const WhileStmt &stmt) = 0;
    virtual R visitBlockStmt(const BlockStmt &stmt) = 0;
    virtual R visitBreakStmt(const BreakStmt &stmt) = 0;
    virtual R visitContinueStmt(const ContinueStmt &stmt) = 0;
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

class WhileStmt : public Stmt {
public:
    WhileStmt(const Expr &condition, const Stmt &body) : condition(condition), body(body) {}

    void accept(StmtVisitor<void> &visitor) const override {
        visitor.visitWhileStmt(*this);
    }

    const Expr &condition;
    const Stmt &body;
};

/**
 * A block statement.
 * A block is a sequence of statements enclosed in braces.
 * It creates a new scope for variables.
 */
class BlockStmt : public Stmt {
public:
    BlockStmt(std::vector<Stmt*> statements) : statements(std::move(statements)) {}

    void accept(StmtVisitor<void> &visitor) const override {
        visitor.visitBlockStmt(*this);
    }

    const std::vector<Stmt*> statements;
};

class IfStmt : public Stmt {
public:
    IfStmt(const Expr &condition, const Stmt &thenBranch, const Stmt *elseBranch)
        : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}

    void accept(StmtVisitor<void> &visitor) const override {
        visitor.visitIfStmt(*this);
    }

    const Expr &condition;
    const Stmt &thenBranch;
    const Stmt *elseBranch; // nullptr if no else branch
};

/**
 * A break statement.
 * Used to exit from the innermost loop.
 */
class BreakStmt : public Stmt {
public:
    BreakStmt(const Token& keyword) : keyword(keyword) {}

    void accept(StmtVisitor<void> &visitor) const override {
        visitor.visitBreakStmt(*this);
    }

    const Token keyword;  // The 'break' token, for error reporting
};

/**
 * A continue statement.
 * Used to skip to the next iteration of the innermost loop.
 */
class ContinueStmt : public Stmt {
public:
    ContinueStmt(const Token& keyword) : keyword(keyword) {}

    void accept(StmtVisitor<void> &visitor) const override {
        visitor.visitContinueStmt(*this);
    }

    const Token keyword;  // The 'continue' token, for error reporting
};

#endif // STMT_H_

