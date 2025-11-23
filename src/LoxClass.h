#ifndef LOXCLASS_H_
#define LOXCLASS_H_
#pragma once
#include "LoxCallable.h"
#include "LoxFunction.h"
#include <memory>
#include <string>
#include <unordered_map>

class LoxClass : public LoxCallable,
                 public std::enable_shared_from_this<LoxClass> {
  friend class LoxInstance;

public:
  LoxClass(std::string name) : m_name(name) {}
  LoxClass(
      std::string name, std::shared_ptr<LoxClass> superclass,
      std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods)
      : m_name(std::move(name)), m_superclass(superclass),
        m_methods(std::move(methods)) {}

  LiteralValue call(Interpreter &interpreter,
                    const std::vector<LiteralValue> &arguments) override;
  int arity() const override;
  std::string toString() const override;
  std::shared_ptr<LoxFunction> findMethod(const std::string &name) const;

private:
  std::string m_name;
  std::shared_ptr<LoxClass> m_superclass;
  std::unordered_map<std::string, std::shared_ptr<LoxFunction>> m_methods;
};

#endif // LOXCLASS_H_
