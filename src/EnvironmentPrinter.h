#ifndef ENVIRONMENT_PRINTER_H_
#define ENVIRONMENT_PRINTER_H_
#pragma once

#include <string>

// Forward declaration to avoid cyclic dependency
class Environment;

/**
 * @brief Generates a formatted string representation of the environment chain.
 * 
 * @param env The Environment object to format.
 * @return A string containing the formatted environment table.
 */
std::string formatEnvironment(const Environment& env);

#endif // ENVIRONMENT_PRINTER_H_ 