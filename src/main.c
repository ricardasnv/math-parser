#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <editline/readline.h>

#include "parser.h"
#include "lexer.h"
#include "error.h"
#include "commands.h"
#include "environment.h"

int main(int argc, char** argv) {
	char* input;

	init_global_env();

	// if a string was given as argv[1], parse it
	if (argc == 2) {
		word* words = get_words_from_string(argv[1]);
		word* postfix = infix_to_postfix(words, global_env);
		printf("%f\n", ws_peek(eval_postfix(postfix, global_env))->val);
		ws_free(words);
		ws_free(postfix);
		return 0;
	}

	while (1) {
		input = readline("REPL>> ");
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

	return 0;
}

