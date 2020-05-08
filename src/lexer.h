#ifndef LEXER_H
#define LEXER_H

#define BUFFER_LEN 64 // max length of a token in characters

#include "word.h"

word* get_words_from_string(char* str);

#endif
