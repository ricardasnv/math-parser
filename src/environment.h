#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "word.h"

// Linked list of key-val mappings
typedef struct environment_struct {
	struct environment_struct* next;
	enum {NONE, VARIABLE, FUNCTION} type;
	word* key;
	int argc; // if function, this holds the argument count
	union {
		word* val;                 // if variable
		void (*f)(word* argstack); // if function
	};
} environment;

extern environment* global_env;

void init_global_env();
environment* resolve_symbol(word* key, environment* env);
void define_variable(word* key, word* val, environment* env);
void define_function(word* key, void (*f)(word* argstack), int argc, environment* env);
void print_env(environment* env);

// Built-in functions
void sin_wrapper(word* argstack);
void cos_wrapper(word* argstack);
void tan_wrapper(word* argstack);
void log_wrapper(word* argstack);

#endif
