#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MACRO_COUNT 100
#define INCLUDE_PATHS 100

typedef struct {
	int macro_count;
	char* name;
	union {
		char* replacement;
		int value;
	} u;
} Macro;

typedef struct {
	char* file_path;
	size_t placeholder_id;
	size_t original_pos;
	size_t end_pos;
	size_t content_length;

	struct IncludeNode* prev;
	struct IncludeNode* next;
} IncludeNode;

typedef struct {
	IncludeNode* head;
	IncludeNode* tail;
	size_t count;
} IncludeList;

typedef struct {
	int line;
	int column;

	char* start;
	char* end;
	char* output;

	IncludeList* includes;
	Macro* macros;
} Preprocessor;


void add_include_node(IncludeList* list, char* file_path, size_t original_pos);

// macro functionality
char* macro_lookup(char* name);
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
void write_to_source(char* source, char* contents);
void generator(Preprocessor* preprocessor, char* source);

Preprocessor* init_preprocessor(char* source);
Preprocessor* preprocess(char* source);

void free_macro(Macro macro);
void free_preprocessor(Preprocessor* preprocessor);
#endif