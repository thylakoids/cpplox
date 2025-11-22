#include "LoxInstance.h"
#include <memory>

LoxInstance::LoxInstance(std::shared_ptr<LoxClass> klass) : m_klass(klass) {}
LiteralValue LoxInstance::get(const std::string &name) {
  auto it = m_fields.find(name);
  if (it != m_fields.end()) {
    return it->second;
  }

  auto methodIt = m_klass->m_methods.find(name);
  if (methodIt != m_klass->m_methods.end()) {
    return methodIt->second->bind(shared_from_this());
  }

  throw std::runtime_error("Undefined property '" + name + "'.");
}

void LoxInstance::set(const std::string &name, const LiteralValue &value) {
  m_fields[name] = value;
}

std::string LoxInstance::toString() const {
  return "<instance of " + m_klass->toString() + ">";
}
