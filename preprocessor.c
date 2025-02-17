#include "preprocessor.h"


Preprocessor* init_preprocessor(char* source) {
	Preprocessor* preprocessor = malloc(sizeof(Preprocessor));
	if (!preprocessor) {
		fprintf(stderr, "Error: Failed to allocate space for preprocessor\n");
		return NULL;
	}

	preprocessor->include_paths = NULL;
	preprocessor->include_path_count = 0;
	preprocessor->macro_count = 0;
	preprocessor->line = 1;
	preprocessor->column = 1;
	preprocessor->start = source;
	preprocessor->end = source;#define MAX_NUMBERS 100

int function() {
	int x = 5;
	int y = 20;
	int z = MAX_NUMBERS + y;

	return z;
}
	preprocessor->output = NULL;
	
	preprocessor->macros = malloc(sizeof(MacroDefinition) * MACRO_COUNT);
	if (!preprocessor->macros) {
		fprintf(stderr, "Error: failed to allocate space for macros\n");
		free(preprocessor);
		return NULL;
	}

	return preprocessor;
}

char* advance(Preprocessor* preprocessor) {
	*preprocessor->column++;
	return *preprocessor->end;
}

char* peek(Preprocessor* preprocessor) {
	return *preprocessor->end;
}

char peek_next(Preprocessor* preprocessor) {
	if (*preprocessor->end == '\0') return '\0';
	return *(preprocessor->end + 1);
}

bool match(Preprocessor* preprocessor, char expected) {
	if (peek(preprocessor) != expected) return false;
	advance(preprocessor);
	return true;
}

void parse_include_directive(Preprocessor* preprocessor) {
	while (*preprocessor->end != '\n') {
		char c = peek(preprocessor);

		if (isspace(c)) {
			advance(preprocessor);
			continue;
		}

		preprocessor->start = preprocessor->end;

		if (!match(preprocessor, '<') || !match(preprocessor, ' " ')) return;

		switch (c) {
			case '<':
				while (*preprocessor->end != '>') {
					advance(preprocessor);
				}
				break;

			case ' " ':
				while (*preprocessor->end != ' " ') {
					advance(preprocessor);
				}
				break;
		}

	}
	// as preprocessor->start + 1 because preprocessor->start included < or " in file path
	int length = preprocessor->end - (preprocessor->start + 1);
	const char* file_path = strndup(preprocessor->start, length);
	preprocessor->include_paths[include_path_count++] = strdup(file_path);
	free(file_path);

}

void parse_define_directive(Preprocessor* preprocessor) {
	while (isspace(peek(preprocessor))) {
		advance(preprocessor);
	}

	preprocessor->start = preprocessor->end;
	if (!isalpha(peek(preprocesor))) return;

	while (isalnum(peek(preprocessor)) || peek(preprocessor) == '_') {
		advance(preprocessor);
	}

	int length = preprocessor->end - preprocessor->start;
	char* name = strndup(preprocessor->start, length);

	while (isspace(peek(preprocessor))) {
		advance(preprocessor);
	} 

	preprocessor->start = preprocessor->end;
	if (!isalnum(peek(preprocessor)) || peek(preprocessor) != '-') return;

	char c = peek(preprocessor);
	if (c == '-') {
		number(preprocessor);
	} else {
		while (*preprocessor->end != '\n' && *preprocessor->end != '\0') {
			advance(preprocessor);
		}
	}

	int replacement_length = preprocessor->end - preprocessor->start;
	char* replacement_text = strndup(preprocessor->start, length);

	preprocessor->macros[macro_count].name = strdup(name);
	if (c == '-' || isdigit(c)) {
		int value = atoi(replacement_text);
		preprocessor->macros[macro_count].u.value = value;
	} else {
		preprocessor->macros[macro_count].u.replacement = strdup(replacement_text);
	}

	preprocessor->macro_count++;

	free(name);
	free(replacement_text);
}

void number(Preprocessor* preprocessor) {
	bool isNegative = false;
	if (*preprocessor->start == '-') {
		isNegative = true;
		advance(preprocessor);
	}

	while (isdigit(peek(preprocessor))) {
		advance(preprocessor);
	}

	int length = preprocessor->end - preprocessor->start;
	char* num_str = strndup(preprocessor->start, length);
	int value = atoi(num_str);
	if (isNegative) value = -value;
	preprocessor->macros[macro_count].u.value = value;
	preprocessor->macro_count++;

	free(num_str);
	
}
void identifier(Preprocessor* preprocessor) {
	if (isalpha(peek(preprocessor))) {
		while (isalnum(peek(preprocessor)) || peek(preprocessor) == '_') {
			advance(preprocessor);
		}
	} 

	int length = preprocessor->end - preprocessor->start;
	char* text = strndup(preprocessor->start, length);
	
	if (strcmp(text, "#define") == 0) {
		parse_define_directive(preprocessor);
	} else if (strcmp(text, "#include") == 0) {
		parse_include_directive(preprocessor);
	}

	free(text);
	
}


void marker(Preprocessor* preprocessor) {
	char c = advance(preprocessor);

	switch (c) {
		case '#':
		case '<': 
		case '"':
			identifier(preprocessor);
			break; 
	}
}

Preprocessor* preprocess(char* source) {
	Preprocessor* preprocessor = init_preprocessor(source);
	
	while (*preprocessor->end != '\0') {
		char c = peek(preprocessor);

		if (isspace(c)) {
			if (c == '\n') {
				preprocessor->line++;
				preprocessor->column++;
			}
			advance(preprocessor);
			continue;
		}

		preprocessor->start = preprocessor->end;
	
		if (isalpha(c)) {
			identifier(preprocessor);
		} else if (isdigit(c) || (c == '_' && isdigit(peek_next(preprocessor)))) {
			number(preprocessor);
		} else if (strchr("#<>\"", c)) {
			marker(preprocessor);
		}
	}

	return preprocessor;
}

void free_macro(MacroDefinition* macro) {
	free(macro->name);
	free(macro->replacement);
	free(macro->parameters);
	free(macro);
}

void free_preprocessor(Preprocessor* preprocessor) {
	if (!preprocessor) return;

	for (int i = 0; i < preprocessor-> macro_count; i++) {
		free_macro(&preprocessor->macros);
	}
	free(preprocessor->include_paths);

	free(preprocessor);
}