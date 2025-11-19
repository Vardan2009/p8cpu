cmake_minimum_required(VERSION 3.15)

project(p8asm LANGUAGES C)

add_executable(${PROJECT_NAME}
	src/defs.h
	src/defs.c

	src/lexer.c
	src/lexer.h

	src/parser.h
	src/parser.c

	src/util.c
	src/util.h

	src/main.c
 "src/codegen.c" "src/codegen.h")