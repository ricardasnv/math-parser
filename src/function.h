#ifndef FUNCTION_H
#define FUNCTION_H

#include "word.h"

typedef struct function_struct {
	word* formal_args;
	word* body;
} function;

function* make_function(word* formal_args, word* body);

#endif
