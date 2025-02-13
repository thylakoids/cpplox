#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_
#pragma once

#include <string>
#include <unordered_map>
#include "Expr.hpp"
#include "error.h"
 
class Environment;

class Environment {
public:
    Environment() = default;
    Environment(Environment* enclosing) : enclosing(enclosing) {}

    void define(const std::string& name, const LiteralValue& value) {
        m_values[name] = value;
    }

    LiteralValue get(const Token& name) {
        if (m_values.find(name.lexeme) != m_values.end()) {
            return m_values.at(name.lexeme);
        }
        if (enclosing != nullptr) return enclosing->get(name);

        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }

    void assign(const Token& name, const LiteralValue& value) {
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

public:
    Environment* enclosing = nullptr;

private:
    std::unordered_map<std::string, LiteralValue> m_values;
};

#endif // ENVIRONMENT_H_
