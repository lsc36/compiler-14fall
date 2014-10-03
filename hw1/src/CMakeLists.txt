cmake_minimum_required(VERSION 2.8)

project(AcDc)
add_executable(AcDc AcDc.c builder.c checker.c codegen.c parser.c scanner.c)
