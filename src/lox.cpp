#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Scanner.h"

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
  } else {
    cout << "Open file" << path << endl;
  }
}

void runPrompt() {
  cout << "Welcome to JackAnalyzer!" << endl;
  string line;
  while (true) {
    cout << '>';
    getline(std::cin, line);
    if (line == ".exit")
      break;
    run(line);
  }
}

void run(const string &source) {
    Scanner scanner(source);
    vector<Token> tokens = scanner.scanTokens();
    for (const Token &token : tokens) {
      cout << token.toString() << endl;
    }
}
