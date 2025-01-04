#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using std::cout;
using std::endl;
using std::string;

void runFile(const string &);
void runPrompt();
string run(const string &);

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
    cout << line << endl;
    if (line == ".exit")
      break;
    cout << run(line) << endl;
  }
}

string run(const string &source) {
    return source;
}
