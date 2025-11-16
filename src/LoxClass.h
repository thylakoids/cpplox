#ifndef LOXCLASS_H_
#define LOXCLASS_H_
#pragma once
#include "LoxCallable.h"

class LoxClass : public LoxCallable, public std::enable_shared_from_this<LoxClass> {
public:
  LoxClass(std::string name);

  LiteralValue call(Interpreter &interpreter,
                    const std::vector<LiteralValue> &arguments) override;
  int arity() const override;
  std::string toString() const override;

private:
  std::string m_name;
};

#endif // LOXCLASS_H_
