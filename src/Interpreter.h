#ifndef INTERPRETER_H_
#define INTERPRETER_H_
#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include "Expr.hpp"
#include "Stmt.hpp"
#include "Environment.hpp"

// Custom exception for handling break statements
class BreakException : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override {
        return "Break statement encountered";
    }
};

class ReturnException : public std::exception {
public:
    explicit ReturnException(LiteralValue value) : m_value(std::move(value)) {}

    [[nodiscard]] const char* what() const noexcept override {
        return "Return statement encountered";
    }

    [[nodiscard]] LiteralValue getValue() const {
        return m_value;
    }
private:
    LiteralValue m_value;
};

// Custom exception for handling continue statements
class ContinueException : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override {
        return "Continue statement encountered";
    }
};

class Interpreter : public ExprVisitor<LiteralValue>, public StmtVisitor<void> {
friend class Resolver;
public:
    Interpreter();
    Environment* getEnvironment() const;
    void interpret(const std::vector<Stmt*>& statements);

    // ExprVisitor method implementations
    LiteralValue visitLiteralExpr(const LiteralExpr &expr) override;
    LiteralValue visitGroupingExpr(const GroupingExpr &expr) override;
    LiteralValue visitUnaryExpr(const UnaryExpr &expr) override;
    LiteralValue visitVariableExpr(const VariableExpr &expr) override;
    LiteralValue visitBinaryExpr(const BinaryExpr &expr) override;
    LiteralValue visitCallExpr(const CallExpr &expr) override;
    LiteralValue visitGetExpr(const GetExpr &expr) override;
    LiteralValue visitSetExpr(const SetExpr &expr) override;
    LiteralValue visitThisExpr(const ThisExpr &expr) override;
    LiteralValue visitSuperExpr(const SuperExpr &expr) override;
    LiteralValue visitLogicalExpr(const LogicalExpr &expr) override;
    LiteralValue visitAssignExpr(const AssignExpr &expr) override;

    // StmtVisitor method implementations
    void visitExpressionStmt(const ExpressionStmt &stmt) override;
    void visitClassStmt(const ClassStmt &stmt) override;
    void visitFunctionStmt(const FunctionStmt &stmt) override;
    void visitIfStmt(const IfStmt &stmt) override;
    void visitPrintStmt(const PrintStmt &stmt) override;
    void visitVarStmt(const VarStmt &stmt) override;
    void visitBlockStmt(const BlockStmt &stmt) override;
    void visitWhileStmt(const WhileStmt &stmt) override;
    void visitBreakStmt(const BreakStmt &stmt) override;
    void visitContinueStmt(const ContinueStmt &stmt) override;
    void visitReturnStmt(const ReturnStmt &stmt) override;

    // Public block execution method (needed by LoxFunction)
    void executeBlock(const std::vector<Stmt*>& statements, std::shared_ptr<Environment> env);

private:
    std::shared_ptr<Environment> m_globals; // Global scope environment
    std::shared_ptr<Environment> m_envptr;  // Current environment pointer
    std::unordered_map<const Expr*, int> m_locals;

    LiteralValue evaluate(const Expr& expr);
    LiteralValue lookUpVariable(const Token&, const Expr&);
    void execute(const Stmt& stmt);
    void resolve(const Expr& expr, int depth);
    bool isTruthy(const LiteralValue& value);
    bool isEqual(const LiteralValue& a, const LiteralValue& b);
    bool isNumber(const LiteralValue& value);
    double getNumberValue(const LiteralValue& value);
    void checkNumberOperand(const Token& op, const LiteralValue& operand);
    void checkNumberOperand(const Token& op, const LiteralValue& left, const LiteralValue& right);
};

#endif // INTERPRETER_H_ 
