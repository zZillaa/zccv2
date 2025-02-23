#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MACRO_COUNT 100
#define INCLUDE_PATHS 100
#define INITIAL_BUFFER_SIZE 4096

struct IncludeNode {
	char* file_path;
	size_t placeholder_id;
	int start_pos;
	int end_pos;
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

	int current_pos;

	IncludeList* includes;
	MacroList* macros;
} Preprocessor;


// macro functionality
bool is_at_end(Preprocessor* preprocessor);
bool is_at_character(Preprocessor* preprocessor, char c);

char* get_identifier(Preprocessor* preprocessor);
int get_number(Preprocessor* preprocessor);
void parse_define(Preprocessor* preprocessor);
void parse_include(Preprocessor* preprocessor, int start_pos);
void add_include(MacroList* list, char* name, int value);

char advance(Preprocessor* preprocessor);
char peek(Preprocessor* preprocessor);
char peek_next(Preprocessor* preprocessor);
void skip_whitespace(Preprocessor* preprocessor);


bool macro_exists(MacroList* macros, char* name);
void add_macro(IncludeList* list, char* file_path, int start_pos);
int find_macro_replacement(MacroList* macros, const char* name);
void replace_macros(Preprocessor* preprocessor);
void add_macro(MacroList* macros, const char* name, const char* value);

// writer code from #include directive to file
long get_file_size(FILE* file);
char* get_file_contents(char* source);

void init_macrolist(Preprocessor* preprocessor);
void init_includelist(Preprocessor* preprocessor);
Preprocessor* init_preprocessor(char* source);
Preprocessor* preprocess(char* source);

void free_preprocessor(Preprocessor* preprocessor);

#endif