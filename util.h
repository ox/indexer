#ifndef _UTIL_H
#define _UTIL_H

#include "list.h"

char * strtolower(char * string);

int is_folder(const char * pathname);
char * join_path(const char * dir, const char * file);
struct Node * get_files_in_folder(struct Node * head, const char * pathname);

#endif

