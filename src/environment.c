#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "environment.h"
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

		while (cursor->next != NULL) {
			cursor = cursor->next;
		}

		// Append new pair to env
		cursor->next = new_pair;
	}
}

void define_function(word* key, void (*f)(word* argstack), int argc, environment* env) {
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

void sin_wrapper(word* argstack) {
	word* arg = ws_pop(argstack);
	word* result = make_number_word(sin(arg->val));
	ws_push(argstack, result);
}

void cos_wrapper(word* argstack) {
	word* arg = ws_pop(argstack);
	word* result = make_number_word(cos(arg->val));
	ws_push(argstack, result);
}

void tan_wrapper(word* argstack) {
	word* arg = ws_pop(argstack);
	word* result = make_number_word(tan(arg->val));
	ws_push(argstack, result);
}

void log_wrapper(word* argstack) {
	word* log_base = ws_pop(argstack);
	word* log_arg = ws_pop(argstack);
	word* result = make_number_word(log(log_arg->val) / log(log_base->val));
	ws_push(argstack, result);
}

