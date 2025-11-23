#include "EnvironmentPrinter.h"
#include "Environment.hpp" // Need full definition here
#include "LoxCallable.h"   // For LiteralValue and LoxCallable
#include "LoxInstance.h"
#include <algorithm> // For std::max
#include <cmath>     // For std::isinf, std::isnan
#include <sstream>
#include <string>

// Helper struct to print LiteralValue variants
struct LiteralPrinter {
  std::string operator()(const std::string &s) const { return "\"" + s + "\""; }
  std::string operator()(bool b) const { return b ? "true" : "false"; }
  std::string operator()(int d) const { return std::to_string(d); }
  std::string operator()(double d) const {
    std::string s = std::to_string(d);
    s.erase(s.find_last_not_of('0') + 1);
    if (!s.empty() && s.back() == '.') {
      s.pop_back();
    }
    if (s == "-")
      s = "-0";
    if (s.empty() && std::isinf(d))
      return d > 0 ? "inf" : "-inf";
    if (s.empty() && std::isnan(d))
      return "nan";
    if (s.empty())
      return "0";
    return s;
  }
  std::string operator()(std::nullptr_t) const { return "nil"; }
  std::string operator()(const std::shared_ptr<LoxCallable> &callable) const {
    return callable ? callable->toString() : "nil";
  }
  std::string operator()(const std::shared_ptr<LoxInstance> &instance) const {
    return instance ? instance->toString() : "nil";
  }
};

// Recursive helper function to build the string representation
void formatEnvironmentRecursive(std::stringstream &ss, const Environment *env,
                                size_t depth) {
  if (!env)
    return;

  // Define exact field widths matching the image
  const int nameFieldWidth = 26; // Adjusted from 25
  const int valueFieldWidth = 35;
  const int innerWidth =
      nameFieldWidth + 1 + valueFieldWidth;  // 26 + 1 + 35 = 62
  const int totalWidth = 1 + innerWidth + 1; // 1 + 62 + 1 = 64

  // Top border:
  // +--------------------------+-----------------------------------+
  ss << "+" << std::string(nameFieldWidth, '-') << "+"
     << std::string(valueFieldWidth, '-') << "+\n";

  // Scope header: |            SCOPE 0 (0xaddr)           |
  std::stringstream scope_ss;
  // Use std::hex to format the pointer address
  scope_ss << " SCOPE " << depth << " (0x" << std::hex
           << reinterpret_cast<uintptr_t>(env) << ") ";
  std::string scopeText = scope_ss.str();
  int scopePaddingTotal = innerWidth - scopeText.length();
  scopePaddingTotal = std::max(0, scopePaddingTotal);
  int scopePaddingLeft = scopePaddingTotal / 2;
  int scopePaddingRight = scopePaddingTotal - scopePaddingLeft;
  ss << "|" << std::string(scopePaddingLeft, ' ') << scopeText
     << std::string(scopePaddingRight, ' ') << "|\n";

  // Header separator:
  // +--------------------------+-----------------------------------+
  ss << "+" << std::string(nameFieldWidth, '-') << "+"
     << std::string(valueFieldWidth, '-') << "+\n";

  if (env->m_values.empty()) {
    // Empty message: |     [No variables defined in this scope]      |
    std::string emptyMsg = " [No variables defined in this scope] ";
    int emptyPaddingTotal = innerWidth - emptyMsg.length();
    emptyPaddingTotal = std::max(0, emptyPaddingTotal);
    int emptyPaddingLeft = emptyPaddingTotal / 2;
    int emptyPaddingRight = emptyPaddingTotal - emptyPaddingLeft;
    ss << "|" << std::string(emptyPaddingLeft, ' ') << emptyMsg
       << std::string(emptyPaddingRight, ' ') << "|\n";
  } else {
    for (const auto &[name, value] : env->m_values) {
      // Data row: | name                     | value |

      std::string nameStr = name;
      if (nameStr.length() > nameFieldWidth - 2) {
        nameStr = nameStr.substr(0, nameFieldWidth - 5) + "...";
      }

      std::string valueStr = std::visit(LiteralPrinter{}, value);
      if (valueStr.length() > valueFieldWidth - 2) {
        valueStr = valueStr.substr(0, valueFieldWidth - 5) + "...";
      }

      ss << "| " << std::left << nameStr;
      int namePadding = nameFieldWidth - (1 + nameStr.length());
      namePadding = std::max(0, namePadding);
      ss << std::string(namePadding, ' ');

      ss << "| " << std::left << valueStr;
      int valuePadding = valueFieldWidth - (1 + valueStr.length());
      valuePadding = std::max(0, valuePadding);
      ss << std::string(valuePadding, ' ') << "|\n";
    }
  }

  // Bottom border:
  // +--------------------------+-----------------------------------+
  ss << "+" << std::string(nameFieldWidth, '-') << "+"
     << std::string(valueFieldWidth, '-') << "+\n";

  // Recursively print enclosing environment
  if (env->enclosing != nullptr) {
    ss << std::string(totalWidth / 2, ' ') << "↓\n\n";
    formatEnvironmentRecursive(ss, env->enclosing, depth + 1);
  }
}

std::string formatEnvironment(const Environment &env) {
  std::stringstream ss;
  formatEnvironmentRecursive(ss, &env, 0);
  return ss.str();
}
