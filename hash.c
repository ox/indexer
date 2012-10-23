#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <assert.h>

#include "hash.h"

#define cmp(a, b) printf("[%s:%s] %i\n", a, b, strcmp(a, b));

/*
 HASH ELEMENT HELPERS
 */

struct file_node * new_file_node(char * file_name) {
  struct file_node * tmp = malloc(sizeof(struct file_node));
  
  tmp->count = 1;
  tmp->file_name = malloc(strlen(file_name));
  strcpy(tmp->file_name, file_name);
  tmp->next = NULL;
  
  return tmp;
}

struct hash_node * new_hash_node(char * word) {
  struct hash_node * tmp = malloc(sizeof(struct hash_node));
  
  tmp->word = malloc(strlen(word) + 1);
  strcpy(tmp->word, word);
  tmp->next = NULL;
  tmp->appears_in = NULL;
  
  return tmp;
}

void free_file_node(struct file_node * node) {
  free(node->file_name);
  free(node);
}

void free_file_nodes(struct file_node * node) {
  struct file_node * head = node->next, * next;
  
  // free(node->file_name);
  while(head != NULL) {
    next = head->next;
    free_file_node(head);
    head = next;
  }
}

void free_hash_node(struct hash_node * node) {
  free_file_nodes(node->appears_in);
  free(node->word);
  free(node);
}

void free_hash_nodes(struct hash_node * node) {
  struct hash_node * head = node->next, * next;
  
  //free(node->word);
  free_file_nodes(node->appears_in);
  while(head != NULL) {
    next = head->next;
    free_hash_node(head);
    head = next;
  }
}

/*  func: hash_element_add_occurance(element, str)
 
 if str == element->str then it's just another occurance since
 str is already in the case-sensitive occurances** array. Otherwise
 an existing copy of str is sought in occurances** and if it's not
 found, it is added. count of the str is incrimented in all cases.
 */
int hash_node_add_occurance(struct hash_node * node, char * file_name) {
  struct file_node * head = node->appears_in;
  while (head != NULL) {
    if (strcmp(head->file_name, file_name) == 0) {
      head->count++;
      break;
    } else {
      head = head->next;
    }
  }
  
  if (head == NULL) {
    struct file_node * tmp = new_file_node(file_name);
    tmp->next = node->appears_in;
    node->appears_in = tmp;
    
    return 0;
  } else {
    return 1;
  }
}

struct hash_table * new_hash_table(int size) {
  struct hash_table * table = malloc(sizeof(struct hash_table));
  assert(table != NULL);
  
  if(!table) {
    fprintf(stderr, "no memory while allocating hash_table\n");
    return NULL;
  }
  
  table->size = 0;
  table->key_num = size;
  table->key_alloc = 0;
  table->storage = calloc(table->key_num, sizeof(struct hash_node *));
  
  if(table->storage == NULL) {
    fprintf(stderr, "no memory while allocating table->store\n");
    free(table);
    return NULL;
  }
  
  return table;
}

/*
 HASH TABLE
 */

/*  func: lua_hash(str)
 
 function takes in a string and returns a positive int hash.
 */
int lua_hash(char *str) {
  int l = (int)strlen(str);
  int i, step = ((l >> 5) + 1);
  int h = l + (l >= 4?*(int*)str:0);
  for( i=l; i >= step; i -= step) {
    h = h^(( h<<5 ) + (h >> 2) + ((unsigned char *)str)[i-1]);
  }
  
  /* abs(h) is a modification on the original lua hash
   to make it work better with my method of determining a
   table index using lua_hash%table->key_num */
  return abs(h);
}


/*  func: hash_table_store(table, str, element)
 
 function stores an element in the hash_table table under the key str.
 if there are collisions under the key, it will append the element
 to the linked list at that location.
 
 /!/ This is a store-by-copy hash table! free() your hash_elements
 after you store them in the table.
 
 Example:
 hash_table *table = new_hash_table(16);
 hash_element *tmp = new_hash_element("example");
 hash_table_store(table, tmp->str, tmp);
 free(tmp);
 /!/
 */
void hash_table_store(struct hash_table * table, char * word, struct hash_node * node) {
  if (table->storage[lua_hash(word)%table->key_num] != NULL) {
    /* replace */
    struct hash_node * head = hash_table_get(table, word), * prev;
    
    if (strcmp(head->word, word) == 0) {
      table->storage[lua_hash(word)%table->key_num] = node;
      node->next = head->next;
      free_hash_node(head);
    } else {
      while (head->next != NULL) {
        if (strcmp(head->word, word) == 0) {
          prev->next = node;
          node->next = head->next;
          free_hash_node(head);
          break;
        }
        
        prev = head;
        head = head->next;
      }
    }
  } else {
    /* insert */
    table->storage[lua_hash(word)%table->key_num] = node;
  }
}


/*  func: hash_table_get(table, str)
 
 function takes a hash_table pointer and a string and returns
 a pointer to the hash_element in the table. It will *always*
 return a pointer to the right hash_element, even if there are
 multiple collisions under the hash of str.
 
 suitable for modifying hash_elements;
 */
struct hash_node * hash_table_get(struct hash_table* table, char * word) {
  int hash = lua_hash(word);
  struct hash_node * head;

  if(table->storage[hash%table->key_num] == NULL) {
    return NULL;
  } else {
    head = table->storage[hash%table->key_num];

    if(head->next == NULL) {
      return head;
    }

    while(head != NULL) {
      if (strcmp(head->word, word) == 0) {
        return head;
      } else {
        head = head->next;
      }
    }
  }
  return NULL;
}

/*  func: hash_table_get_all_keys(table)
 
 function takes a hash_table pointer and returns an array of
 hash_element pointers (hash_element **).
 */
char ** hash_table_get_all_keys(struct hash_table *table) {
  char ** arr = malloc(table->key_alloc * sizeof(char *));
  struct hash_node * head = NULL;
  
  int i = 0, k = 0;
  for(; i < table->key_num; i++) {
    if(table->storage[i]) {
      head = table->storage[i];
      
      while (head != NULL) {
        arr[k] = head->word;
        k++;
        head = head->next;
      }
    }
  }
  
  return arr;
}

void hash_table_free(struct hash_table *table) {
  int i = 0;
  for(; i < table->key_num; i++) {
    if(table->storage[i] != NULL) {
      free_hash_nodes(table->storage[i]);
    }
  }
  free(table->storage);
  // free(table);
}
