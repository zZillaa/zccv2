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
	preprocessor->end = source;
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

void identifier(Preprocessor* preprocessor) {
	if (isalpha(peek(preprocessor))) {
		while (isalnum(peek(preprocessor)) || peek(preprocessor) == '_') {
			advance(preprocessor);
		}
	} 

	int length = preprocessor->end - preprocessor->start;
	char* text = strndup(preprocessor->start, length);
	preprocessor->macros[macro_count++].name = strdup(text);

	free(text);
	

}

void number(Preprocessor* preprocessor) {
	bool isNegative = false;
	if (*lexer->start == '-') {
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
	preprocessor->macros[macro_count++].u.value = value;

	free(num_str);
	
}

void directive(Preprocessor* preprocessor) {
	if (isalpha(peek(preprocessor))) {
		while (isalpha(peek(preprocessor))) {
			advance(preprocessor);
		}
	}

	int length = preprocessor->end - preprocessor->start;
	char* text = strndup(preprocessor->start, length);

	if (strcmp(text, "#define") == 0) {
		identifier(preprocessor);
	} else if (strcmp(text, "#include") == 0) {

	}

	free(text);
	
}

void marker(Preprocessor* preprocessor) {
	char c = advance(preprocessor);

	switch (c) {
		case '#':
			directive(preprocessor);
			break;

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