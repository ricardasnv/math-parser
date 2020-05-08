#ifndef OPERATORS_H
#define OPERATORS_H

#include "word.h"

typedef struct {
	char* repr; // representation
	int prec;   // precedence
} operator_struct;

#define VALID_OPERATOR_CHARS "+-*/^"
#define NUM_OF_OPERATORS 5
extern const operator_struct VALID_OPERATORS[NUM_OF_OPERATORS];

// Returns 1 if op1 has higher precedence
int get_precedence(word* w);
int higher_precedence(word* op1, word* op2);

#endif
