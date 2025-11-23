#include "LoxInstance.h"
#include <memory>

LoxInstance::LoxInstance(std::shared_ptr<LoxClass> klass) : m_klass(klass) {}
LiteralValue LoxInstance::get(const std::string &name) {
  auto it = m_fields.find(name);
  if (it != m_fields.end()) {
    return it->second;
  }

  auto method = m_klass->findMethod(name);
  if (method) {
    return method->bind(shared_from_this());
  }

  throw std::runtime_error("Undefined property '" + name + "'.");
}

void LoxInstance::set(const std::string &name, const LiteralValue &value) {
  m_fields[name] = value;
}

std::string LoxInstance::toString() const {
  return "<instance of " + m_klass->toString() + ">";
}
