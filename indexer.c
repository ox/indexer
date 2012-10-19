#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <regex.h>

#include "util.h"
#include "indexer.h"
#include "hash.h"

#define CHUNK_SIZE 4096
#define TABLE_SIZE 1021

char * index_file(char * file, const char * root) {
  char * result_file;
  FILE * tmp_file_fd;
  hash_table * reverse_hash = NULL;

  /* generate a temp file name to creat and write to */
  result_file = tmpnam(NULL);
  printf("indexing to file %s\n", result_file);

  /* get a rever hash table of the terms in the file */
  reverse_hash = hash_table_from_word_count(file);

  

  free(result_file);
  fclose(tmp_file_fd);
  hash_table_free(reverse_hash);

  return result_file;
}

hash_table * hash_table_from_word_count(char * file) {
  FILE * file_fd;
  int file_size = 0;
  int consumed = 0;

  char * buffer = calloc(CHUNK_SIZE, sizeof(char));
  assert(buffer != NULL);
  char * leftover = calloc(CHUNK_SIZE * 2, sizeof(char));
  assert(leftover != NULL);

  char * leftover_start = leftover;
  char * last_word;

  int error;

  /* regex pattern and match for finding the words */
  regex_t regex;
  regmatch_t pm;

  /* the hash table that will store our lists and words */
  hash_table* table = new_hash_table(TABLE_SIZE);

  /* get the file size */
  file_fd = fopen(file, "r");
  fseek(file_fd, 0, SEEK_END);
  file_size = ftell(file_fd);
  rewind(file_fd);

  error = regcomp(&regex, "([[:alpha:]][[:alnum:]]*)", REG_EXTENDED);
  assert(error == 0);

  while(consumed < file_size) {
      fread(buffer, sizeof(char), CHUNK_SIZE, file_fd);
      if(buffer == NULL) {
          printf("buffer == NULL, consumed: %i, file_size: %i\n", consumed, file_size);
          return NULL;
      }

      strcat(leftover, buffer);

      error = regexec(&regex, leftover, 1, &pm, 0);
      while( error == 0 ) {
          char *word = malloc((int)pm.rm_eo - (int)pm.rm_so + 2 );
          strncpy(word, leftover + pm.rm_so, (int)pm.rm_eo - (int)pm.rm_so + 1);

          char *lowered = malloc(strlen(word) + 1);
          strcpy(lowered, word);
          strtolower(lowered);

          error = regexec(&regex, leftover += pm.rm_eo , 1, &pm, REG_NOTBOL);
          last_word = word;

          if(error != 0 && file_size - consumed > CHUNK_SIZE) {
              strncpy(leftover, last_word, strlen(last_word) + 1);
          } else {
              hash_element *tmp = hash_table_get(table, lowered);

              if(tmp) {
                  hash_element_add_occurance(tmp, word);
              } else {
                  hash_table_store(table, lowered, new_hash_element(word));
              }
          }

          free(lowered);
          free(word);
      }

      leftover = leftover_start;
      consumed += sizeof(char) * CHUNK_SIZE;
  }

  regfree(&regex);
  free(buffer);
  free(leftover);
  fclose(file_fd);
}

