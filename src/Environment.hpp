#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_
#pragma once

#include "EnvironmentPrinter.h"
#include "Expr.hpp"
#include "error.h"
#include <memory>
#include <string>
#include <unordered_map>

class Environment : public std::enable_shared_from_this<Environment> {
  // Make EnvironmentPrinter a friend class so it can access m_values and
  // enclosing
  friend void formatEnvironmentRecursive(std::stringstream &ss,
                                         const Environment *env, size_t depth);

public:
  Environment() = default;
  explicit Environment(std::shared_ptr<Environment> enclosing)
      : enclosing(std::move(enclosing)) {}

  void define(const std::string &name, const LiteralValue &value) {
    m_values[name] = value;
  }

  LiteralValue get(const Token &name) {
    if (m_values.find(name.lexeme) != m_values.end()) {
      return m_values.at(name.lexeme);
    }
    if (enclosing != nullptr)
      return enclosing->get(name);

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
  }

  LiteralValue getAt(int distance, const Token &name) {
    std::shared_ptr<Environment> environment = shared_from_this();
    for (int i = 0; i < distance; i++) {
      environment = environment->enclosing;
    }
    auto it = environment->m_values.find(name.lexeme);
    if (it != environment->m_values.end()) {
      return it->second;
    }
    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
  }

  void assign(const Token &name, const LiteralValue &value) {
    if (m_values.find(name.lexeme) != m_values.end()) {
      m_values[name.lexeme] = value;
      return;
    }
    if (enclosing != nullptr) {
      enclosing->assign(name, value);
      return;
    }

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
  }

  void assignAt(int distance, const Token &name, const LiteralValue &value) {
    std::shared_ptr<Environment> environment = shared_from_this();
    for (int i = 0; i < distance; i++) {
      environment = environment->enclosing;
    }
    // Assuming the resolver guarantees the variable exists at this distance.
    environment->m_values[name.lexeme] = value;
  }

  // Returns a string representation of the environment chain by calling the
  // helper
  std::string toString() const { return formatEnvironment(*this); }

public:
  std::shared_ptr<Environment> enclosing = nullptr;

private:
  std::unordered_map<std::string, LiteralValue> m_values;
};

#endif // ENVIRONMENT_H_
