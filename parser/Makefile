CC		= gcc
CFLAGS	= -std=gnu11 -Wall -Wextra

BUILD	= build
INCLUDE	= -Iinclude/
OBJ_DIR = $(BUILD)/objects
SRC		= src
AST		= src/ast/decl.c src/ast/expr.c src/ast/param_list.c \
		  src/ast/stmt.c src/ast/type.c

BISONFLAGS = --header=include/yy.h

lex.yy.c: $(SRC)/scanner.flex
	flex -o $(SRC)/lex.yy.c $(SRC)/scanner.flex

parser.yy.c: $(SRC)/parser.y
	bison $(BISONFLAGS) --output=$(SRC)/parser.yy.c $(SRC)/parser.y

parser: $(SRC)/main.c parser.yy.c lex.yy.c
	$(CC) -o parser $(CFLAGS) $(INCLUDE) $(SRC)/main.c $(SRC)/lex.yy.c \
		$(SRC)/parser.yy.c $(AST)

.PHONY: debug debug-parser

debug: CFLAGS += -g
debug: parser

debug-parser: BISONFLAGS += -v -Wcounterexamples
debug-parser: parser.yy.c
