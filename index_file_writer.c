#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hash.h"

int write_formatted_hash_to_file(hash_table * table, char * file) {
  FILE * file_fd;
  int i = 0;
  hash_element * tmp = NULL;

  char ** keys = hash_table_get_all_keys(table);
  sort_strings(keys, table->key_alloc);

  file_fd = fopen(file, "a+");

  for (; i < table->key_alloc; i++) {
    tmp = hash_table_get(table, keys[i]);

    fwrite(file_fd, strlen(keys[i])
  }

  fclose(file_fd);

  return 0;
}

