#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "parser.h"
#include "word.h"
#include "operators.h"
#include "error.h"

// Convert infix to RPN using the shunting yard algorithm
// NOTE: does not modify passed wordstack
word* infix_to_rpn(word* passed_base) {
	word* infix = ws_copy(passed_base);
	word* rpn = make_base_word();
	word* opstack = make_base_word();

	ws_reverse(infix);

	while (!ws_isempty(infix)) {
		word* w = ws_pop(infix);

		// determine word type
		if (is_number_word(w) || is_symbol_word(w)) {
			ws_push(rpn, w);
		} else if (is_operator_word(w)) {
			word* ot = ws_peek(opstack); // opstack top

			// if w has lower/equal precedence to top of opstack, push to output
			while (!ws_isempty(opstack) && !is_bracket_word(ot) && !higher_precedence(w, ot)) {
				ws_push(rpn, ws_pop(opstack));
			}
			
			ws_push(opstack, w);
		} else if (is_bracket_word(w)) {
			// special logic for cancelling brackets
			if (w->facing == RIGHT) {
				while (!ws_isempty(opstack) && !is_bracket_word(ws_peek(opstack))) {
					ws_push(rpn, ws_pop(opstack));
				}

				// remove left bracket
				if (is_bracket_word(ws_peek(opstack)) && ws_peek(opstack)->facing == LEFT) {
					ws_pop(opstack);
				} else {
					warning("infix_to_rpn", "Surplus right bracket.");
				}
			} else if (w->facing == LEFT) {
				ws_push(opstack, w);
			}
		}
	}

	while (!ws_isempty(opstack)) {
		word* w = ws_pop(opstack);

		if (is_bracket_word(w)) {
			warning("infix_to_rpn", "Surplus left bracket.");
			continue;
		}

		ws_push(rpn, w);
	}

	ws_free(infix);

	return rpn;
}

word* eval_rpn(word* base) {
	word* rev = ws_copy(base);
	word* evalstack = make_base_word();

	// reverse given stack to read from its bottom
	ws_reverse(rev);

	while (!ws_isempty(rev)) {
		word* top = ws_pop(rev);
		ws_push(evalstack, top);

		if (is_operator_word(top)) {
			apply_operator(evalstack);
		} else if (is_symbol_word(top)) {
			// TODO: lookup symbol in current environment
			//eval_symbol(evalstack);
			char msg[80];
			sprintf(msg, "Unknown symbol %s. Evaluating to zero.", top->sym);
			warning("eval_rpn", msg);

			ws_pop(evalstack);
			ws_push(evalstack, make_number_word(0));
		}
	}

	if (ws_isempty(evalstack)) {
		return make_base_word();
	}

	return ws_pop(evalstack);
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
		// don't know how to evaluate operator
	}

	ws_push(evalstack, result);
}

