#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "environment.h"
#include "parser.h"
#include "error.h"

environment* global_env = NULL;

void init_global_env() {
	global_env = malloc(sizeof(environment));
	memset(global_env, 0, sizeof(environment));
	global_env->type = NONE;

	// Constants
	word* pi_key = make_symbol_word("pi");
	word* pi_val = make_number_word(3.14159265);
	define_variable(pi_key, pi_val, global_env);

	word* e_key = make_symbol_word("e");
	word* e_val = make_number_word(2.718281828);
	define_variable(e_key, e_val, global_env);

	// Functions
	word* define_key = make_symbol_word("define");
	define_function(define_key, define_wrapper, 2, global_env);

	word* sin_key = make_symbol_word("sin");
	define_function(sin_key, sin_wrapper, 1, global_env);

	word* cos_key = make_symbol_word("cos");
	define_function(cos_key, cos_wrapper, 1, global_env);

	word* tan_key = make_symbol_word("tan");
	define_function(tan_key, tan_wrapper, 1, global_env);

	word* log_key = make_symbol_word("log");
	define_function(log_key, log_wrapper, 2, global_env);
}

// Returns NULL if sym can't be found in env
environment* resolve_symbol(word* key, environment* env) {
	environment* cursor = env;

	while (cursor != NULL) {
		if ((cursor->type == VARIABLE || cursor->type == FUNCTION) && strcmp(cursor->key->sym, key->sym) == 0) {
			return cursor;
		}

		cursor = cursor->next;
	}

	return NULL;
}

void define_variable(word* key, word* val, environment* env) {
	environment* new_pair = malloc(sizeof(environment));
	memset(new_pair, 0, sizeof(environment));
	new_pair->type = VARIABLE;
	new_pair->key = key;
	new_pair->val = val;

	if (env == NULL) {
		warning("define_variable", "env is NULL!");
	} else {
		environment* cursor = env;

		// Walk the environment to the end
		while (cursor->next != NULL) {
			// If symbol is already assigned a value, rewrite it
			if (cursor->key != NULL && strcmp(key->sym, cursor->key->sym) == 0) {
				cursor->val->val = val->val;
				return;
			}

			cursor = cursor->next;
		}

		// Check last pair in environment
		if (cursor->key != NULL && strcmp(key->sym, cursor->key->sym) == 0) {
			cursor->val->val = val->val;
			return;
		}

		// Append new pair to env
		cursor->next = new_pair;
	}
}

void define_function(word* key, void (*f)(word* argstack, environment* env), int argc, environment* env) {
	environment* new_pair = malloc(sizeof(environment));
	new_pair->next = NULL;
	new_pair->type = FUNCTION;
	new_pair->key = key;
	new_pair->f = f;
	new_pair->argc = argc;

	if (env == NULL) {
		warning("define_function", "env is NULL!");
	} else {
		environment* cursor = env;

		while (cursor->next != NULL) {
			cursor = cursor->next;
		}

		// Append new pair to env
		cursor->next = new_pair;
	}
}

void print_env(environment* env) {
	environment* cursor = env;

	while (cursor != NULL) {
		switch (cursor->type) {
		case NONE:
			break;
		case VARIABLE:
			printf("variable %s: ", cursor->key->sym); 
			print_word(cursor->val);
			break;
		case FUNCTION:
			printf("function %s of %d arguments\n", cursor->key->sym, cursor->argc); 
			break;
		default:
			printf("unknown mapping\n");
			break;
		}

		cursor = cursor->next;
	}
}

void define_wrapper(word* argstack, environment* env) {
	word* value = ws_pop(argstack);
	word* target = ws_pop(argstack);

	if (!is_symbol_word(target)) {
		char msg[80];
		snprintf(msg, 80, "Cannot assign to non-symbol. Returning same value.", target->sym);
		warning("define_wrapper", msg);
	}

	if (!is_number_word(value)) {
		char msg[80];
		snprintf(msg, 80, "Attempting to assign non-number value to symbol %s.", target->sym);
		warning("define_wrapper", msg);
	}

	define_variable(target, value, global_env);
	ws_push(argstack, target);
}

void sin_wrapper(word* argstack, environment* env) {
	word* arg = ws_pop(argstack);

	if (is_symbol_word(arg)) {
		arg = eval_symbol_var(arg, env);

		if (arg == NULL) {
			ws_push(argstack, arg);
			return;
		}
	}

	if (!is_number_word(arg)) {
		warning("sin_wrapper", "Non-number passed as argument where number is expected.");
		ws_push(argstack, arg);
		return;
	}

	word* result = make_number_word(sin(arg->val));
	ws_push(argstack, result);
}

void cos_wrapper(word* argstack, environment* env) {
	word* arg = ws_pop(argstack);

	if (is_symbol_word(arg)) {
		arg = eval_symbol_var(arg, env);

		if (arg == NULL) {
			ws_push(argstack, arg);
			return;
		}
	}

	if (!is_number_word(arg)) {
		warning("cos_wrapper", "Non-number passed as argument where number is expected.");
		ws_push(argstack, arg);
		return;
	}

	word* result = make_number_word(cos(arg->val));
	ws_push(argstack, result);
}

void tan_wrapper(word* argstack, environment* env) {
	word* arg = ws_pop(argstack);

	if (is_symbol_word(arg)) {
		arg = eval_symbol_var(arg, env);

		if (arg == NULL) {
			ws_push(argstack, arg);
			return;
		}
	}

	if (!is_number_word(arg)) {
		warning("tan_wrapper", "Non-number passed as argument where number is expected.");
		ws_push(argstack, arg);
		return;
	}

	word* result = make_number_word(tan(arg->val));
	ws_push(argstack, result);
}

void log_wrapper(word* argstack, environment* env) {
	word* log_base = ws_pop(argstack);
	word* log_arg = ws_pop(argstack);

	if (is_symbol_word(log_base)) {
		log_base = eval_symbol_var(log_base, env);

		if (log_base == NULL) {
			ws_push(argstack, log_base);
			return;
		}
	}

	if (is_symbol_word(log_arg)) {
		log_arg = eval_symbol_var(log_arg, env);

		if (log_arg == NULL) {
			ws_push(argstack, log_arg);
			return;
		}
	}

	if (!is_number_word(log_arg) || !is_number_word(log_base)) {
		warning("log_wrapper", "Non-number passed as argument where number is expected.");
		ws_push(argstack, log_arg);
		ws_push(argstack, log_base);
		return;
	}

	word* result = make_number_word(log(log_arg->val) / log(log_base->val));
	ws_push(argstack, result);
}

