
all: simple-linenoise

linenoise.o: linenoise.c
	$(CC) -Wall -W -Os -g -c -o $@ $<

simple-linenoise: simple-linenoise.c linenoise.o
	$(CC) -Wall -W -Os -g linenoise.o -o $@ $<

clean:
	rm -f linenoise.o simple-linenoise
