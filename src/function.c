#include <stdlib.h>
#include "function.h"

function* make_function(word* formal_args, word* body) {
	function* new_fun = malloc(sizeof(function));
	new_fun->formal_args = formal_args;
	new_fun->body = body;
	return new_fun;
}

