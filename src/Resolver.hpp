#ifndef RESOLVER_H_
#define RESOLVER_H_
#pragma once

#include <vector>
#include "Expr.hpp"
#include "Interpreter.h"
#include "Stmt.hpp"
#include "dataStruct.hpp"


class Resolver : public ExprVisitor<void>, public StmtVisitor<void> {
private:
    enum class FunctionType {
        NONE,
        FUNCTION,
    };
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
            if (it != scopes.top().end() && it->second == false) {
                 lox::error(expr.name, "Can't read local variable in its own initializer.");
            }
        }
        resolveLocal(expr, expr.name);
    }

    void visitAssignExpr(const AssignExpr& expr) override{
        resolve(&expr.value);
        resolveLocal(expr, expr.name);
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
        resolve(&stmt.body);
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
        // Do nothing
    }

    void visitBreakStmt(const BreakStmt& stmt) override{
        // Do nothing
    }

private:

    void beginScope(){
        scopes.push(std::unordered_map<std::string, bool>());
    }

    void endScope(){
        scopes.pop();
    }

    void declare(const Token& name){
        if (scopes.empty()) return;

        // Check if the name already exists in the current scope to prevent re-declaration errors later
        if (scopes.top().count(name.lexeme)) {
            lox::error(name, "Already a variable with this name in this scope.");
        }

        scopes.top().emplace(name.lexeme, false);
    }

    void define(const Token& name){
        if (scopes.empty()) return;
        scopes.top()[name.lexeme] = true;
    }

    void resolveLocal(const Expr& expr, const Token& name) {
        for (int i = scopes.size() -1; i>=0; i--){
            if (scopes.get(i).contains(name.lexeme)){
                m_interpreter.resolve(expr, scopes.size() -1 - i);
            }
        }
    }

    void resolveFunction(const FunctionStmt& function, FunctionType type){
        FunctionType enclosingFunction = currentFunction;
        currentFunction = type;
        beginScope();
        for (Token param: function.params){
            declare(param);
            define(param);
        }
        resolve(function.body);
        endScope();
    }

private:
    Interpreter &m_interpreter;
    IndexableStack<std::unordered_map<std::string, bool>> scopes {};
    FunctionType currentFunction = FunctionType::NONE;
};

#endif // RESOLVER_H_
