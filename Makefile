CC := gcc
CFLAGS := -I. -Wall
MODULE := liszt
FILES := core/util.c \
		core/val.c \
		core/bytebuffer.c \
		lexer/lexer.c \
		parser/parser.c

all:
	$(CC) $(CFLAGS) -O2 -o $(MODULE) main.c $(FILES)
