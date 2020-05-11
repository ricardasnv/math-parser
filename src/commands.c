#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>

#include "commands.h"
#include "word.h"
#include "lexer.h"
#include "error.h"
#include "parser.h"
#include "function.h"

void run_quit();
void run_help();
void run_parser();
void run_env();
void run_lexer();
void run_eval();
void run_conv();
void run_debug();

void run_command(char* command) {
	if (strcmp(command, "quit") == 0 || strcmp(command, "q") == 0) {
		run_quit();
	} else if (strcmp(command, "help") == 0) {
		run_help();
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
	} else if (strcmp(command, "debug") == 0) {
		run_debug();
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
	printf("  parser  - enter mathematical expression parser.\n");
	printf("  env     - show current environment.\n");
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
		word* exprs = extract_expressions(words);

		ws_reverse(exprs);

		if (ws_isempty(exprs)) {
			warning("run_parser", "No expressions given. (did you forget a semicolon?)");
		}

		while (!ws_isempty(exprs)) {
			word* current_expr = ws_pop(exprs)->expr;
			word* postfix = infix_to_postfix(current_expr, global_env);
			word* evaluated = eval_postfix(postfix, global_env);
			
			if (!ws_isempty(evaluated)) {
				print_word(ws_peek(evaluated), "");
			}

			ws_free(current_expr);
			ws_free(postfix);
			ws_free(evaluated);
		}

		ws_free(words);
		ws_free(exprs);
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
		print_words(words, "  ");

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
		print_words(eval_postfix(words, global_env), "  ");

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
		print_words(infix_to_postfix(words, global_env), "  ");

		free(input);
	}
}

void run_debug() {
	char* input = readline("debug>> ");

	word* words = get_words_from_string(input);
	word* expressions = extract_expressions(words);

	print_words(expressions, "");
}

