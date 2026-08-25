cmake_minimum_required(VERSION 3.20)

project(MiniGuard LANGUAGES CXX)

add_executable(miniguard
    src/main.cpp
)

target_compile_features(miniguard PRIVATE cxx_std_20)

target_compile_options(miniguard PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)