#ifndef LOXINSTANCE_H_
#define LOXINSTANCE_H_
#pragma once
#include <string>
#include <memory>
#include "LoxClass.h"

class LoxInstance {
public:
  LoxInstance(std::shared_ptr<LoxClass> klass);
  std::string toString() const;

private:
  std::shared_ptr<LoxClass> m_klass;
};

#endif // LOXINSTANCE_H_
