#include "hash.h"

/* returns the name of the file that contains all the words */
char * index_file(char * file, const char * root);
struct hash_table * hash_table_from_word_count(char * file);
