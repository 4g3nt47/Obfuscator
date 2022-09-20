bin/obfuscator: src/obfuscator.c src/obfuscator.h
	gcc -Os -s -Wall src/obfuscator.c -o bin/obfuscator
