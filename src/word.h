#ifndef WORD_H
#define WORD_H

typedef struct word_struct {
	struct word_struct* next;
	enum {BASE, NUMBER, OPERATOR, SYMBOL, SEPARATOR} type;
	union {
		double val; // if number
		char* op;   // if operator
		char* sym;  // if symbol
		enum {      // if separator
			LBRACKET, RBRACKET, // brackets
			ARGSEP, EXPRSEP     // argument/expression separator
		} sep;
	};
} word;

word* make_base_word();
word* make_number_word(double val);
word* make_operator_word(char* op);
word* make_symbol_word(char* sym);
word* make_separator_word(int sep);

int is_base_word(word* w);
int is_number_word(word* w);
int is_operator_word(word* w);
int is_symbol_word(word* w);
int is_separator_word(word* w);

// basic word stack operations
int ws_isempty(word* base); // returns 1 if src only has 1 word of type BASE
int ws_height(word* base); // returns height of stack (ignoring the BASE word)
void ws_push(word* base, word* new_word);
word* ws_peek(word* base);
word* ws_pop(word* base);

// complex word stack operations
word* ws_copy(word* src); // copy entire stack
void ws_free(word* base); // free entire stack
void ws_reverse(word* base);

// for debugging
void print_word(word* w);
void print_words(word* list);

#endif
