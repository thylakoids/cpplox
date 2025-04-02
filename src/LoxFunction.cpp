#include "LoxFunction.h"
#include "Interpreter.h"

LoxFunction::LoxFunction(const FunctionStmt *declaration, std::shared_ptr<Environment>closure)
    : m_declaration(declaration), m_closureptr(closure) {}

LiteralValue LoxFunction::call(Interpreter &interpreter,
                               const std::vector<LiteralValue> &arguments) {

    auto envptr = std::make_shared<Environment>(m_closureptr.get());

    // Bind arguments to parameters
    for (size_t i = 0; i < m_declaration->params.size(); i++) {
        envptr->define(m_declaration->params[i].lexeme, arguments[i]);
    }

    /*std::cout << "\ncalling " << this->toString() << "\n" << envptr->toString() << std::endl;*/

    try {
        interpreter.executeBlock(m_declaration->body, envptr);
    } catch (const ReturnException &e) {
        return e.getValue();
    }

    // Return nil if no return statement was executed
    return nullptr;
}

int LoxFunction::arity() const { return m_declaration->params.size(); }

std::string LoxFunction::toString() const {
    return "<fn " + m_declaration->name.lexeme + ">";
}
