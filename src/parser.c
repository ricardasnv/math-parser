#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "parser.h"
#include "word.h"
#include "operators.h"
#include "error.h"

// shorthands
int is_left_bracket(word* w);
int is_right_bracket(word* w);

// Convert infix to postfix using the shunting yard algorithm
// NOTE: does not modify passed wordstack
word* infix_to_postfix(word* passed_base, environment* env) {
	word* infix = ws_copy(passed_base);
	word* postfix = make_base_word();
	word* opstack = make_base_word();

	ws_reverse(infix);

	while (!ws_isempty(infix)) {
		word* w = ws_pop(infix);

		// determine word type
		if (is_number_word(w)) {
			ws_push(postfix, w);
		} else if (is_symbol_word(w)) {
			// attempt to resolve
			environment* result = resolve_symbol(w, env);

			// if variable or undefined, push to output
			if (result == NULL || result->type == VARIABLE) {
				ws_push(postfix, w);
			} else if (result->type == FUNCTION) {
				ws_push(opstack, w);
			}
		} else if (is_left_bracket(w)) {
			ws_push(opstack, w);
		} else if (is_right_bracket(w)) {
			// special logic for cancelling brackets
			while (!ws_isempty(opstack) && !is_left_bracket(ws_peek(opstack))) {
				ws_push(postfix, ws_pop(opstack));
			}

			// remove left bracket
			if (is_left_bracket(ws_peek(opstack))) {
				ws_pop(opstack);
			} else {
				warning("infix_to_postfix", "Surplus right bracket.");
			}
		} else if (is_separator_word(w) && (w->sep == EXPRSEP || w->sep == ARGSEP)) {
			// if found expression/argument separator, flush the operator stack
			while (!ws_isempty(opstack) && !is_left_bracket(ws_peek(opstack))) {
				ws_push(postfix, ws_pop(opstack));
			}
		} else if (is_operator_word(w)) {
			// while w has lower/equal precedence to top of opstack AND there is not a bracket
			// at the top of opstack, push to output
			while (!ws_isempty(opstack)
			        && !is_separator_word(ws_peek(opstack))
					&& (!higher_precedence(w, ws_peek(opstack)) || is_symbol_word(ws_peek(opstack)))) {
				ws_push(postfix, ws_pop(opstack));
			}
			
			ws_push(opstack, w);
		}
	}

	while (!ws_isempty(opstack)) {
		word* w = ws_pop(opstack);

		if (is_left_bracket(w)) {
			warning("infix_to_postfix", "Surplus left bracket.");
			continue;
		}

		ws_push(postfix, w);
	}

	ws_free(infix);

	return postfix;
}

// Evaluates a given postfix expression and returns the resulting evalstack
word* eval_postfix(word* base, environment* env) {
	word* rev = ws_copy(base);
	word* evalstack = make_base_word();

	// reverse given stack to read from its bottom
	ws_reverse(rev);

	while (!ws_isempty(rev)) {
		word* top = ws_pop(rev);
		ws_push(evalstack, top);

		if (is_symbol_word(top)) {
			// lookup symbol in current environment
			environment* result = resolve_symbol(top, env);

			// if symbol cannot be resolved
			if (result == NULL) {
				char msg[80];
				snprintf(msg, 80, "Unknown symbol %s. Evaluating to zero.", top->sym);
				warning("eval_postfix", msg);
				ws_pop(evalstack);
				ws_push(evalstack, make_number_word(0));
				continue;
			}

			// if symbol is resolved to a function, apply it to arguments currently on evalstack
			if (result->type == FUNCTION) {
				// stack size is argc+1 (including function name symbol)
				int given_argc = ws_height(evalstack) - 1;

				if (given_argc < result->argc) {
					char msg[80];
					snprintf(msg, 80, "Not enough arguments for function %s (expected %d, got %d)", top->sym, result->argc, given_argc);
					warning("eval_postfix", msg);
					continue;
				}

				ws_pop(evalstack); // pop off function name before calling
				apply_function(result, evalstack);
			} else if (result->type == VARIABLE) {
				// if symbol is resolved to a variable, substitute its numeric value
				// NOTE: copy before pushing to avoid problems with multiple instances of same symbol
				ws_pop(evalstack);
				ws_push(evalstack, ws_copy(result->val));
			}
		} else if (is_operator_word(top)) {
			apply_operator(evalstack);
		}
	}

	return evalstack;
}

// apply operator to two operands
void apply_operator(word* evalstack) {
	word* op;
	word* arg2;
	word* arg1;

	if (ws_isempty(evalstack)) {
		warning("apply_operator", "Empty stack given to apply_operator.");
		return;
	}

	op = ws_pop(evalstack);

	if (ws_height(evalstack) < 2) {
		char msg[80];
		sprintf(msg, "Not enough arguments for operator %s.", op->op);
		warning("apply_operator", msg);
		return;
	}

	arg2 = ws_pop(evalstack);
	arg1 = ws_pop(evalstack);

	word* result = make_number_word(0);

	if (strcmp(op->op, "+") == 0) {
		result->val = arg1->val + arg2->val;
	} else if (strcmp(op->op, "-") == 0) {
		result->val = arg1->val - arg2->val;
	} else if (strcmp(op->op, "*") == 0) {
		result->val = arg1->val * arg2->val;
	} else if (strcmp(op->op, "/") == 0) {
		result->val = arg1->val / arg2->val;
	} else if (strcmp(op->op, "^") == 0) {
		result->val = pow(arg1->val, arg2->val);
	} else {
		char msg[80];
		snprintf(msg, 80, "Don't know how to apply operator %s.", op->op);
		warning("apply_operator", msg);
		// don't know how to evaluate operator
	}

	ws_push(evalstack, result);
}

void apply_function(environment* func, word* argstack) {
	func->f(argstack);
}

int is_left_bracket(word* w) {
	return w->type == SEPARATOR && w->sep == LBRACKET;
}

int is_right_bracket(word* w) {
	return w->type == SEPARATOR && w->sep == RBRACKET;
}
