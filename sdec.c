#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32
typedef unsigned char BYTE;
typedef unsigned long WORD;
typedef unsigned long long DWORD;
#else
#include <windows.h>
#endif

#include "aes256.h"

/*
void DumpHex(const void* data, size_t size) {
	char ascii[17];
	size_t i, j;
	ascii[16] = '\0';
	for (i = 0; i < size; ++i) {
		printf("%02X ", ((unsigned char*)data)[i]);
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char*)data)[i];
		} else {
			ascii[i % 16] = '.';
		}
		if ((i+1) % 8 == 0 || i+1 == size) {
			printf(" ");
			if ((i+1) % 16 == 0) {
				printf("|  %s \n", ascii);
			} else if (i+1 == size) {
				ascii[(i+1) % 16] = '\0';
				if ((i+1) % 16 <= 8) {
					printf(" ");
				}
				for (j = (i+1) % 16; j < 16; ++j) {
					printf("   ");
				}
				printf("|  %s \n", ascii);
			}
		}
	}
}
*/

void error(const char *what)
{
  fprintf(stderr, "%s failed.", what);
  exit(1);
}

BYTE *hex2byte(const char *hex)
{
  int len = strlen(hex) / 2;
  BYTE *bytes = malloc(len);
  if (bytes == NULL) {
    error("malloc");
    return NULL;
  }
  //unsigned char val[2];
  BYTE val;

  for (int i = 0; i < len; i++) {
    sscanf(&hex[i * 2], "%2hhx", &val);
    bytes[i] = val;
  }
  return bytes;
}

int main(int argc, char *argv[])
{

  DWORD numBytes = 16;
  aes256_context ctxt;


  if ((argc < 3) || (argc > 5)) {
    fprintf(stderr, "Usage: %s <key_file> <input_file> [<output_file>] [<magic_file>]\n", argv[0]);
    return 1;
  }

  BYTE key[32];

  FILE *key_file = fopen(argv[1], "rb");
  if (!key_file) {
    perror("Error opening input file");
    return 1;
  }

  if (!fread(key, 1, 32, key_file))
    perror("Error reading key file.");

  fclose(key_file);

  aes256_init(&ctxt, key);

  FILE *input_file = fopen(argv[2], "rb");
  if (!input_file) {
    perror("Error opening input file");
    return 1;
  }

  FILE *output_file;

  if (argc > 3) {
    output_file = fopen(argv[3], "wb");
    if (!output_file) {
      perror("Error opening output file");
      fclose(input_file);
      return 1;
    }
  } else {
    output_file = stdout;
  }

  FILE *magic_file = NULL;

  if (argc == 5) {
    magic_file = fopen(argv[4], "wb");
    if (!magic_file) {
      perror("Error opening output file");
      fclose(input_file);
      return 1;
    }
  }

  BYTE blk[numBytes];

  if (!fread(blk, 1, numBytes, input_file))
    perror("Error reading file.");      // throw first block

  if (argc == 5) {
    aes256_decrypt_ecb(&ctxt, blk);
    //DumpHex(blk,16);
    fwrite(blk, 1, numBytes, magic_file);
  }

  if (!fread(blk, 1, numBytes, input_file))
    perror("Error reading file.");      // throw first block

  if (argc == 5) {
    aes256_decrypt_ecb(&ctxt, blk);
    //DumpHex(blk,16);
    fwrite(blk, 1, numBytes, magic_file);
  }
  while (fread(blk, 1, numBytes, input_file) == numBytes) {

    aes256_decrypt_ecb(&ctxt, blk);

    fwrite(blk, 1, numBytes, output_file);
  }

  fclose(input_file);
  fclose(output_file);
  if (argc == 4) {
    fclose(magic_file);
  }
  return 0;
}
