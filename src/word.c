#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "word.h"

word* make_base_word() {
	word* w = malloc(sizeof(word));
	memset(w, 0, sizeof(word));
	w->type = BASE;
	return w;
}

word* make_number_word(double val) {
	word* w = malloc(sizeof(word));
	w->type = NUMBER;
	w->val = val;
	w->next = NULL;
	return w;
}

word* make_operator_word(char* op) {
	word* w = malloc(sizeof(word));
	w->type = OPERATOR;
	w->op = malloc(strlen(op) * sizeof(char));
	strcpy(w->op, op);
	w->next = NULL;
	return w;
}

word* make_symbol_word(char* sym) {
	word* w = malloc(sizeof(word));
	w->type = SYMBOL;
	w->sym = malloc(strlen(sym) * sizeof(char));
	strcpy(w->sym, sym);
	w->next = NULL;
	return w;
}

word* make_bracket_word(int facing) {
	word* w = malloc(sizeof(word));
	w->type = BRACKET;
	w->facing = facing;
	w->next = NULL;
	return w;
}

int is_base_word(word* w) {
	return w->type == BASE;
}

int is_number_word(word* w) {
	return w->type == NUMBER;
}

int is_operator_word(word* w) {
	return w->type == OPERATOR;
}

int is_symbol_word(word* w) {
	return w->type == SYMBOL;
}

int is_bracket_word(word* w) {
	return w->type == BRACKET;
}

int ws_isempty(word* base) {
	return base->type == BASE && base->next == NULL;
}

int ws_height(word* base) {
	word* cursor = base;
	int counter = 0;

	while (cursor->next != NULL) {
		counter++;
		cursor = cursor->next;
	}

	return counter;
}

void ws_push(word* base, word* new_word) {
	ws_peek(base)->next = new_word;
}

word* ws_peek(word* base) {
	word* cursor = base;

	while (cursor->next != NULL)
		cursor = cursor->next;

	return cursor;
}

word* ws_pop(word* base) {
	word* cursor = base;
	word* top;

	// walk to second last word
	while (cursor->next->next != NULL)
		cursor = cursor->next;

	top = cursor->next;
	cursor->next = NULL;

	return top;
}

word* ws_copy(word* src) {
	word* cursor = src;
	word* dest = malloc(sizeof(word));
	word* dest_base = dest;

	// not implemented yet
	while (cursor != NULL) {
		memcpy(dest, cursor, sizeof(word));
		
		// if not at top of source stack yet, allocate new memory for next element
		// otherwise let dest->next point to NULL
		if (cursor->next != NULL) {
			dest->next = malloc(sizeof(word));
		}

		dest = dest->next;
		cursor = cursor->next;
	}

	return dest_base;
}

void ws_free(word* base) {
	if (base->next == NULL) {
		free(base);
	} else {
		ws_free(base->next);
		free(base);
	}
}

void ws_reverse(word* base) {
	word* tmp_base = make_base_word();

	while (!ws_isempty(base)) {
		ws_push(tmp_base, ws_pop(base));
	}

	// Replace old base pointer with new one
	memcpy(base, tmp_base, sizeof(word));
	free(tmp_base);
}

void print_word(word* w) {
	char facing_str[6];
	
	printf("  (");

	switch (w->type) {
	case BASE:
		printf("word stack base");
		break;
	case NUMBER:
		printf("number, %f", w->val);
		break;
	case OPERATOR:
		printf("operator, %s", w->op);
		break;
	case SYMBOL:
		printf("symbol, %s", w->sym);
		break;
	case BRACKET:
		w->facing == LEFT ? strcpy(facing_str, "LEFT") : strcpy(facing_str, "RIGHT");
		printf("bracket, %s", facing_str);
		break;
	default:
		printf("unknown type");
		break;
	}

	printf(")\n");
}

void print_words(word* list) {
	word* cursor = list;

	while (cursor != NULL) {
		print_word(cursor);
		cursor = cursor->next;
	}
}

