#ifndef PARSER_H
#define PARSER_H

#include "word.h"

word* infix_to_rpn(word* base);
word* eval_rpn(word* base);

void apply_operator(word* evalstack);

#endif
