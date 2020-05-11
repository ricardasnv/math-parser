#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "parser.h"
#include "word.h"
#include "operators.h"
#include "error.h"
#include "builtins.h"

// shorthands
int is_left_bracket(word* w);
int is_right_bracket(word* w);
int is_left_expr_bracket(word* w);
int is_right_expr_bracket(word* w);

// Reverse wordstack and dispatch to revinfix_to_postfix
// NOTE: does not modify passed wordstack
word* infix_to_postfix(word* base, environment* env) {
	word* base_copy = ws_copy(base);
	ws_reverse(base_copy);
	return revinfix_to_postfix(base_copy, env);
}

// Convert infix to postfix using the shunting yard algorithm
// NOTE: modifies passed wordstack
word* revinfix_to_postfix(word* infix, environment* env) {
	word* postfix = make_base_word();
	word* opstack = make_base_word();

	while (!ws_isempty(infix)) {
		word* w = ws_pop(infix);

		if (is_left_expr_bracket(w)) {
			word* subexpr = make_base_word();
			int bracket_depth = 1;
			
			while (bracket_depth > 0) {
				word* curr = ws_pop(infix);

				if (is_left_expr_bracket(curr)) {
					bracket_depth++;
				} else if (is_right_expr_bracket(curr)) {
					bracket_depth--;
				}

				ws_push(subexpr, curr);
			}

			// pop off the leftover right bracket
			ws_pop(subexpr);

			ws_push(postfix, make_expr_word(subexpr));
		} else if (is_number_word(w)) {
			ws_push(postfix, w);
		} else if (is_symbol_word(w)) {
			// if built-in, treat as function
			if (is_built_in(w->sym)) {
				ws_push(opstack, w);
				continue;
			}

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
				warning("revinfix_to_postfix", "Surplus right bracket.");
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
			warning("revinfix_to_postfix", "Surplus left bracket.");
			continue;
		}

		ws_push(postfix, w);
	}

	return postfix;
}

word* extract_expressions(word* input) {
	word* infix = ws_copy(input);
	word* exprlist = make_base_word();
	word* expr = make_base_word();

	ws_reverse(infix);

	int expr_depth = 0;

	while (!ws_isempty(infix)) {
		word* w = ws_pop(infix);

		if (is_left_expr_bracket(w)) {
			expr_depth++;
		} else if (is_right_expr_bracket(w)) {
			expr_depth--;
		}

		if (expr_depth == 0 && is_separator_word(w) && w->sep == EXPRSEP) {
			ws_push(exprlist, make_expr_word(expr));
			expr = make_base_word();
			continue;
		}

		ws_push(expr, w);
	}

	return exprlist;
}

// Evaluates a given postfix expression and returns the resulting evalstack
word* eval_postfix(word* base, environment* env) {
	word* rev = ws_copy(base);
	word* evalstack = make_base_word();

	// reverse given stack so that the first word is on top
	ws_reverse(rev);

	while (!ws_isempty(rev)) {
		word* top = ws_pop(rev);
		ws_push(evalstack, top);

		if (is_symbol_word(top)) {
			// if builtin, dispatch to corresponding function
			if (is_built_in(top->sym)) {
				ws_pop(evalstack);
				run_builtin_func(top->sym, evalstack, env);
				continue;
			}

			// lookup symbol in current environment
			environment* result = resolve_symbol(top, env);

			// if symbol cannot be resolved, then evaluate to itself
			if (result == NULL) {
				// leave the symbol on the evalstack
				continue;
			}

			// if variable, keep resolving the value until the result is a number
			while (result != NULL && result->type == VARIABLE) {
				// pop off the symbol name and push its resolved value
				ws_pop(evalstack);
				ws_push(evalstack, ws_copy(result->val));

				// if resolved value is another symbol, resolve it again
				// if it is a number, break out of the loop and move on to the next word
				// otherwise raise a warning and break out of the loop
				if (is_symbol_word(result->val)) {
					result = resolve_symbol(result->val, env);
				} else if (is_number_word(result->val)) {
					break;
				} else {
					char msg[80];
					snprintf(msg, 80, "Failed to resolve symbol %s.", result->key->sym);
					warning("eval_postfix", msg);
					break;
				}
			}

			// if symbol is resolved to a function, apply it to arguments currently on evalstack
			if (result->type == FUNCTION) {
				apply_function(result->fun, evalstack, env);
			}
		} else if (is_operator_word(top)) {
			apply_operator(evalstack, env);
		}
	}

	return evalstack;
}

