# Obfuscator (OBFS)

**Obfuscator** is a simple program for obfuscating strings in a binary file. It protects the strings in a binary file from being easily viewed using a hex editor or tools like `strings`, which is often done in malware development.

## How It Works

During obfuscation, the program will look for strings in the target binary that start with a unique string (a *marker*), which by default is `[OBFS_ENC]`. It will then encode the string one byte at a time by `XOR`ing it with a key that is continously adjusted (which helps hide common patterns).

For the protected binary to decode its strings, it will need the key used, and the `obfs_decode()` function, which is self contained and should simply be copied into the source of the protected program.

## Installation

```sh
# Clone the repo
git clone https://github.com/4g3nt47/Obfuscator.git
# Compile
cd Obfuscator
mkdir bin
make
# Install
sudo make install
# Clean
make clean
```

## Demo

1. `test.c` (the program to protect);
```c
#include <stdio.h>
#include <string.h>

// The decoding function (modified so we don't have to keep passing the key for every call)
char *obfs_decode(char str[]){

  unsigned char key = 144; // The key in use.
  size_t len = strlen(str);
  unsigned char curr_key;
  for (int i = 0; i < len; i++){
    curr_key = key * (i + 1);
    while (curr_key == 0 || curr_key == 10 || (curr_key >= 32 && curr_key <= 126))
      curr_key += 47;
    str[i] = str[i] ^ curr_key;
    key = curr_key;
  }
  return str;
}

int main(int argc, char **argv){

  // The string to protect.
  char secret[] = "[OBFS_ENC]Th1s 1s 4 v3ry 1mp0rt4nt 5tr1ng! Pr0t3ct it!";
  // Decrypt it.
  obfs_decode(secret);
  printf("%s\n", secret);
  return 0;
}
```

2. Compile the binary with size optimizations (I noticed without it the target strings may be broken apart during compilation);
```sh
$ gcc -Os test.c -o test
```

3. Obfuscate it;
```sh
$ obfuscator test protected 144
[*] Finding strings marked for obfuscation...
[+] 1 targets identified!
[*] Obfuscating strings using key: 0x90...
[+] Offset 0x00002004 : 44 bytes...
[*] Copying remaining bytes...
[+] 1 strings obfuscated :)
```

4. Our target string offset is `0x2004`, let's confirm the string has been obfuscated;

`test` (the unprotected binary)
```
00001FF0   00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00  01 00 02 00  5B 4F 42 46  53 5F 45 4E  ....................[OBFS_EN
0000200C   43 5D 54 68  31 73 20 31  73 20 34 20  76 33 72 79  20 31 6D 70  30 72 74 34  6E 74 20 35  C]Th1s 1s 4 v3ry 1mp0rt4nt 5
00002028   74 72 31 6E  67 21 20 50  72 30 74 33  63 74 20 69  74 21 00 00  01 1B 03 3B  40 00 00 00  tr1ng! Pr0t3ct it!.....;@...
```

`protected` (the protected binary)
```
00001FF0   00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00  01 00 02 00  C4 C5 36 6F  AC 97 F9 5F  ......................6o..._
0000200C   92 8B FE BC  D3 B7 32 9C  C1 68 F8 D2  D9 EA 9C C4  AE AE FC 92  BE AC CA 81  80 CE E8 98  ......2..h..................
00002028   D2 97 9F FB  C7 8F D6 F9  00 00 00 00  00 00 00 00  00 00 00 00  01 1B 03 3B  40 00 00 00  .......................;@...
```

As you can see from the hex dump, the string is now obfuscated. `strings` also didn't find it;
```sh
$ strings test | grep Th1
[OBFS_ENC]Th1s 1s 4 v3ry 1mp0rt4nt 5tr1ng! Pr0t3ct it!
$ strings protected | grep Th1

```

5. Test if the protected binary works as expected;

```sh
$ ./protected 
Th1s 1s 4 v3ry 1mp0rt4nt 5tr1ng! Pr0t3ct it!
```
