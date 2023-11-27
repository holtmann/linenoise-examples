
all: simple-linenoise

simple-linenoise: simple-linenoise.c
	$(CC) -Wall -W -Os -g -o $@ $< -llinenoise

clean:
	rm -f simple-linenoise
