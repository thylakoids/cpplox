#include "LoxInstance.h"
#include <memory>

LoxInstance::LoxInstance(std::shared_ptr<LoxClass> klass) : m_klass(klass) {}
LiteralValue LoxInstance::get(const std::string &name) {
  auto it = m_fields.find(name);
  if (it != m_fields.end()) {
    return it->second;
  }

  throw std::runtime_error("Undefined property '" + name + "'.");
}
std::string LoxInstance::toString() const {
  return "<instance of " + m_klass->toString() + ">";
}
