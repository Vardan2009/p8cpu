cmake_minimum_required(VERSION 3.15)

project(p8asm LANGUAGES C)

add_executable(${PROJECT_NAME}
  src/lexer.c
  src/lexer.h 

  src/main.c
)
