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
  return obfs_run(argv[1], argv[2], atoi(argv[3]), 1);
}
