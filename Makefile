CC := gcc
PREFIX := usr/local
CFLAGS := -I. -Wall
MODULE := liszt
FILES := core/util.c \
		core/val.c \
		core/bytebuffer.c \
		lexer/lexer.c \
		parser/parser.c

$(MODULE):
	$(CC) $(CFLAGS) -O2 -o $(MODULE) main.c $(FILES)

install: $(MODULE)
	install $(MODULE) $(PREFIX)/bin

uninstall:
	rm $(PREFIX)/bin/$(MODULE)

clean:
	rm $(MODULE)