void apply_function(function* f, word* stack, environment* env) {
	// Pop the function name off the stack
	word* function_symbol = ws_pop(stack);

	// Add the function to its own local env (to allow recursion)
	environment* local_env_initial = make_empty_env();
	link_env(local_env_initial, env);

	// Evaluate the arguments on the stack
	word* args = eval_postfix(stack, local_env_initial);

	int argc_provided = ws_height(args);
	int argc_expected = ws_height(f->formal_args);

	if (argc_provided < argc_expected) {
		char msg[80];
		snprintf(msg, 80, "Not enough arguments to function %s. (expected %d, provided %d)", function_symbol->sym, argc_expected, argc_provided);
		warning("apply_function", msg);
		return;
	}

	// Make a copy of formal_args to avoid altering the function's definition
	word* formal_args_copy = ws_copy(f->formal_args);

	// Bind actual arguments to the formal arguments and finish building local env
	environment* local_env = make_empty_env();
	define_function(function_symbol, f, local_env);
	for (int i = 0; i < argc_expected; i++) {
		word* current_formal_arg = ws_pop(formal_args_copy);
		word* current_actual_arg = ws_pop(stack);
		define_variable(current_formal_arg, current_actual_arg, local_env);
	}

	// Link the function's local environment to the parent environment
	link_env(local_env, local_env_initial);

	// Break function up into expressions and evaluate every one in order
	word* exprs = extract_expressions(f->body);
	ws_reverse(exprs); // evaluate earlier expressions first
	word* result = make_base_word(); // keep track of evaluation results
	while (!ws_isempty(exprs)) {
		word* expr = ws_pop(exprs)->expr;
		word* postfix = infix_to_postfix(expr, local_env);
		ws_push(result, ws_peek(eval_postfix(postfix, local_env)));
	}

	// Return result of last evaluated expression
	ws_push(stack, ws_peek(result));
}

word* eval_symbol_var(word* sym, environment* env) {
	environment* result = resolve_symbol(sym, env);

	if (result == NULL) {
		char msg[80];
		snprintf(msg, 80, "Failed to evaluate symbol %s.", sym->sym);
		warning("eval_symbol_var", msg);
		return ws_copy(sym);
	}

	if (result->type != VARIABLE) {
		char msg[80];
		snprintf(msg, 80, "Symbol %s is not a variable.", sym->sym);
		warning("eval_symbol_var", msg);
	}

	return ws_copy(result->val);
}

// apply operator to two operands
void apply_operator(word* stack, environment* env) {
	word* op;
	word* arg2;
	word* arg1;

	if (ws_isempty(stack)) {
		warning("apply_operator", "Empty stack given to apply_operator.");
		return;
	}

	op = ws_pop(stack);

	if (ws_height(stack) < 2) {
		char msg[80];
		snprintf(msg, 80, "Not enough arguments for operator %s.", op->op);
		warning("apply_operator", msg);
		return;
	}

	arg2 = ws_pop(stack);
	arg1 = ws_pop(stack);

	// Attempt to evaluate arguments if any symbols were passed
	if (arg1->type == SYMBOL) {
		arg1 = eval_symbol_var(arg1, env);

		if (arg1 == NULL) {
			char msg[80];
			snprintf(msg, 80, "First argument to operator %s couldn't be resolved.", op->op);
			warning("apply_operator", msg);
			return;
		}
	}

	if (arg2->type == SYMBOL) {
		arg2 = eval_symbol_var(arg2, env);

		if (arg2 == NULL) {
			char msg[80];
			snprintf(msg, 80, "Second argument to operator %s couldn't be resolved.", op->op);
			warning("apply_operator", msg);
			return;
		}
	}

	// If either argument is still not a number after resolving symbols
	if (arg1->type != NUMBER || arg2->type != NUMBER) {
		char msg[80];
		snprintf(msg, 80, "Non-number given to operator %s.", op->op);
		warning("apply_operator", msg);
	}

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

	ws_push(stack, result);
}

int is_left_bracket(word* w) {
	return w->type == SEPARATOR && w->sep == LBRACKET;
}

int is_right_bracket(word* w) {
	return w->type == SEPARATOR && w->sep == RBRACKET;
}

int is_left_expr_bracket(word* w) {
	return w->type == SEPARATOR && w->sep == LEXPRBR;
}

int is_right_expr_bracket(word* w) {
	return w->type == SEPARATOR && w->sep == REXPRBR;
}

