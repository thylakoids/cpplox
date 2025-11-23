#include "LoxFunction.h"
#include "Interpreter.h"
#include <memory>

LoxFunction::LoxFunction(const FunctionStmt *declaration,
                         std::shared_ptr<Environment> closure,
                         bool isInitializer)
    : m_declaration(declaration), m_closureptr(closure),
      m_isInitializer(isInitializer) {}

LiteralValue LoxFunction::call(Interpreter &interpreter,
                               const std::vector<LiteralValue> &arguments) {

  auto envptr = std::make_shared<Environment>(m_closureptr.get());

  // Bind arguments to parameters
  for (size_t i = 0; i < m_declaration->params.size(); i++) {
    envptr->define(m_declaration->params[i].lexeme, arguments[i]);
  }

  /*std::cout << "\ncalling " << this->toString() << "\n" << envptr->toString()
   * << std::endl;*/

  try {
    interpreter.executeBlock(m_declaration->body, envptr);
  } catch (const ReturnException &e) {
    if (m_isInitializer) {
      return m_closureptr->getAt(
          0, Token(TokenType::THIS, "this", m_declaration->name.line));
    }
    return e.getValue();
  }

  if (m_isInitializer) {
    return m_closureptr->getAt(
        0, Token(TokenType::THIS, "this", m_declaration->name.line));
  }

  // Return nil if no return statement was executed
  return nullptr;
}

std::shared_ptr<LoxFunction>
LoxFunction::bind(std::shared_ptr<LoxInstance> instance) {
  auto envptr = std::make_shared<Environment>(m_closureptr.get());
  envptr->define("this", instance);
  return std::make_shared<LoxFunction>(m_declaration, envptr, m_isInitializer);
}

int LoxFunction::arity() const { return m_declaration->params.size(); }

std::string LoxFunction::toString() const {
  return "<fn " + m_declaration->name.lexeme + ">";
}
