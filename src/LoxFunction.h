#ifndef LOXFUNCTION_H_
#define LOXFUNCTION_H_
#pragma once

#include <memory>
#include <fmt/core.h>
#include "LoxCallable.h"
#include "Stmt.hpp"
#include "Environment.hpp"

// Forward declaration
class Interpreter;

class LoxFunction : public LoxCallable {
public:
    explicit LoxFunction(const FunctionStmt* declaration, std::shared_ptr<Environment> closure);
    LiteralValue call(Interpreter& interpreter, const std::vector<LiteralValue>& arguments) override;
    std::shared_ptr<LoxFunction> bind(std::shared_ptr<class LoxInstance> instance);
    int arity() const override;
    std::string toString() const override;

private:
    const FunctionStmt* m_declaration;
    std::shared_ptr<Environment> m_closureptr;
};

#endif // LOXFUNCTION_H_
