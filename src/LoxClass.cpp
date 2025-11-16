#include "LoxClass.h"
#include "LoxInstance.h"
#include <string>

LoxClass::LoxClass(std::string name) : m_name(name) {}

LiteralValue LoxClass::call(Interpreter &interpreter,
                            const std::vector<LiteralValue> &arguments) {
  std::shared_ptr<LoxInstance> instance =
      std::make_shared<LoxInstance>(shared_from_this());
  return instance;
}
int LoxClass::arity() const { return 0; }
std::string LoxClass::toString() const { return m_name; }
