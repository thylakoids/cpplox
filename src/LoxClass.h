#ifndef LOXCLASS_H_
#define LOXCLASS_H_
#pragma once
#include "LoxCallable.h"
#include "LoxFunction.h"
#include <string>
#include <unordered_map>

class LoxClass : public LoxCallable, public std::enable_shared_from_this<LoxClass> {
  friend class LoxInstance;
public:
  LoxClass(std::string name);
  LoxClass(std::string name,
           std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods);

  LiteralValue call(Interpreter &interpreter,
                    const std::vector<LiteralValue> &arguments) override;
  int arity() const override;
  std::string toString() const override;

private:
  std::string m_name;
  std::unordered_map<std::string, std::shared_ptr<LoxFunction>> m_methods;
};

#endif // LOXCLASS_H_
