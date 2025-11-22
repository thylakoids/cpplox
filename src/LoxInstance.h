#ifndef LOXINSTANCE_H_
#define LOXINSTANCE_H_
#pragma once
#include <string>
#include <memory>
#include "LoxClass.h"
#include <unordered_map>

class LoxInstance {
public:
  LoxInstance(std::shared_ptr<LoxClass> klass);
  std::string toString() const;
  LiteralValue get(const std::string &name);

private:
  std::shared_ptr<LoxClass> m_klass;
  std::unordered_map<std::string, LiteralValue> m_fields;
};

#endif // LOXINSTANCE_H_
