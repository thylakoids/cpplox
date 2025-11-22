#include "LoxClass.h"
#include "LoxInstance.h"
#include <string>

LoxClass::LoxClass(std::string name) : m_name(name) {}
LoxClass::LoxClass(
    std::string name,
    std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods)
    : m_name(std::move(name)), m_methods(std::move(methods)) {}

LiteralValue LoxClass::call(Interpreter &interpreter,
                            const std::vector<LiteralValue> &arguments) {
  std::shared_ptr<LoxInstance> instance =
      std::make_shared<LoxInstance>(shared_from_this());
  auto it = m_methods.find("init");
  if (it != m_methods.end()) {
    auto initializer = it->second;
    initializer->bind(instance)->call(interpreter, arguments);
  }
  return instance;
}

int LoxClass::arity() const {
    auto it = m_methods.find("init");
    if (it != m_methods.end()) {
        return it->second->arity();
    }
    return 0;
}
std::string LoxClass::toString() const { return m_name; }
