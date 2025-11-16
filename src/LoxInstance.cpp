#include "LoxInstance.h"
#include <memory>

LoxInstance::LoxInstance(std::shared_ptr<LoxClass>klass) : m_klass(klass) {}
std::string LoxInstance::toString() const {
  return "<instance of " + m_klass->toString() + ">";
}
