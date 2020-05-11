#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "word.h"
#include "function.h"

// Linked list of key-val mappings
typedef struct environment_struct {
	struct environment_struct* next;
	enum {START, VARIABLE, FUNCTION} type;
	word* key;
	union {
		word* val;     // if variable
		function* fun; // if function
	};
} environment;

extern environment* global_env;

// Initialize global_env with default values
void init_global_env();

// Basic operations
environment* make_empty_env();
void link_env(environment* child, environment* parent);

// Functions for resolving/defining/undefining symbols
environment* resolve_symbol(word* key, environment* env);
void define_variable(word* key, word* val, environment* env);
void define_function(word* key, function* f, environment* env);
void undef_symbol(word* key, environment* env);

// Prints all mappings in env
void print_env(environment* env);

#endif
