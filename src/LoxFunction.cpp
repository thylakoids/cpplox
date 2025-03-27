#include "LoxFunction.h"
#include "Interpreter.hpp"

LoxFunction::LoxFunction(const FunctionStmt* declaration) : m_declaration(declaration) {}

LiteralValue LoxFunction::call(Interpreter& interpreter, const std::vector<LiteralValue>& arguments) const {
    // Create a new environment for the function
    auto env = Environment(interpreter.getEnvironment());

    // Bind arguments to parameters
    for (size_t i = 0; i < m_declaration->params.size(); i++) {
        env.define(m_declaration->params[i].lexeme, arguments[i]);
    }

    interpreter.executeBlock(m_declaration->body, &env);
    // Return nil if no return statement was executed
    return nullptr;
}

int LoxFunction::arity() const {
    return m_declaration->params.size();
}

std::string LoxFunction::toString() const {
    return "<fn " + m_declaration->name.lexeme + ">";
}
