cmake_minimum_required(VERSION 2.8)

project(AcDc)
set(CMAKE_C_FLAGS "-Wall -std=c99")
add_executable(AcDc AcDc.c builder.c checker.c codegen.c parser.c scanner.c)