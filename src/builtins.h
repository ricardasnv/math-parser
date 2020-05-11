#ifndef BUILTINS_H
#define BUILTINS_H

#include "word.h"
#include "environment.h"

typedef struct {
	char* name;
	void (*f)(word*, environment*);
} builtin_struct;

#define NUM_OF_BUILTINS 6
extern builtin_struct BUILTINS[NUM_OF_BUILTINS];

// Returns 1 if name is in BUILTIN_NAMES
int is_built_in(char* name);

void run_builtin_func(char* name, word* stack, environment* env);

// Built-in functions
void do_show(word* stack, environment* env);
void do_env(word* stack, environment* env);
void do_defvar(word* stack, environment* env);
void do_deffun(word* stack, environment* env);
void do_undef(word* stack, environment* env);
void do_if(word* stack, environment* env);

#endif
