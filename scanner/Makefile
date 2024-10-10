CC		= gcc
CFLAGS	= -std=c11 -Wall -Wextra

BUILD	= build
INCLUDE	= -Iinclude/
OBJ_DIR = $(BUILD)/objects
SRC		= src

lex.yy.c: $(SRC)/scanner.flex
	flex -o $(SRC)/lex.yy.c $(SRC)/scanner.flex

scanner: lex.yy.c
	$(CC) $(CFLAGS) -o scanner $(INCLUDE) $(SRC)/main.c $(SRC)/lex.yy.c

.PHONY: debug

debug: CFLAGS += -g
debug: scanner
