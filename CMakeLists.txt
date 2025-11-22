cmake_minimum_required(VERSION 3.10)

project(cpplox)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(CREATE_LINK
  "${CMAKE_BINARY_DIR}/compile_commands.json"
  "${CMAKE_SOURCE_DIR}/compile_commands.json"
  SYMBOLIC
)

add_executable(${PROJECT_NAME}
    src/lox.cpp
    src/Scanner.cpp
    src/Token.cpp
    src/error.cpp
    src/LoxFunction.cpp
    src/LoxClass.cpp
    src/LoxInstance.cpp
    src/Interpreter.cpp
    src/EnvironmentPrinter.cpp
)

add_executable(test_expr
    src/test_expr.cpp
    src/LoxClass.cpp
    src/LoxInstance.cpp
    src/Interpreter.cpp
    src/EnvironmentPrinter.cpp
    src/LoxFunction.cpp
    src/error.cpp
)

find_package(fmt)
target_link_libraries(${PROJECT_NAME} fmt::fmt)
target_link_libraries(test_expr fmt::fmt)
