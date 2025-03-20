cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJNAME pegcalc)

set(PEGTL_DIR /opt/taocpp/PEGTL/include)

project(${PROJNAME} LANGUAGES C CXX)

add_compile_options(
        -std=c++20
        -Wall -Wextra -Wshadow -pedantic -pedantic-errors 
)

add_executable(${PROJNAME})
target_include_directories(${PROJNAME} PUBLIC ${PEGTL_DIR})

target_sources(${PROJNAME}
    PUBLIC 
    calculator.cxx
)

target_sources(${PROJNAME}
    PUBLIC 
    FILE_SET repl TYPE CXX_MODULES FILES
    ReplEcho.cxx
    CalcGrammar.cxx
)
