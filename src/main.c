#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <editline/readline.h>

#include "parser.h"
#include "lexer.h"
#include "commands.h"
#include "environment.h"

int main(int argc, char** argv) {
	char* input;

	init_global_env();

	// if a string was given as argv[1], parse it
	if (argc == 2) {
		word* words = get_words_from_string(argv[1]);
		word* postfix = infix_to_postfix(words, global_env);
		eval_postfix(postfix, global_env);
		ws_free(words);
		ws_free(postfix);
		return 0;
	}

	printf("Type \'help\' for usage instructions.\n");

	while (1) {
		input = readline(">> ");
		add_history(input);
		run_command(input);
		free(input);
	}

	return 0;
}

