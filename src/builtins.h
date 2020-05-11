#ifndef BUILTINS_H
#define BUILTINS_H

#include "word.h"
#include "environment.h"

#define NUM_OF_BUILTINS 6
extern const char* BUILTIN_NAMES[NUM_OF_BUILTINS];

// Returns 1 if name is in BUILTIN_NAMES
int is_built_in(char* name);

// Built-in functions
void do_echo(word* stack, environment* env);
void do_env(word* stack, environment* env);
void do_defvar(word* stack, environment* env);
void do_deffun(word* stack, environment* env);
void do_undef(word* stack, environment* env);
void do_if(word* stack, environment* env);

#endif
