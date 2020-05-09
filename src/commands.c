#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>

#include "config.h"
#include "commands.h"
#include "word.h"
#include "lexer.h"
#include "parser.h"

void quit();
void help();
void version();
void parser();
void lexer();
void env();
void debug();

void run_command(char* command) {
	if (strcmp(command, "quit") == 0 || strcmp(command, "q") == 0) {
		quit();
	} else if (strcmp(command, "help") == 0) {
		help();
	} else if (strcmp(command, "version") == 0) {
		version();
	} else if (strcmp(command, "parser") == 0) {
		parser();
	} else if (strcmp(command, "lexer") == 0) {
		lexer();
	} else if (strcmp(command, "env") == 0) {
		env();
	} else if (strcmp(command, "debug") == 0) {
		debug();
	} else {
		printf("Unknown command \'%s\'. Type \'help\' for a list of valid commands.\n", command);
	}
}

void quit() {
	exit(0);
}

void help() {
	printf("Available commands:\n");
	printf("  quit    - return to shell.\n");
	printf("  help    - show this message.\n");
	printf("  version - show version number.\n");
	printf("  parser  - enter mathematical expression parser.\n");
	printf("  lexer   - enter lexer debugging mode.\n");
	printf("  env     - show current environment.\n");
}

void version() {
	printf("Version %s\n", VERSION);
}

void parser() {
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

void lexer() {
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

void env() {
	printf("Current environment:\n");
	print_env(global_env);
}

void debug() {
	char* input;

	while (1) {
		input = readline("debug>> ");
		add_history(input);

		if (strcmp(input, "quit") == 0 || strcmp(input, "q") == 0) {
			break;
		}

		word* words = get_words_from_string(input);
		word* tmp = infix_to_postfix(words, global_env);
		printf("Input:\n");
		print_words(words);
		printf("In RPN:\n");
		print_words(tmp);
		printf("Evaluated to:\n");
		print_words(eval_postfix(tmp, global_env));

		ws_free(words);
		ws_free(tmp);
		free(input);
	}
}

