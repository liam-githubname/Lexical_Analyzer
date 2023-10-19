#include "lexer.h"
#include <stdio.h>

int main(int argc, char *argv[]) {

  lexer_init(argv[1]);

  lexer_output();

  printf("\n");
  return 0;
}
