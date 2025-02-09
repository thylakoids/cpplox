#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_
#pragma once

#include <any>
#include <string>
#include <unordered_map>
#include "Expr.hpp"
#include "error.h"


class Environment {
public:
    Environment() = default;

    void define(const std::string& name, const LiteralValue& value) {
        m_values[name] = value;
    }

    LiteralValue get(const Token& name) {
        if (m_values.find(name.lexeme) != m_values.end()) {
            return m_values.at(name.lexeme);
        }

        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }

    void assign(const Token& name, const LiteralValue& value) {
        if (m_values.find(name.lexeme) != m_values.end()) {
            m_values[name.lexeme] = value;
            return;
        }

        throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }

private:
    std::unordered_map<std::string, LiteralValue> m_values;
};

#endif // ENVIRONMENT_H_

