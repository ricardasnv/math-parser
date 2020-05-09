#include "operators.h"
#include <stdio.h>
#include <string.h>

// All operators ordered from highest to lowest precedence
const operator_struct VALID_OPERATORS[NUM_OF_OPERATORS] = {
	{"=", 0},
	{"+", 1},
	{"-", 1},
	{"*", 2},
	{"/", 2},
	{"^", 3}
};

int get_precedence(word* w) {
	if (w->type != OPERATOR) {
		printf("WARNING: non-operator of type %d given to get_precedence.\n", w->type);
		return -1;
	}

	for (int i = 0; i < NUM_OF_OPERATORS; i++) {
		if (strcmp(w->op, VALID_OPERATORS[i].repr) == 0) {
			return VALID_OPERATORS[i].prec;
		}
	}

	printf("WARNING: can't find operator \'%s\' in VALID_OPERATORS.\n", w->op);
	return -1;
}

int higher_precedence(word* op1, word* op2) {
	return get_precedence(op1) > get_precedence(op2);
}

