#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Scanner.h"
#include "Parser.hpp"
#include "error.h"
#include "Interpreter.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

void runFile(const string &);
void runPrompt();
void run(const string &);

int main(int argc, char *argv[]) {
  if (argc > 2) {
    std::cout << "Usage: lox [script]" << std::endl;
    return 64;
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    runPrompt();
  }
  return 0;
}

void runFile(const string &path) {
  cout << "processing file: " << path << endl;
  std::ifstream ifile(path);
  std::stringstream ss;
  if (ifile.is_open()) {
    ss << ifile.rdbuf();
    run(ss.str());
    ifile.close();

    // Indicate an error in the exit code
    if (lox::hadError) exit(65);
    if (lox::hadRuntimeError) exit(70);
  } else {
    cout << "Failed to open file: " << path << endl;
    exit(74);
  }
}

void runPrompt() {
  cout << "Welcome to Lox!" << endl;
  string line;
  while (true) {
    cout << "> ";
    if (!std::getline(std::cin, line)) break;
    if (line == ".exit") break;
    run(line);
    // Reset error flag in REPL mode
    lox::resetError();
  }
}

void run(const string &source) {
    Scanner scanner(source);
    vector<Token> tokens = scanner.scanTokens();

    Parser parser(tokens);
    std::vector<Stmt*> statements = parser.parse();
    // Stop if there was a syntax error
    if (lox::hadError) return;
    Interpreter interpreter;
    interpreter.interpret(statements);
}
