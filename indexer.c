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
  FILE * tmp_file_fd = NULL;
  struct hash_table * reverse_hash = NULL;
  
  /* generate a temp file name to creat and write to */
  result_file = tmpnam(NULL);
  printf("indexing to file %s\n", result_file);
  
  /* get a rever hash table of the terms in the file */
  reverse_hash = hash_table_from_word_count(file);
  
  print_hash_keys_and_values(reverse_hash);
  
  fclose(tmp_file_fd);
  
  if (reverse_hash) {
    hash_table_free(reverse_hash);
  }
  
  return result_file;
}

struct hash_table * hash_table_from_word_count(char * file) {
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
  
  struct hash_table * table = new_hash_table(TABLE_SIZE);
  
  file_fd = fopen(file, "r");
  
  if (file_fd == NULL) {
    fprintf(stderr, "couldn't get handler for %s\n", file);
    free(buffer);
    free(leftover);
    hash_table_free(table);
    return NULL;
  }
  
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
      char * word = malloc((int)pm.rm_eo - (int)pm.rm_so + 2 );
      strncpy(word, leftover + pm.rm_so, (int)pm.rm_eo - (int)pm.rm_so + 1);
      puts(word);
      strtolower(word);
      
      error = regexec(&regex, leftover += pm.rm_eo , 1, &pm, REG_NOTBOL);
      last_word = word;
      
      if(error != 0 && file_size - consumed > CHUNK_SIZE) {
        strncpy(leftover, last_word, strlen(last_word) + 1);
      } else {
        struct hash_node * tmp = hash_table_get(table, word);
        
        if(tmp) {
          hash_node_add_occurance(tmp, file);
        } else {
          struct hash_node * node = new_hash_node(word);
          node->appears_in = new_file_node(file);
          hash_table_store(table, word, node);
        }
      }
    }
    
    leftover = leftover_start;
    consumed += CHUNK_SIZE;
  }
  
  regfree(&regex);
  free(buffer);
  free(leftover);
  fclose(file_fd);
  
  return table;
}

