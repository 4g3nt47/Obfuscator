/**
 *-------------------------------------------------------------------------------------
 *   Obfuscator - A program for obfuscating strings in a program.
 * This is designed to prevent sensitive strings from showing up when dumping strings
 * from a binary, but the encoding scheme is not that secure.
 *   This should be used as a standalone program from the CLI to obfuscate a file with
 * a given key. You only need to copy the obfs_decode() function to your source code to
 * be able to decode the strings at runtime.
 *                                                                   Author: Umar Abdul
 *-------------------------------------------------------------------------------------
 */

#include "obfuscator.h"

void obfs_encode(unsigned char key, char *str){
  
  size_t len = strlen(str);
  unsigned char curr_key;
  for (int i = 0; i < len; i++){
    curr_key = key * (i + 1);
    while (curr_key == 0 || curr_key == 10 || (curr_key >= 32 && curr_key <= 126))
      curr_key += 47;
    str[i] = str[i] ^ curr_key;
  }
}

void obfs_decode(unsigned char key, char *str){

  size_t len = strlen(str);
  unsigned char curr_key;
  for (int i = 0; i < len; i++){
    curr_key = key * (i + 1);
    while (curr_key == 0 || curr_key == 10 || (curr_key >= 32 && curr_key <= 126))
      curr_key += 47;
    str[i] = str[i] ^ curr_key;
  }
}

long obfs_find_offset(FILE *rfo, const void *target, size_t len){

  void *buff = malloc(len);
  int c;
  size_t n = fread(buff, 1, len, rfo);
  if (!n)
    return -1;
  while (1){
    if (!memcmp(buff, target, len)){
      free(buff);
      return ftell(rfo) - len;
    }
    for (int i = 1; i < len; i++) // Shift all bytes back one place, discarding the first byte.
      *((unsigned char *)buff + (i - 1)) = *((unsigned char *)buff + i);
    // Load one new byte to the end of the buffer.
    c = fgetc(rfo);
    if (c == EOF)
      break;
    *((unsigned char *)buff + (len - 1)) = c;
  }
  free(buff);
  return -1;
}

ssize_t obfs_filecpy(FILE *dest, FILE *src, size_t len){

  size_t n;
  int c;
  for (n = 0; n < len; n++){
    if ((c = fgetc(src)) == EOF)
      break;
    fputc(c, dest);
  }
  return n;
}

int obfs_run(char *infile, char *outfile, unsigned char key, short int verbose){

  FILE *rfo = fopen(infile, "r");
  if (!rfo){
    if (verbose)
      printf("[-] Error opening file: %s\n", infile);
    return 1;
  }
  if (verbose)
    printf("[*] Finding strings marked for obfuscation in file: %s...\n", infile);
  unsigned short marker_len = strlen(OBFS_MARKER);
  ssize_t *offsets = malloc(sizeof(ssize_t) * OBFS_MAX_OFFSETS_COUNT * 2);
  size_t offsets_count = 0;
  size_t target_len;
  int c;
  for (int index = 0; index < OBFS_MAX_OFFSETS_COUNT && (!feof(rfo)); index++){
    ssize_t offset = obfs_find_offset(rfo, OBFS_MARKER, marker_len);
    if (offset == -1)
      break;
    printf("0x%08x\n", (unsigned int)offset);
    // `rfo` if now pointed to the beginning of a target string (with the marker skipped).
    target_len = 0;
    while (1){
      c = fgetc(rfo);
      if (c == EOF || c == 0)
        break;
      target_len++;
    }
    *(offsets + (index * OBFS_MAX_OFFSETS_COUNT)) = offset;
    *(offsets + (index * OBFS_MAX_OFFSETS_COUNT) + 1) = target_len + marker_len;
    offsets_count++;
  }
  if (verbose)
    printf("[+] %d targets identified!\n", (unsigned int)offsets_count);
  if (offsets_count == 0)
    goto end;
  FILE *wfo = fopen(outfile, "w");
  if (!wfo){
    if (verbose)
      printf("[-] Error opening file: %s\n", outfile);
    goto end;
  }
  if (verbose)
    printf("[*] Obfuscating strings using key: 0x%02x...\n", key);
  rewind(rfo);
  for (int i = 0; i < offsets_count; i++){
    size_t offset = *(offsets + (i * OBFS_MAX_OFFSETS_COUNT));
    size_t target_len = *(offsets + (i * OBFS_MAX_OFFSETS_COUNT) + 1);
    // Copy all bytes preceding the obfuscated string.
    // This will point `rfo` to the beginning of the marker, and `wfo` to the place our obfuscated string should be.
    obfs_filecpy(wfo, rfo, offset - ftell(rfo));
    fseek(rfo, marker_len, SEEK_CUR); // Skip over the marker.
    // Encode the string.
    size_t org_str_len = target_len - marker_len;
    char *buff = malloc(target_len); // This also includes the length of the marker, which we will fill with nulls.
    memset(buff, 0, target_len);
    fread(buff, 1, org_str_len, rfo);
    obfs_encode(key, buff);
    fwrite(buff, 1, target_len, wfo); // Write the encoded string with marker removed.
    free(buff);
    if (verbose)
      printf("[+] Offset 0x%08x : %ld bytes...\n", (unsigned int)offset, org_str_len);
  }
  // Copy the remaining bytes.
  if (verbose)
    printf("[*] Copying remaining bytes...\n");
  while ((c = fgetc(rfo)) != EOF)
    fputc(c, wfo);
  fclose(wfo);
  printf("[+] %ld strings obfuscated :)\n", offsets_count);

  // We are done :)
  end:
    fclose(rfo);
    free(offsets);
    return 0;
}

int main(int argc, char **argv){

  if (argc < 4){
    printf("[-] Usage: %s <infile> <outfile> <key>\n", argv[0]);
    return 1;
  }
  return obfs_run(argv[1], argv[2], atoi(argv[3]), 1);
}
