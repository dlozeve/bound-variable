#include "uthash.h"
#include <stdint.h>

struct array {
  uint32_t id;
  uint32_t *arr;
  uint32_t size;
  UT_hash_handle hh;
};

void allocate_array(struct array **arrays, uint32_t next_id, uint32_t size);
struct array *get_array(struct array *arrays, uint32_t id);
void free_array(struct array **arrays, uint32_t id);
uint32_t *read_program(char *filename);
void run(uint32_t *program);
