#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>

#include "config.h"
#include "commands.h"
#include "word.h"
#include "lexer.h"
#include "parser.h"
#include "function.h"

void run_quit();
void run_help();
void run_version();
void run_parser();
void run_env();
void run_lexer();
void run_eval();
void run_conv();
void run_defvar();
void run_deffun();
void run_undef();

void run_command(char* command) {
	if (strcmp(command, "quit") == 0 || strcmp(command, "q") == 0) {
		run_quit();
	} else if (strcmp(command, "help") == 0) {
		run_help();
	} else if (strcmp(command, "version") == 0) {
		run_version();
	} else if (strcmp(command, "parser") == 0) {
		run_parser();
	} else if (strcmp(command, "env") == 0) {
		run_env();
	} else if (strcmp(command, "lexer") == 0) {
		run_lexer();
	} else if (strcmp(command, "eval") == 0) {
		run_eval();
	} else if (strcmp(command, "conv") == 0) {
		run_conv();
	} else if (strcmp(command, "defvar") == 0) {
		run_defvar();
	} else if (strcmp(command, "deffun") == 0) {
		run_deffun();
	} else if (strcmp(command, "undef") == 0) {
		run_undef();
	} else {
		printf("Unknown command \'%s\'. Type \'help\' for a list of valid commands.\n", command);
	}
}

void run_quit() {
	exit(0);
}

void run_help() {
	printf("Available commands:\n");
	printf("  quit    - return to shell.\n");
	printf("  help    - show this message.\n");
	printf("  version - show version number.\n");
	printf("  parser  - enter mathematical expression parser.\n");
	printf("  defvar  - define a variable.\n");
	printf("  deffun  - define a function.\n");
	printf("  undef   - delete a definition.\n");
	printf("  env     - show current environment.\n");
}

void run_version() {
	printf("Version %s\n", VERSION);
}

void run_parser() {
	char* input;

	printf("Type 'quit' to exit parser.\n");

	while (1) {
		input = readline("parser>> ");
		add_history(input);

		if (strcmp(input, "quit") == 0 || strcmp(input, "q") == 0) {
			break;
		}

		word* words = get_words_from_string(input);
		word* postfix = infix_to_postfix(words, global_env);

		printf("Contents of evaluator stack:\n");
		print_words(eval_postfix(postfix, global_env));

		ws_free(words);
		ws_free(postfix);
		free(input);
	}
}


void run_env() {
	printf("Current environment:\n");
	print_env(global_env);
}

void run_lexer() {
	char* input;

	printf("Type 'quit' to exit lexer debugging mode.\n");

	while (1) {
		input = readline("lexer>> ");
		add_history(input);

		if (strcmp(input, "quit") == 0 || strcmp(input, "q") == 0) {
			break;
		}

		word* words = get_words_from_string(input);
		print_words(words);

		ws_free(words);
		free(input);
	}
}

void run_eval() {
	char* input;

	printf("Type 'quit' to exit postfix evaluator debugging mode.\n");

	while (1) {
		input = readline("eval>> ");
		add_history(input);

		if (strcmp(input, "quit") == 0 || strcmp(input, "q") == 0) {
			break;
		}

		word* words = get_words_from_string(input);

		printf("Evaluator stack:\n");
		print_words(eval_postfix(words, global_env));

		ws_free(words);
		free(input);
	}
}

void run_conv() {
	char* input;

	printf("Type 'quit' to exit infix->postfix converter debugging mode.\n");

	while (1) {
		input = readline("conv>> ");
		add_history(input);

		if (strcmp(input, "quit") == 0 || strcmp(input, "q") == 0) {
			break;
		}

		word* words = get_words_from_string(input);
		print_words(infix_to_postfix(words, global_env));

		ws_free(words);
		free(input);
	}
}

void run_defvar() {
	char* input;

	input = readline("Variable name: ");
	word* name = ws_peek(get_words_from_string(input));
	input = readline("Value: ");
	word* val = ws_peek(get_words_from_string(input));

	define_variable(name, val, global_env);

	free(input);
}

void run_deffun() {
	char* input;

	input = readline("Function name: ");
	word* fname = ws_peek(get_words_from_string(input));
	input = readline("Formal argument list: ");
	word* farg = get_words_from_string(input);
	input = readline("Function body: ");
	word* fbody = infix_to_postfix(get_words_from_string(input), global_env);

	function* f = make_function(farg, fbody);

	define_function(fname, f, global_env);

	free(input);
}

void run_undef() {
	char* input;

	input = readline("Symbol to undefine: ");
	word* name = ws_peek(get_words_from_string(input));

	undef_symbol(name, global_env);

	free(input);
}

