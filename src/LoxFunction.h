#ifndef LOXFUNCTION_H_
#define LOXFUNCTION_H_
#pragma once

#include "LoxCallable.hpp"
#include "Stmt.hpp"

// Forward declaration
class Interpreter;

class LoxFunction : public LoxCallable {
public:
    explicit LoxFunction(const FunctionStmt* declaration);
    LiteralValue call(Interpreter& interpreter, const std::vector<LiteralValue>& arguments) const override;
    int arity() const override;
    std::string toString() const override;

private:
    const FunctionStmt* m_declaration;
};

#endif // LOXFUNCTION_H_
