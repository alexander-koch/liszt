CC = gcc
CFLAGS = -I. -Wall
FILES = core/util.c \
		core/val.c \
		core/bytebuffer.c \
		lexer/lexer.c \
		parser/parser.c

all:
	$(CC) $(CFLAGS) -g -o lisp main.c $(FILES)
