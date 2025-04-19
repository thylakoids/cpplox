#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_
#pragma once

#include <string>
#include <unordered_map>
#include "Expr.hpp"
#include "error.h"
#include "EnvironmentPrinter.h"

class Environment {
    // Make EnvironmentPrinter a friend class so it can access m_values and enclosing
    friend void formatEnvironmentRecursive(std::stringstream& ss, const Environment* env, size_t depth);

public:
    Environment() = default;
    explicit Environment(Environment* enclosing) : enclosing(enclosing) {}

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

    LiteralValue getAt(int distance, const Token& name) {
        Environment* environment = this;
        for (int i = 0; i < distance; i++) {
            environment = environment->enclosing; 
        }
        if (environment->m_values.find(name.lexeme) != environment->m_values.end()) {
            return environment->m_values.at(name.lexeme);
        }
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

    void assignAt(int distance, const Token& name, const LiteralValue& value) {
        Environment* environment = this;
        for (int i = 0; i < distance; i++) {
            environment = environment->enclosing;
        }
        // Assuming the resolver guarantees the variable exists at this distance.
        environment->m_values[name.lexeme] = value;
    }

    // Returns a string representation of the environment chain by calling the helper
    std::string toString() const {
        return formatEnvironment(*this);
    }

public:
    // We don't own the environment, it's not our responsibility to delete it
    Environment* enclosing = nullptr;

private:
    std::unordered_map<std::string, LiteralValue> m_values;
};

#endif // ENVIRONMENT_H_
