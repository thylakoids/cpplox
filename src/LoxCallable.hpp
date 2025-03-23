#ifndef LOX_CALLABLE_H_
#define LOX_CALLABLE_H_
#pragma once

#include <vector>
#include <string>
#include "Expr.hpp"

// Forward declarations to avoid circular dependency
class Interpreter;

class LoxCallable {
public:
    virtual ~LoxCallable() = default;

    // Returns the number of arguments this function expects
    virtual int arity() const = 0;

    // Executes the function with the given arguments
    virtual LiteralValue call(Interpreter& interpreter,
                             const std::vector<LiteralValue>& arguments) const = 0;

    // String representation of the callable
    virtual std::string toString() const = 0;
};

#endif // LOX_CALLABLE_H_
