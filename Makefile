CC = gcc
CFLAGS = -I. -Wall
FILES = core/util.c \
		lexer/lexer.c \
		parser/parser.c \
		adt/bytebuffer.c \
		adt/vector.c \
		val.c

all:
	$(CC) $(CFLAGS) -g -o sloth main.c $(FILES)
