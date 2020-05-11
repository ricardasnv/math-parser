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
}

environment* make_empty_env() {
	environment* new_env = malloc(sizeof(environment));
	memset(new_env, 0, sizeof(environment));
	new_env->type = START;
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

		// Walk the environment
		while (cursor->next != NULL) {
			// Do not enter parent env
			if (cursor->next->type == START) {
				environment* parent = cursor->next;
				cursor->next = new_pair;
				new_pair->next = parent;
				return;
			}

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

	// If symbol already has a definition, overwrite it
	environment* resolved = resolve_symbol(key, env);
	if (resolved != NULL) {
		resolved->fun = f;
		return;
	}

	// Add symbol definition to current env
	if (env == NULL) {
		warning("define_function", "env is NULL!");
	} else {
		environment* cursor = env;

		// Walk the environment
		while (cursor->next != NULL) {
			// Do not enter parent env
			if (cursor->next->type == START) {
				environment* parent = cursor->next;
				cursor->next = new_pair;
				new_pair->next = parent;
				return;
			}

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
	// (first mapping is guaranteed to be of type START)
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
		case START:
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

