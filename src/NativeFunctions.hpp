#ifndef NATIVE_FUNCTIONS_H_
#define NATIVE_FUNCTIONS_H_
#pragma once

#include <iostream>
#include <chrono>
#include <memory>
#include "LoxCallable.h"
#include "Interpreter.h"

// ClockFunction: Native function that returns current time in seconds
class ClockFunction : public LoxCallable {
public:
    int arity() const override {
        return 0; // Takes no arguments
    }

    LiteralValue call(Interpreter& interpreter,
                     const std::vector<LiteralValue>& arguments) override {
        // Get current time since epoch in seconds
        auto now = std::chrono::system_clock::now();
        auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
        auto epoch = seconds.time_since_epoch();
        double time = static_cast<double>(epoch.count());

        return time;
    }

    std::string toString() const override {
        return "<native fn: clock>";
    }
};

class __printEnv : public LoxCallable {
public:
    int arity() const override {
        return 0; // Takes no arguments
    }

    LiteralValue call(Interpreter& interpreter,
                     const std::vector<LiteralValue>& arguments) override {
        std::cout << interpreter.getEnvironment()->toString() << std::endl;
        return nullptr;
    }

    std::string toString() const override {
        return "<native fn: __printEnv>";
    }
};

// Factory function to create all native functions
inline std::vector<std::pair<std::string, std::shared_ptr<LoxCallable>>> createNativeFunctions() {
    std::vector<std::pair<std::string, std::shared_ptr<LoxCallable>>> functions;

    // Add clock function
    functions.push_back({"clock", std::make_shared<ClockFunction>()});
    // Add printEnv function
    functions.push_back({"__printEnv", std::make_shared<__printEnv>()});

    return functions;
}

#endif // NATIVE_FUNCTIONS_H_
