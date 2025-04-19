#include <iostream>
#include "NativeFunctions.hpp"
#include "Interpreter.h"
#include "LoxFunction.h"
#include "AstPrinter.hpp"

Interpreter::Interpreter() {
    m_globals = std::make_shared<Environment>();
    m_envptr = m_globals;

    // Register native functions in the global environment
    for (const auto& [name, function] : createNativeFunctions()) {
        m_globals->define(name, function);
    }
}

Environment* Interpreter::getEnvironment() const {
    return m_envptr.get();
}

void Interpreter::interpret(const std::vector<Stmt*>& statements) {
    try {
        for (const Stmt* stmt : statements) {
            execute(*stmt);
        }
    } catch (const RuntimeError& error) {
        lox::error(error.m_token, error.what(), true);
    }
}

LiteralValue Interpreter::visitLiteralExpr(const LiteralExpr &expr) {
    return expr.value;
}

LiteralValue Interpreter::visitGroupingExpr(const GroupingExpr &expr) {
    return evaluate(expr.expr);
}

LiteralValue Interpreter::visitUnaryExpr(const UnaryExpr &expr) {
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

LiteralValue Interpreter::visitVariableExpr(const VariableExpr &expr) {
    /*return m_envptr->get(expr.name);*/
    return lookUpVariable(expr.name, expr);
}

LiteralValue Interpreter::lookUpVariable(const Token& name, const Expr& expr){
    // Check if the expression exists in the locals map
    auto it = m_locals.find(&expr);
    if (it != m_locals.end()) {
        int distance = it->second;
        return m_envptr->getAt(distance, name);
    } else {
        // If not found in locals, assume it's a global variable.
        // The Resolver should have caught undefined variables already.
        return m_globals->get(name);
    }
}

void Interpreter::visitWhileStmt(const WhileStmt &stmt) {
    try {
        while (isTruthy(evaluate(stmt.condition))) {
            try {
                execute(stmt.body);
            } catch (const ContinueException&) {
                if (stmt.increment) execute(*stmt.increment);
                continue;
            }
            if (stmt.increment) execute(*stmt.increment);
        }
    } catch (const BreakException&) {
        // Break encountered, exit the loop
    }
}

LiteralValue Interpreter::visitBinaryExpr(const BinaryExpr &expr) {
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

LiteralValue Interpreter::visitCallExpr(const CallExpr &expr) {
    LiteralValue callee = evaluate(expr.callee);

    std::vector<LiteralValue> arguments;
    for (const Expr* argument : expr.arguments) {
        arguments.push_back(evaluate(*argument));
    }

    if (!std::holds_alternative<std::shared_ptr<LoxCallable>>(callee)) {
        throw RuntimeError(expr.paren, "Can only call functions and classes.");
    }

    auto function = std::get<std::shared_ptr<LoxCallable>>(callee);
    if (arguments.size() != function->arity()) {
        throw RuntimeError(expr.paren, "Expected " + std::to_string(function->arity()) +
            " arguments but got " + std::to_string(arguments.size()) + ".");
    }

    return function->call(*this, arguments);
}

LiteralValue Interpreter::visitLogicalExpr(const LogicalExpr &expr) {
    LiteralValue left = evaluate(expr.left);
    if (expr.op.lexeme == "or") {
        if (isTruthy(left)) return left;
    } else {
        if (!isTruthy(left)) return left;
    }
    return evaluate(expr.right);
}

void Interpreter::visitExpressionStmt(const ExpressionStmt &stmt) {
    evaluate(stmt.expression);
}

void Interpreter::visitFunctionStmt(const FunctionStmt &stmt) {
    std::shared_ptr<LoxFunction> function = std::make_shared<LoxFunction>(&stmt, m_envptr);
    m_envptr->define(stmt.name.lexeme, function);
}

void Interpreter::visitIfStmt(const IfStmt &stmt) {
    if (isTruthy(evaluate(stmt.condition))) {
        execute(stmt.thenBranch);
    } else if (stmt.elseBranch) {
        execute(*stmt.elseBranch);
    }
}

void Interpreter::visitPrintStmt(const PrintStmt &stmt) {
    LiteralValue value = evaluate(stmt.expression);
    AstPrinter printer;
    std::cout << printer.print(LiteralExpr(value)) << std::endl;
}

void Interpreter::visitVarStmt(const VarStmt &stmt) {
    LiteralValue value = nullptr; // Set to nil if it isn't explicitly initialized
    if (stmt.initializer) {
        value = evaluate(*stmt.initializer);
    }
    m_envptr->define(stmt.name.lexeme, value);
}

LiteralValue Interpreter::visitAssignExpr(const AssignExpr &expr) {
    LiteralValue value = evaluate(expr.value);

    auto it = m_locals.find(&expr);
    if (it != m_locals.end()) {
        int distance = it->second;
        m_envptr->assignAt(distance, expr.name, value);
    } else {
        m_globals->assign(expr.name, value);
    }

    return value;
}

void Interpreter::visitBlockStmt(const BlockStmt &stmt) {
    auto env = std::make_shared<Environment>(m_envptr.get());
    executeBlock(stmt.statements, env);
}

void Interpreter::executeBlock(const std::vector<Stmt*>& statements, std::shared_ptr<Environment> env) {
    // error prone
    auto previous = m_envptr;
    m_envptr = env;

    try {
        for (const Stmt* statement : statements) {
            execute(*statement);
        }
    } catch (...) {
        m_envptr = previous;
        throw;
    }

    m_envptr = previous;
}

void Interpreter::visitBreakStmt(const BreakStmt &stmt) {
    throw BreakException();
}

void Interpreter::visitContinueStmt(const ContinueStmt &stmt) {
    throw ContinueException();
}

void Interpreter::visitReturnStmt(const ReturnStmt &stmt) {
    LiteralValue value = nullptr;
    if (stmt.value) {
        value = evaluate(*stmt.value);
    }
    throw ReturnException(value);
}

LiteralValue Interpreter::evaluate(const Expr& expr) {
    return expr.accept(*this);
}

void Interpreter::execute(const Stmt& stmt) {
    stmt.accept(*this);
}

void Interpreter::resolve(const Expr& expr, int depth){
    m_locals[&expr] = depth;
}

bool Interpreter::isTruthy(const LiteralValue& value) {
    if (std::holds_alternative<std::nullptr_t>(value)) return false;
    if (std::holds_alternative<bool>(value)) return std::get<bool>(value);
    if (std::holds_alternative<int>(value)) return std::get<int>(value) != 0;
    if (std::holds_alternative<double>(value)) return std::get<double>(value) != 0;
    return true;
}

bool Interpreter::isEqual(const LiteralValue& a, const LiteralValue& b) {
    // Compare numbers regardless of their exact type (int or double)
    if (isNumber(a) && isNumber(b)) {
        return getNumberValue(a) == getNumberValue(b);
    }
    return a == b;
}

bool Interpreter::isNumber(const LiteralValue& value) {
    return std::holds_alternative<double>(value) || std::holds_alternative<int>(value);
}

double Interpreter::getNumberValue(const LiteralValue& value) {
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

void Interpreter::checkNumberOperand(const Token& op, const LiteralValue& operand) {
    if (!isNumber(operand)) {
        throw RuntimeError(op, "Operand must be a number.");
    }
}

void Interpreter::checkNumberOperand(const Token& op, const LiteralValue& left, const LiteralValue& right) {
    if (!isNumber(left) || !isNumber(right)) {
        throw RuntimeError(op, "Operands must be numbers.");
    }
}
