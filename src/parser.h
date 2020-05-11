#ifndef PARSER_H
#define PARSER_H

#include "word.h"
#include "environment.h"

word* infix_to_postfix(word* base, environment* env);
word* revinfix_to_postfix(word* base, environment* env);

word* extract_expressions(word* input);

word* eval_postfix(word* base, environment* env);
word* eval_symbol_var(word* sym, environment* env);

void apply_operator(word* stack, environment* env);
void apply_function(function* func, word* stack, environment* env);

#endif
