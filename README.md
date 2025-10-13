# cpplox

A C++20 implementation of the Lox interpreter from *Crafting Interpreters*.
This repository tracks the project up through Chapter 11 (resolving and
binding) with a few extra conveniences such as native helpers and environment
visualisation.

## Features Implemented
- Scanner supporting strings, numbers, block comments, and the loop control keywords.
- Recursive-descent parser that owns the AST nodes it allocates.
- Static resolver that validates scope usage, detects unused locals, and records
  lexical depth for fast lookups.
- Tree-walk interpreter with closures, return/break/continue control flow, and a
  pair of native functions (`clock`, `__printEnv`).

## Getting Started

### Prerequisites
- CMake 3.10+
- A C++20-compatible compiler (clang++ 12+, MSVC 2019+, or g++ 10+)
- `fmt` library installed on your system (`brew install fmt` on macOS,
  `sudo apt install libfmt-dev` on Ubuntu, etc.)

### Configure and Build
```bash
cmake -S . -B build
cmake --build build
```

Both the interpreter (`cpplox`) and a small AST printer demo (`test_expr`) will
be emitted in the `build/` directory.

### Running Lox Code
- Launch the REPL:
  ```bash
  ./build/cpplox
  ```
- Execute a script:
  ```bash
  ./build/cpplox path/to/script.lox
  ```
  Sample programs live under `build/` (`test2.lox`, `test3.lox`, …) after
  you copy or author them.

Inside the REPL, type `.exit` to quit. Use the `__printEnv()` native helper to
inspect the current environment chain while debugging.

### Printing the AST (optional)
Builds also include a small driver to exercise the AST printer:
```bash
./build/test_expr
```

## Project Layout
- `src/` – scanner, parser, resolver, interpreter, and runtime support.
- `CMakeLists.txt` – build script generating both executables.
- `expression.md`, `statement.md` – grammar notes generated while working
  through the book.

## Next Steps
Upcoming chapters introduce classes, inheritance, and the bytecode VM. When you
resume the book, run a quick smoke test (closures, loop control) after each
feature and consider adding scripted `.lox` examples for regression coverage.
