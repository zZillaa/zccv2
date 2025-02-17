#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MACRO_COUNT 100

typedef enum {
	PREPROCESSED_INCLUDE,
	PREPROCESSED_DEFINE,
} preprocessed_t;

typedef struct {
	char* name;
	union {
		char* replacement;
		int value;
	} u;
} Macro;

typedef struct {
	Macro* macros;
	char* include_paths;
	int include_path_count;
	int macro_count;
	
	int line;
	int column;

	char* start;
	char* end;
	char* output;
} Preprocessor;


// macro functionality
char* macro_lookup(char* name);
void macro_bind(char* name, char* replacement);

char* advance(Preprocessor* preprocessor);
char* peek(Preprocessor* preprocessor);
char* peek_next(Preprocessor* preprocessor);

void identifier(Preprocessor* preprocessor);
void marker(Preprocessor* preprocessor);
int number(Preprocessor* preprocessor);
void directive(Preprocessor* preprocessor);
Preprocessor* init_preprocessor(char* source);
Preprocessor* preprocess(char* source);

void free_macro(MacroDefinition* macro);
void free_preprocessr(Preprocessor* preprocessor);
#endif