
all: simple-linenoise ell-linenoise

linenoise.o: linenoise.c
	$(CC) -Wall -W -Os -g -c -o $@ $<

simple-linenoise: simple-linenoise.c linenoise.o
	$(CC) -Wall -W -Os -g linenoise.o -o $@ $<

ell-linenoise: ell-linenoise.c linenoise.o
	$(CC) -Wall -W -Os -g linenoise.o -o $@ $< `pkg-config --libs ell`

clean:
	rm -f linenoise.o simple-linenoise ell-linenoise
