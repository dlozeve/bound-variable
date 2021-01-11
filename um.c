#include "uthash.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

struct array {
  uint32_t id;
  uint32_t *arr;
  uint32_t size;
  UT_hash_handle hh;
};

void allocate_array(struct array **arrays, uint32_t next_id, uint32_t size) {
  struct array *a;
  a = malloc(sizeof(struct array));
  a->id = next_id;
  a->arr = calloc(size, sizeof(uint32_t));
  a->size = size;
  HASH_ADD(hh, *arrays, id, sizeof(uint32_t), a);
}

struct array *get_array(struct array *arrays, uint32_t id) {
  struct array *a;
  HASH_FIND(hh, arrays, &id, sizeof(uint32_t), a);
  return a;
}

void free_array(struct array **arrays, uint32_t id) {
  struct array *a;
  HASH_FIND(hh, *arrays, &id, sizeof(uint32_t), a);
  HASH_DEL(*arrays, a);
  free(a->arr);
  free(a);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <program>\n", argv[0]);
    return EXIT_SUCCESS;
  }

  struct stat file_stat;
  if (stat(argv[1], &file_stat) == -1) {
    fprintf(stderr, "Could not stat file %s\n", argv[1]);
    return EXIT_FAILURE;
  }

  FILE *fp = fopen(argv[1], "rb");
  if (fp == NULL) {
    fprintf(stderr, "Could not open file %s\n", argv[1]);
    return EXIT_FAILURE;
  }

  uint32_t *program = malloc(file_stat.st_size);

  int c = 0;
  size_t n = -1;
  int i = 0;
  while ((c = fgetc(fp)) != EOF) {
    if (i == 0) {
      n++;
      program[n] = c;
    } else {
      program[n] = (program[n] << 8) | c;
    }
    i = (i + 1) % 4;
  }
  fclose(fp);

  uint32_t reg[8] = {0};       // registers
  struct array *arrays = NULL; // arrays of platters
  uint32_t next_id = 1;        // next free index available for a new array

  size_t pc = 0;
  for (;;) {
    uint32_t opcode = program[pc] >> 28;
    uint32_t c = program[pc] & 7;
    uint32_t b = (program[pc] >> 3) & 7;
    uint32_t a = (program[pc] >> 6) & 7;
    /* printf("\n%zu: %u (op=%u, a=%u, b=%u, c=%u)\n", pc, program[pc], opcode, a, b, c); */
    /* printf("reg[a] = %u, reg[b] = %u, reg[c] = %u\n", reg[a], reg[b], reg[c]); */
    pc++;
    switch (opcode) {
    case 0:
      if (reg[c]) {
        reg[a] = reg[b];
      }
      break;
    case 1: {
      uint32_t *arr = reg[b] ? get_array(arrays, reg[b])->arr : program;
      reg[a] = arr[reg[c]];
      break;
    }
    case 2: {
      uint32_t *arr = reg[a] ? get_array(arrays, reg[a])->arr : program;
      arr[reg[b]] = reg[c];
      break;
    }
    case 3:
      reg[a] = reg[b] + reg[c];
      break;
    case 4:
      reg[a] = reg[b] * reg[c];
      break;
    case 5:
      reg[a] = reg[b] / reg[c];
      break;
    case 6:
      reg[a] = (~reg[b]) | (~reg[c]);
      break;
    case 7:
      return EXIT_SUCCESS;
    case 8:
      allocate_array(&arrays, next_id, reg[c]);
      reg[b] = next_id;
      next_id++;
      break;
    case 9:
      free_array(&arrays, reg[c]);
      break;
    case 10:
      putchar(reg[c]);
      break;
    case 11:
      reg[c] = getchar();
      break;
    case 12: {
      if (reg[b]) {
        free(program);
        struct array *arr = get_array(arrays, reg[b]);
        program = malloc(arr->size * sizeof(uint32_t));
        memcpy(program, arr->arr, arr->size * sizeof(uint32_t));
      }
      pc = reg[c];
      break;
    }
    case 13:
      a = (program[pc - 1] >> 25) & 7;
      reg[a] = program[pc - 1] & 0x1FFFFFF;
      break;
    }
  }
  free(program);
  free(arrays);

  return EXIT_SUCCESS;
}
