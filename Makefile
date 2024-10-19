CC		= gcc
CFLAGS	= -std=gnu11 -Wall -Wextra

BUILD	= build
INCLUDE	= -Iinclude/
OBJ_DIR = $(BUILD)/objects
SRC		= src
LEXER	= $(SRC)/lex.yy.c
PARSER	= $(SRC)/parser.yy.c
AST		= $(SRC)/ast/decl.c $(SRC)/ast/expr.c $(SRC)/ast/param_list.c \
		  $(SRC)/ast/stmt.c $(SRC)/ast/type.c
SEMANTIC= $(SRC)/hash.c $(SRC)/symbol.c $(SRC)/typecheck.c

BISONFLAGS = --header=include/yy.h

lexer: $(SRC)/scanner.flex
	flex -o $(LEXER) $(SRC)/scanner.flex

parser: $(SRC)/parser.y
	bison $(BISONFLAGS) --output=$(PARSER) $(SRC)/parser.y

bmcc: parser lexer
	$(CC) $(CFLAGS) -o bmcc $(INCLUDE) $(SRC)/main.c $(LEXER) $(PARSER) \
		$(AST) $(SEMANTIC)

.PHONY: debug debug-parser

debug: CFLAGS += -g
debug: bmcc

debug-parser: BISONFLAGS += -v -Wcounterexamples
debug-parser: parser.yy.c
