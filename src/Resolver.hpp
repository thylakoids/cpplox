#ifndef RESOLVER_H_
#define RESOLVER_H_
#pragma once

#include <vector>
#include "Expr.hpp"
#include "Interpreter.h"
#include "Stmt.hpp"
#include "dataStruct.hpp"

/*
A block statement introduces a new scope for the statements it contains.

A function declaration introduces a new scope for its body and binds its parameters in that scope.

A variable declaration adds a new variable to the current scope.

Variable and assignment expressions need to have their variables resolved.
*/
class Resolver : public ExprVisitor<void>, public StmtVisitor<void> {
private:
    enum class FunctionType {
        NONE,
        FUNCTION,
    };

    enum class VariableState {
        DECLARED,
        DEFINED,
        USED
    };

    // Store the variable state and the token for error reporting
    using Scope = std::unordered_map<std::string, std::pair<VariableState, Token>>;
    IndexableStack<Scope> scopes {};

public:
    explicit Resolver(Interpreter &interpreter) : m_interpreter(interpreter) {}

    void resolve(const std::vector<Stmt* >& statements){
        for(Stmt* statement: statements) resolve(statement);
    }

    void resolve(const Stmt* const statement){
        statement->accept(*this);
    }

    void resolve(const Expr* const expr){
        expr->accept(*this);
    }

    void visitBlockStmt(const BlockStmt &stmt) override{
        beginScope();
        resolve(stmt.statements);
        endScope();
    }

    void visitVarStmt(const VarStmt &stmt) override{
        declare(stmt.name);
        if(stmt.initializer) resolve(stmt.initializer);
        define(stmt.name);
    }

    void visitVariableExpr (const VariableExpr &expr) override{
        if(!scopes.empty()){
            auto it = scopes.top().find(expr.name.lexeme);
            if (it != scopes.top().end() && it->second.first == VariableState::DECLARED) {
                 lox::error(expr.name, "Can't read local variable in its own initializer.");
            }
        }
        resolveLocal(expr, expr.name, true);
    }

    void visitAssignExpr(const AssignExpr& expr) override{
        resolve(&expr.value);
        resolveLocal(expr, expr.name, false);
    }

    void visitFunctionStmt(const FunctionStmt& stmt) override{
        declare(stmt.name);
        define(stmt.name);
        resolveFunction(stmt, FunctionType::FUNCTION);
    }

    void visitExpressionStmt(const ExpressionStmt& stmt) override{
        resolve(&stmt.expression);
    }

    void visitIfStmt(const IfStmt& stmt) override{
        resolve(&stmt.condition);
        resolve(&stmt.thenBranch);
        if (stmt.elseBranch) resolve(stmt.elseBranch);
    }

    void visitReturnStmt(const ReturnStmt& stmt) override{
        if(currentFunction == FunctionType::NONE){
            lox::error(stmt.keyword, "Can't return from top-level code.");
        }
        if(stmt.value) resolve(stmt.value);
    }

    void visitWhileStmt(const WhileStmt& stmt) override{
        resolve(&stmt.condition);
        m_loop_depth++;
        resolve(&stmt.body);
        m_loop_depth--;
        if (stmt.increment) {
            resolve(stmt.increment);
        }
    }

    void visitBinaryExpr(const BinaryExpr& expr) override{
        resolve(&expr.left);
        resolve(&expr.right);
    }

    void visitCallExpr(const CallExpr& expr) override{
        resolve(&expr.callee);
        for (Expr* arg: expr.arguments){
            resolve(arg);
        }
    }

    void visitGroupingExpr(const GroupingExpr& expr) override{
        resolve(&expr.expr);
    }

    void visitLiteralExpr(const LiteralExpr& expr) override{}

    void visitLogicalExpr(const LogicalExpr& expr) override{
        resolve(&expr.left);
        resolve(&expr.right);
    }

    void visitUnaryExpr(const UnaryExpr& expr) override{
        resolve(&expr.right);
    }

    void visitPrintStmt(const PrintStmt& stmt) override{
        resolve(&stmt.expression);
    }

    void visitContinueStmt(const ContinueStmt& stmt) override{
        if (m_loop_depth == 0) {
            lox::error(stmt.keyword, "Cannot use 'continue' outside of a loop.");
        }
    }

    void visitBreakStmt(const BreakStmt& stmt) override{
        if (m_loop_depth == 0) {
            lox::error(stmt.keyword, "Cannot use 'break' outside of a loop.");
        }
    }

private:

    void beginScope(){
        scopes.push(Scope());
    }

    void endScope(){
        Scope scope = scopes.top();
        scopes.pop();

        for(const auto& [name, state_token_pair] : scope){
            if (state_token_pair.first != VariableState::USED){
                lox::error(state_token_pair.second, "Local variable '" + name + "' is defined but never used.");
            }
        }
    }

    void declare(const Token& name){
        if (scopes.empty()) return;

        Scope& current_scope = scopes.top();
        if (current_scope.count(name.lexeme)) {
            lox::error(name, "Already a variable with this name in this scope.");
        }
        current_scope.emplace(name.lexeme, std::make_pair(VariableState::DECLARED, name));
    }

    void define(const Token& name){
        if (scopes.empty()) return;
        scopes.top()[name.lexeme].first = VariableState::DEFINED;
    }

    // Added 'isRead' parameter to distinguish variable access (read) from assignment target resolution
    void resolveLocal(const Expr& expr, const Token& name, bool isRead) {
        for (int i = scopes.size() - 1; i >= 0; i--) {
            Scope& scope = scopes.get(i); // Get mutable reference
            if (scope.contains(name.lexeme)) {
                m_interpreter.resolve(expr, scopes.size() - 1 - i);
                // Mark as used only if it's being read, not just assigned to
                if (isRead) {
                     scope[name.lexeme].first = VariableState::USED;
                }
                return;
            }
        }
        // If not found in local scopes, assume global (resolution error handled elsewhere if needed)
    }

    void resolveFunction(const FunctionStmt& function, FunctionType type){
        FunctionType enclosingFunction = currentFunction;
        currentFunction = type;
        beginScope();
        for (const Token& param: function.params){
            declare(param);
            define(param);
            // Parameters are implicitly used if the function is called,
            // but we can mark them USED immediately to avoid unused errors
            scopes.top()[param.lexeme].first = VariableState::USED;
        }
        resolve(function.body);
        endScope();
        currentFunction = enclosingFunction;
    }

private:
    Interpreter &m_interpreter;
    FunctionType currentFunction = FunctionType::NONE;
    int m_loop_depth = 0; // Track loop nesting level
};

#endif // RESOLVER_H_
