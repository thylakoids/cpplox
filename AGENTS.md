# Repository Guidelines

## Project Structure & Module Organization
This interpreter follows the Crafting Interpreters layout. Core runtime, scanning, parsing, and resolution code lives in `src/`. `src/lox.cpp` launches the REPL and file runner, while `src/test_expr.cpp` drives the AST printer demo. Build artifacts land in `build/`. Reference grammars in `expression.md` and `statement.md` mirror the chapter progression; update them when syntax changes.

## Build, Test, and Development Commands
```bash
cmake -S . -B build                 # configure once per environment or when CMakeLists.txt changes
cmake --build build                 # compile cpplox and test_expr with the default generator
cmake --build build --target test_expr  # rebuild only the AST printer helper
./build/cpplox                      # start the REPL; pass a .lox path to execute a script
./build/test_expr                   # smoke-test expression printing after parser tweaks
```
Regenerate `compile_commands.json` by rerunning the configure step if you swap compilers or flags.

## Coding Style & Naming Conventions
Target C++20, include project headers with double quotes, and prefer `<...>` for standard headers. Use four-space indentation, braces on the same line, and `PascalCase` for types (`Interpreter`, `Environment`). Member functions are `camelCase`, while helpers that return objects often use verbs (`lookUpVariable`). Avoid raw ownership; prefer `std::shared_ptr` or `std::unique_ptr` as the existing code does. Run `clang-format -i src/*.cpp src/*.hpp --style=LLVM` before sending patches.

## Testing Guidelines
There is no unit-test harness yet, so lean on targeted `.lox` scripts. Add ad-hoc programs beside the build output (e.g., `build/loops.lox`) and run them with `./build/cpplox build/loops.lox`. Exercise regressions: closures, while loops with `break`/`continue`, and resolver warnings for unused variables. After AST or parser work, run `./build/test_expr` and inspect the output for expected structure. When adding automated tests, prefer Catch2 and wire them through CMake so `cmake --build build --target tests` becomes possible.

## Commit & Pull Request Guidelines
History favors Conventional Commits (`feat(resolver): detect unused locals`, `refactor: tidy environment stacks`). Use a short imperative summary, and scope modules in parentheses when it clarifies impact. Multi-step features can be prefixed with context tags like `[Cursor]` when mirroring the book chapters. Pull requests should describe motivation, list verification steps (commands run, scripts exercised), and link related Crafting Interpreters chapters or issues. Include screenshots of interpreter output only when behavior changes are visual.

## Environment & Debug Tips
Within the REPL, `.exit` quits cleanly. Call the native helper `__printEnv()` while paused at a breakpoint to inspect lexical scopes. Prefer tracing by temporarily enabling prints in `EnvironmentPrinter.cpp`, then revert before opening a PR. Document any new debug utilities in `README.md` so other contributors discover them quickly.
