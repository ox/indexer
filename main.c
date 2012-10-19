#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/dir.h>

#include <unistd.h>

#include "hash.h"
#include "list.h"
#include "util.h"
#include "indexer.h"

int main(int argc, char ** argv) {
  struct Node * head = NULL;
  char cwd[PATH_MAX];

  if (argc != 3) {
    puts("usage:\n\tindexer <index file> <dir or file>");
    return 0;
  }

  /* get a list of files or just use the file given */
  getcwd(cwd, sizeof(cwd));
  strcat(cwd, "/");
  strcat(cwd, argv[2]);

  head = get_files_in_folder(head, cwd);

  getcwd(cwd, sizeof(cwd));
  strcat(cwd, "/tmp");

  puts(cwd);

  while (head) {
    puts(head->data);
    index_file(head->data, cwd);
    head = head->next;
  }

  return 0;
}
