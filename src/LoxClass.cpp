#include "LoxClass.h"
#include "LoxInstance.h"
#include <string>

LiteralValue LoxClass::call(Interpreter &interpreter,
                            const std::vector<LiteralValue> &arguments) {
  std::shared_ptr<LoxInstance> instance =
      std::make_shared<LoxInstance>(shared_from_this());
  auto initializer = findMethod("init");
  if (initializer) {
    initializer->bind(instance)->call(interpreter, arguments);
  }
  return instance;
}

int LoxClass::arity() const {
  auto methos = findMethod("init");
  if (methos) {
    return methos->arity();
  }
  return 0;
}
std::string LoxClass::toString() const { return m_name; }

std::shared_ptr<LoxFunction> LoxClass::findMethod(const std::string &name) const {
  auto it = m_methods.find(name);
  if (it != m_methods.end()) {
    return it->second;
  }
  if (m_superclass) {
    return m_superclass->findMethod(name);
  }
  return nullptr;
}
