bin/main: bin/main.o bin/obfuscator.o
	gcc -Os -s -Wall bin/main.o bin/obfuscator.o -o bin/main

bin/main.o: src/main.c
	gcc -Os -c -Wall src/main.c -o bin/main.o

bin/obfuscator.o: src/obfuscator.c src/obfuscator.h
	gcc -Os -c -Wall src/obfuscator.c -o bin/obfuscator.o

install:
	cp bin/main /usr/local/bin/obfuscator

clean:
	rm bin/*
