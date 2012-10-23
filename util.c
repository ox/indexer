#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/dir.h>

#include "util.h"

#include "list.h"

/*  func: strtolower(string)
 
 convert string into lower case.
 /!/
 string is modified! Malloc a new string.
 /!/
 */
char *strtolower(char* string) {
    int q;
    for(q = 0; q < strlen(string); q++) {
        string[q] = tolower(string[q]);
    }
    return string;
}

char * join_path(const char * dir, const char * file) {
  char * full_path = malloc(strlen(dir) + strlen(file) + 3);
  sprintf(full_path, "%s/%s", dir, file);
  return full_path;
}

struct Node * get_files_in_folder(struct Node * head, const char * pathname) {
  DIR *dp;
  struct dirent *ep;

  dp = opendir (pathname);
  if (dp != NULL) {
    while ((ep = readdir (dp))) {
      if (ep->d_type == DT_DIR
          && (strcmp(ep->d_name, ".") != 0)
          && (strcmp(ep->d_name, "..") != 0)) {
            head = get_files_in_folder(head, join_path(pathname, ep->d_name));
      } else if (ep->d_type == DT_REG) {
        struct Node * tmp = malloc(sizeof(struct Node));

        tmp->data = join_path(pathname, ep->d_name);

        if (!tmp->data) {
          fprintf(stderr, "could not get filepath for %s\n", ep->d_name);
        }

        tmp->next = head;
        head = tmp;
      }
    }
  }

  return head;
}

/*  func: sort_strings(arr, size)
 
 sort <size> strings in <arr> in lexicongraphical order.
 */
void sort_strings(char ** arr, int size) {
  int i;
  for(i = 0; i < size; i++) {
    int lowest = i;
    
    int k;
    for(k = i; k < size; k++)
      if(strcmp(arr[k], arr[lowest])<0) lowest = k;
    
    char * tmp = arr[i];
    arr[i] = arr[lowest];
    arr[lowest] = tmp;
  }
}

void print_hash_keys_and_values(struct hash_table * table) {
  char ** arr = hash_table_get_all_keys(table);
  struct hash_node * head;
  struct file_node * head_node;
  int i = 0;
  
  sort_strings(arr, table->key_alloc);
  
  for(; i < table->key_alloc; i++) {
    head = hash_table_get(table, arr[i]);
    printf("%s\n", head->word);
    
    for(head_node = head->appears_in; head_node != NULL; head_node = head_node->next) {
      printf("\t%s\n", head_node->file_name);
    }
    
    printf("\n");
  }
  
  free(arr);
}