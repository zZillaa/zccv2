#include "preprocessor.h"

Preprocessor* init_preprocessor(char* source) {
	Preprocessor* preprocessor = malloc(sizeof(Preprocessor));
	if (!preprocessor) {
		fprintf(stderr, "Error: Failed to allocate space for preprocessor\n");
		return NULL;
	}

	preprocessor->include_path_count = 0;
	preprocessor->macro_count = 0;
	preprocessor->line = 1;
	preprocessor->column = 1;
	preprocessor->start = source;
	preprocessor->end = source;
	preprocessor->output = NULL;
	
	preprocessor->macros = malloc(sizeof(Macro) * MACRO_COUNT);
	if (!preprocessor->macros) {
		fprintf(stderr, "Error: failed to allocate space for macros\n");
		free(preprocessor);
		return NULL;
	}

	preprocessor->includes = malloc(sizeof(IncludeNode) * INCLUDE_PATHS);
	if (!preprocessor->includes) {
		free(preprocessor->macros);
		free(preprocessor);
		return NULL;
	}

	return preprocessor;
}

char advance(Preprocessor* preprocessor) {
	preprocessor->column++;
	return *preprocessor->end;
}

char peek(Preprocessor* preprocessor) {
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

void add_include_node(IncludeList* list, char* file_path, size_t original_pos) {
	IncludeNode* node = malloc(sizeof(IncludeNode));
	if (!node) return;

	node->original_pos = original_pos;
	node->next = NULL;
	node->file_path = strdup(file_path);
	if (!node->file_path) {
		free(node);
		return;
	}

	if (list->tail) {
		list->tail->next = node;
		node->prev = list->tail;
		list->tail = node;
	} else {
		node->prev = NULL;
		list->head = node;
		list->tail = node;

	}
	list->count++;

}

void parse_include_directive(Preprocessor* preprocessor) {
	while (*preprocessor->end != '\n') {
		char c = peek(preprocessor);

		if (isspace(c)) {
			advance(preprocessor);
			continue;
		}

		preprocessor->start = preprocessor->end;

		if (!match(preprocessor, '<') || !match(preprocessor, '"')) return;

		switch (c) {
			case '<':
				while (*preprocessor->end != '>') {
					advance(preprocessor);
				}
				break;

			case '"':
				while (*preprocessor->end != '"') {
					advance(preprocessor);
				}
				break;
		}

	}
	// as preprocessor->start + 1 because preprocessor->start included < or " in file path
	int length = preprocessor->end - (preprocessor->start + 1);
	const char* file_path = strndup(preprocessor->start, length);
	add_include_node(preprocessor->includes, file_path, start_pos )
	free((void*)file_path);

}

void parse_define_directive(Preprocessor* preprocessor) {
	while (isspace(peek(preprocessor))) {
		advance(preprocessor);
	}

	preprocessor->start = preprocessor->end;
	if (!isalpha(peek(preprocessor))) return;

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

	preprocessor->macros[preprocessor->macro_count].name = strdup(name);
	if (c == '-' || isdigit(c)) {
		int value = atoi(replacement_text);
		preprocessor->macros[preprocessor->macros->macro_count].u.value = value;
	} else {
		preprocessor->macros[preprocessor->macros->macro_count].u.replacement = strdup(replacement_text);
	}

	preprocessor->macros->macro_count++;

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
	preprocessor->macros[preprocessor->macro_count].u.value = value;
	preprocessor->macro_count++;

	free(num_str);
	
}
void identifier(Preprocessor* preprocessor, size_t start_pos) {
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
		parse_include_directive(preprocessor, start_pos);
	}

	free(text);
	
}

void marker(Preprocessor* preprocessor) {
	char c = advance(preprocessor);
	size_t start_pos = 0;

	switch (c) {
		case '#':
		case '<': 
		case '"':
			start_pos = preprocessor->end;
			identifier(preprocessor, start_pos);
			break; 
	}
}

long get_file_size(FILE* file) {
    fpos_t posIndicator;
    if (fgetpos(file, &posIndicator) != 0) {
        fprintf(stderr, "fgetpos() failed\n");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    
    if (fsetpos(file, &posIndicator) != 0) {
        fprintf(stderr, "fsetpos() failed\n");
        exit(EXIT_FAILURE);
    }

    return file_size;
}

char* get_file_contents(char* file_path) {
    FILE* file = fopen(file_path, "rb");
    if (!file) {
    	fprintf(stderr, "Error: Failed to open file\n");
        exit(EXIT_FAILURE);
    }

    long file_size = get_file_size(file);
    char* contents = malloc(file_size + 1);
    char* writeIt = contents;

    size_t bytes_read = 0;
    while (bytes_read < file_size) {
        size_t curr_bytes_read = fread(writeIt, 1, file_size - bytes_read, file);
        bytes_read += curr_bytes_read;
        writeIt += curr_bytes_read;

        if (feof(file)) { break; }
    }

    if (bytes_read < file_size) {
        printf("Bytes read: %zu\nFile size: %li\n", bytes_read, file_size);
        free(contents);
        exit(EXIT_FAILURE);
    }
    contents[bytes_read] ='\0';
    fclose(file);

    return contents;

}

void write_to_source(char* source, char* contents) {
	
}

void generator(Preprocessor* preprocessor, char* source) {
	for (int i = 0; i < preprocessor->include_path_count; i++) {

	}
}

Preprocessor* preprocess(char* source) {
	Preprocessor* preprocessor = init_preprocessor(source);
	
	// first pass to get directives and their corresponding strings
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

	// second pass to include code from #include directives
	generator(preprocessor, source);

	return preprocessor;
}

void free_macro(Macro macro) {
	free(macro.name);
	free(macro.u.replacement);
}

void free_preprocessor(Preprocessor* preprocessor) {
	if (!preprocessor) return;

	for (int i = 0; i < preprocessor-> macro_count; i++) {
		free_macro(preprocessor->macros[i]);
	}
	
	for (int i = 0; i < preprocessor->include_path_count; i++) {
		free(preprocessor->include_paths[i]);
	}
	free(preprocessor->include_paths);
	free(preprocessor);
}