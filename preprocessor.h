#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MACRO_COUNT 100
#define INCLUDE_PATHS 100

struct IncludeNode {
	char* file_path;
	size_t placeholder_id;
	size_t original_pos;
	size_t end_pos;
	size_t content_length;

	struct IncludeNode* prev;
	struct IncludeNode* next;
};

typedef struct {
	struct IncludeNode* head;
	struct IncludeNode* tail;
	size_t include_count;
} IncludeList;

typedef struct {
	char* name;
	union {
		char* replacement;
		int value;
	} u;

} Macro;

typedef struct {
	Macro* macro;
	size_t macro_count;
	size_t macro_capacity;
} MacroList;

typedef struct {
	int line;
	int column;

	char* start;
	char* end;
	char* output;

	IncludeList* includes;
	MacroList* macros;
} Preprocessor;


void add_include_node(IncludeList* list, char* file_path, size_t original_pos);
void add_int_macro_node(MacroList* list, char* name, int value);
void add_string_macro_node(MacroList* list, char* name, char* replacement);

// macro functionality
bool macro_exists(MacroList* macros, char* name);
void macro_bind(char* name, char* replacement);

void parse_define_directive(Preprocessor* preprocessor);
void parse_include_directive(Preprocessor* preprocesor, size_t start_pos);

char advance(Preprocessor* preprocessor);
char peek(Preprocessor* preprocessor);
char peek_next(Preprocessor* preprocessor);

void identifier(Preprocessor* preprocessor, size_t start_pos);
void marker(Preprocessor* preprocessor);
void number(Preprocessor* preprocessor);

// writer code from #include directive to file
long get_file_size(FILE* file);
char* get_file_contents(char* source);
void update_subsequent_positions(struct IncludeNode* node, size_t shift);
void write_to_source(char* source, char* contents, size_t pos);
void generator(Preprocessor* preprocessor, char* source);

void init_macrolist(Preprocessor* preprocessor);
void init_includelist(Preprocessor* preprocessor);
Preprocessor* init_preprocessor(char* source);
Preprocessor* preprocess(char* source);

void free_macro(Macro macro);
void free_preprocessor(Preprocessor* preprocessor);
#endif