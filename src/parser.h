#ifndef PARSER_H
#define PARSER_H

#include "word.h"
#include "environment.h"

word* infix_to_postfix(word* base, environment* env);

word* eval_postfix(word* base, environment* env);
word* eval_symbol_var(word* sym, environment* env);

void apply_operator(word* evalstack, environment* env);
void apply_function(environment* func, word* argstack, environment* env);

#endif
