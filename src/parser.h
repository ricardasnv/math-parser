#ifndef PARSER_H
#define PARSER_H

#include "word.h"

word* infix_to_postfix(word* base);
word* eval_postfix(word* base);

void apply_operator(word* evalstack);

#endif
