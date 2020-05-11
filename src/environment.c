#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "environment.h"
#include "parser.h"
#include "error.h"

environment* global_env = NULL;

void init_global_env() {
	global_env = make_empty_env();

	// Constants
	word* pi_key = make_symbol_word("pi");
	word* pi_val = make_number_word(3.14159265);
	define_variable(pi_key, pi_val, global_env);

	word* e_key = make_symbol_word("e");
	word* e_val = make_number_word(2.718281828);
	define_variable(e_key, e_val, global_env);

	// Built-in functions
}

environment* make_empty_env() {
	environment* new_env = malloc(sizeof(environment));
	memset(new_env, 0, sizeof(environment));
	new_env->type = NONE;
	return new_env;
}

void link_env(environment* child, environment* parent) {
	environment* cursor = child;

	// Walk to end of child
	while (cursor->next != NULL) {
		cursor = cursor->next;
	}

	cursor->next = parent;
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

		// If symbol is already assigned a value, rewrite it
		while (cursor != NULL) {
			if (cursor->key != NULL && strcmp(key->sym, cursor->key->sym) == 0) {
				cursor->val->val = val->val;
				return;
			}

			cursor = cursor->next;
		}

		// Walk the environment a second time
		cursor = env;
		while (cursor->next != NULL) {
			cursor = cursor->next;
		}

		// Append new pair to env
		cursor->next = new_pair;
	}
}

void define_function(word* key, function* f, environment* env) {
	environment* new_pair = make_empty_env();
	new_pair->next = NULL;
	new_pair->type = FUNCTION;
	new_pair->key = key;
	new_pair->fun = f;

	if (env == NULL) {
		warning("define_function", "env is NULL!");
	} else {
		environment* cursor = env;

		// If symbol is already assigned a value, rewrite it
		while (cursor != NULL) {
			if (cursor->key != NULL && strcmp(key->sym, cursor->key->sym) == 0) {
				cursor->fun = f;
				return;
			}

			cursor = cursor->next;
		}

		// Walk the environment a second time
		cursor = env;
		while (cursor->next != NULL) {
			cursor = cursor->next;
		}

		// Append new pair to env
		cursor->next = new_pair;
	}
}

void undef_symbol(word* key, environment* env) {
	environment* cursor = env;
	environment* prev = NULL;

	// If only 1 mapping in env, return.
	// (first mapping is guaranteed to be of type NONE)
	if (cursor->next == NULL) {
		return;
	}

	// If more than 1 mapping
	while (cursor != NULL) {
		if (cursor->key != NULL && prev != NULL && strcmp(key->sym, cursor->key->sym) == 0) {
			prev->next = cursor->next;
		}

		prev = cursor;
		cursor = cursor->next;
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
			print_word(cursor->val, "");
			break;
		case FUNCTION:
			printf("function %s of %d arguments\n", cursor->key->sym, ws_height(cursor->fun->formal_args)); 
			break;
		default:
			printf("unknown mapping\n");
			break;
		}

		cursor = cursor->next;
	}
}

