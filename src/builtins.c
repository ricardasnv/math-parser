#include <stdio.h>
#include <string.h>

#include "builtins.h"
#include "error.h"
#include "parser.h"

const char* BUILTIN_NAMES[NUM_OF_BUILTINS] = {
	"echo",
	"env",
	"defvar",
	"deffun",
	"undef",
	"if"
};

int is_built_in(char* name) {
	for (int i = 0; i < NUM_OF_BUILTINS; i++) {
		if (strcmp(name, BUILTIN_NAMES[i]) == 0) {
			return 1;
		}
	}

	return 0;
}

void do_echo(word* stack, environment* env) {
	if (ws_height(stack) < 1) {
		warning("do_echo", "Not enough arguments for echo. (expected 1)");
		return;
	}

	word* w = ws_pop(stack);
	print_word(w, "");
	// return nothing
	//ws_push(stack, w);
}

void do_env(word* stack, environment* env) {
	print_env(env);
}

void do_defvar(word* stack, environment* env) {
	int argc_provided = ws_height(stack);

	if (argc_provided < 2) {
		char msg[80];
		snprintf(msg, 80, "Not enough arguments for defvar. (expected 2, provided %d)", argc_provided);
		warning("do_defvar", msg);
		return;
	}

	word* value = ws_pop(stack);
	word* name = ws_pop(stack);

	// if first arg is an expression, convert it to a symbol
	if (is_expr_word(name)) {
		if (ws_height(name->expr) != 1) {
			char msg[80];
			snprintf(msg, 80, "Variable name must be single symbol. (%d words provided)", ws_height(name->expr));
			warning("do_defvar", msg);
			return;
		}

		name = ws_peek(name->expr);
	}

	if (!is_symbol_word(name)) {
		warning("do_defvar", "Variable name must be a symbol.");
		return;
	}

	if (is_built_in(name->sym)) {
		char msg[80];
		snprintf(msg, 80, "Cannot assign variable to built-in name %s.", name->sym);
		warning("do_defvar", msg);
		return;
	}

	define_variable(name, value, env);

	// return nothing
	//ws_push(stack, name);
}

void do_deffun(word* stack, environment* env) {
	int argc_provided = ws_height(stack);

	if (argc_provided < 3) {
		char msg[80];
		snprintf(msg, 80, "Not enough arguments for deffun. (expected 3, provided %d)", argc_provided);
		warning("do_deffun", msg);
		return;
	}

	word* fbody = ws_pop(stack);
	word* fargs = ws_pop(stack);
	word* fname = ws_pop(stack);

	if (!is_expr_word(fbody)) {
		warning("do_deffun", "Function body must be an expression.");
		return;
	}

	if (!is_expr_word(fargs)) {
		warning("do_deffun", "Function formal argument list must be an expression.");
		return;
	}

	fargs = fargs->expr;
	fbody = fbody->expr;

	// if name is an expression, convert it to a symbol
	if (is_expr_word(fname)) {
		if (ws_height(fname->expr) != 1) {
			char msg[80];
			snprintf(msg, 80, "Function name must be single symbol. (%d words provided)", ws_height(fname->expr));
			warning("do_deffun", msg);
			return;
		}

		fname = ws_peek(fname->expr);
	}

	if (!is_symbol_word(fname)) {
		warning("do_deffun", "Variable name must be a symbol.");
		return;
	}

	if (is_built_in(fname->sym)) {
		char msg[80];
		snprintf(msg, 80, "Cannot assign variable to built-in name %s.", fname->sym);
		warning("do_deffun", msg);
		return;
	}

	// Convert body from infix to postfix
	fbody = infix_to_postfix(fbody, env);

	function* f = make_function(fargs, fbody);
	define_function(fname, f, env);

	// return nothing
	//ws_push(stack, fname);
}

void do_undef(word* stack, environment* env) {
	if (ws_height(stack) < 1) {
		warning("do_undef", "Not enough arguments for undef. (expected 1)");
		return;
	}

	word* name = ws_pop(stack);

	// if name is an expression, convert it to a symbol
	if (is_expr_word(name)) {
		if (ws_height(name->expr) != 1) {
			char msg[80];
			snprintf(msg, 80, "Single symbol expected. (%d words provided)", ws_height(name->expr));
			warning("do_undef", msg);
			return;
		}

		name = ws_peek(name->expr);
	}

	if (!is_symbol_word(name)) {
		warning("do_undef", "Expected a symbol.");
		return;
	}

	undef_symbol(name, env);

	// return nothing
	//ws_push(stack, name);
}

void do_if(word* stack, environment* env) {
	int argc_provided = ws_height(stack);

	if (argc_provided < 3) {
		char msg[80];
		snprintf(msg, 80, "Not enough arguments for if. (expected 3, provided %d)", argc_provided);
		warning("do_if", msg);
		return;
	}

	word* alternative = ws_pop(stack);
	word* consequent = ws_pop(stack);
	word* predicate = ws_pop(stack);

	if (!is_expr_word(alternative) || !is_expr_word(consequent) || !is_expr_word(predicate)) {
		warning("do_if", "All 3 arguments must be expressions.");
		return;
	}

	// Convert all three to postfix notation
	alternative = infix_to_postfix(alternative->expr, env);
	consequent = infix_to_postfix(consequent->expr, env);
	predicate = infix_to_postfix(predicate->expr, env);

	// Evaluate the predicate
	word* predicate_val = ws_peek(eval_postfix(predicate, env));

	word* result;

	// If value of predicate is >0, evaluate consequent
	if (predicate_val->val > 0) {
		result = ws_peek(eval_postfix(consequent, env));
	} else {
		// otherwise evaluate alternative
		result = ws_peek(eval_postfix(alternative, env));
	}

	ws_push(stack, result);
}

