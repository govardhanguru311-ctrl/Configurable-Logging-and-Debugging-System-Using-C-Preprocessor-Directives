#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>


void file_inclusion1(void);
void file_inclusion(FILE *fp1, FILE *fp2);
void comment_removal(FILE *fp1);
void macro_replace_const(char *macro, char *replace);
void macro_replace_fun(char *macro, char *arg, char *replace);
int is_word_char(char ch);
void macro_replacement();

