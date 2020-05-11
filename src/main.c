#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <editline/readline.h>

#include "parser.h"
#include "lexer.h"
#include "error.h"
#include "environment.h"

char* read_file_to_string(const char* filepath) {
	FILE *f = fopen(filepath, "r");
	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *string = malloc(fsize + 1);
	fread(string, 1, fsize, f);
	fclose(f);

	string[fsize] = '\0';
	return string;
}

void eval_input(char* input) {
	if (strcmp(input, "quit") == 0 || strcmp(input, "q") == 0) {
		exit(0);
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
}

int main(int argc, char** argv) {
	char* input;

	init_global_env();

	// open file specified in argv[1]
	if (argc == 2) {
		char* program = read_file_to_string(argv[1]);
		eval_input(program);
		return 0;
	}

	while (1) {
		input = readline("REPL>> ");
		add_history(input);
		eval_input(input);
		free(input);
	}

	return 0;
}

