/**
 *------------------------------------------------------------------
 *    A launcher for Obfuscator to use the library as a CLI program.
 * Args: <input-file> <output-file> <numeric-key> 
 *                                             Author: Umar Abdul
 *---------------------------------------------------------------
 */

#include "obfuscator.h"

int main(int argc, char **argv){

  if (argc < 4){
    printf("[-] Usage: %s <infile> <outfile> <key>\n", argv[0]);
    return 1;
  }
  FILE *src = fopen(argv[1], "r");
  if (!src){
    printf("[-] Error opening input file!\n");
  }
  FILE *dest = fopen(argv[2], "w");
  if (!dest){
    printf("[-] Error opening output file!\n");
    return 1;
  }
  obfs_run(dest, src, atoi(argv[3]), 1);
  fclose(src);
  fclose(dest);
  return 0;
}
