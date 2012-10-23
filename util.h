#ifndef _UTIL_H
#define _UTIL_H

#include "list.h"
#include "hash.h"

char * strtolower(char * string);

int is_folder(const char * pathname);
char * join_path(const char * dir, const char * file);
struct Node * get_files_in_folder(struct Node * head, const char * pathname);
void print_hash_keys_and_values(struct hash_table * table);

#endif

