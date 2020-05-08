#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "operators.h"

typedef enum lexer_state_enum {
	SKIPPING,
	READING_NUMBER,
	READING_OPERATOR,
	READING_SYMBOL,
	READING_BRACKET
} lexer_state;

void update_lexer_state(char c, lexer_state* state);

char* char_to_str(char c);

int is_digit(char c);
int is_letter(char c);
int is_whitespace(char c);
int is_operator(char* str);
int is_part_of_operator(char c);
int is_bracket(char c);
int bracket_facing(char c);

// ==== Public definitions ====
word* get_words_from_string(char* passed_str) {
	word* word_stack = make_base_word();

	lexer_state state = SKIPPING;
	lexer_state prev_state = SKIPPING;

	char buffer[BUFFER_LEN];
	strcpy(buffer, "");

	char* str = malloc((strlen(passed_str) + 2) * sizeof(char));
	strcpy(str, passed_str);
	strcat(str, " ");

	int should_clear_buffer = 0;

	// Lexer cycle
	// Phase 1: update lexer state
	// Phase 2: react to current state or state change
	// Phase 3: load current character into buffer (if not in state SKIPPING)
	for (int i = 0; str[i] != '\0'; i++) {
		// ==== PHASE 1 ====
		prev_state = state;
		update_lexer_state(str[i], &state);

		// ==== PHASE 2 ====
		// if finished reading operator
		if (is_operator(buffer)) {
			ws_push(word_stack, make_operator_word(buffer));
			strcpy(buffer, "");
		}

		// if finished reading bracket
		if (prev_state == READING_BRACKET) {
			ws_push(word_stack, make_bracket_word(bracket_facing(buffer[0])));
			strcpy(buffer, "");
		}

		// if finished reading number
		if (prev_state == READING_NUMBER && state != READING_NUMBER) {
			ws_push(word_stack, make_number_word(strtod(buffer, NULL)));
			strcpy(buffer, "");
		}

		// if finished reading symbol
		if (prev_state == READING_SYMBOL && state != READING_SYMBOL) {
			ws_push(word_stack, make_symbol_word(buffer));
			strcpy(buffer, "");
		}

		// ==== PHASE 3 ====
		if (state != SKIPPING) {
			strncat(buffer, char_to_str(str[i]), BUFFER_LEN - strlen(buffer));
		}
	}

	return word_stack;
}

// ==== Private definitions ====
void update_lexer_state(char c, lexer_state* state) {
	if (*state != READING_SYMBOL && is_letter(c)) {
		*state = READING_SYMBOL;
	} else if (*state == READING_SYMBOL && (is_letter(c) || is_digit(c))) {
		*state = READING_SYMBOL;
	} else if (is_digit(c) || c == '.') {
		*state = READING_NUMBER;
	} else if (is_part_of_operator(c)) {
		*state = READING_OPERATOR;
	} else if (is_bracket(c)) {
		*state = READING_BRACKET;
	} else if (is_whitespace(c)) {
		*state = SKIPPING;
	} else {
		*state = SKIPPING;
	}
}

char* char_to_str(char c) {
	char* str = malloc(2 * sizeof(char));
	str[0] = c;
	str[1] = '\0';
	return str;
}

int is_digit(char c) {
	if (c >= 48 && c <= 57) return 1; // from 0 to 9
	return 0;
}

int is_letter(char c) {
	if (c >= 65 && c <= 90) return 1;  // uppercase
	if (c >= 97 && c <= 122) return 1; // lowercase
	if (c == 95) return 1;             // underscore
	return 0;
}

int is_whitespace(char c) {
	if (c == '\0' || c == ' ' || c == '\t' || c == '\n' || c == '\r') return 1;
	return 0;
}

int is_operator(char* str) {
	for (int i = 0; i < NUM_OF_OPERATORS; i++) {
		if (strcmp(str, VALID_OPERATORS[i].repr) == 0) {
			return 1;
		}
	}

	return 0;
}

int is_part_of_operator(char c) {
	for (int i = 0; i < strlen(VALID_OPERATOR_CHARS); i++) {
		if (c == VALID_OPERATOR_CHARS[i]) {
			return 1;
		}
	}

	return 0;
}

int is_bracket(char c) {
	if (c == '(' || c == ')') return 1;
	if (c == '[' || c == ']') return 1;
	if (c == '{' || c == '}') return 1;
	return 0;
}

int bracket_facing(char c) {
	if (c == '(' || c == '[' || c == '{') return LEFT;
	if (c == ')' || c == ']' || c == '}') return RIGHT;
	return -1; // not a bracket
}

