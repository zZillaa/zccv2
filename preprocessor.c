#include "preprocessor.h"

void init_macrolist(Preprocessor* preprocessor) {
	preprocessor->macros = malloc(sizeof(Macro) * MACRO_COUNT);
	if (!preprocessor->macros) return;

	preprocessor->macros->macro_count = 0;
	preprocessor->macros->macro_capacity = MACRO_COUNT;
}

void init_includelist(Preprocessor* preprocessor) {
	preprocessor->includes = malloc(sizeof(IncludeList));
	if (!preprocessor->includes) return;

	preprocessor->includes->include_count = 0;
	preprocessor->includes->head = NULL;
	preprocessor->includes->tail = NULL;
}

Preprocessor* init_preprocessor(char* source) {
	Preprocessor* preprocessor = malloc(sizeof(Preprocessor));
	if (!preprocessor) {
		fprintf(stderr, "Error: Failed to allocate space for preprocessor\n");
		return NULL;
	}
	
	preprocessor->line = 1;
	preprocessor->column = 1;
	preprocessor->output = NULL;
	preprocessor->start = source;
	preprocessor->end = source;

	init_macrolist(preprocessor);
	init_includelist(preprocessor);

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

void add_include_node(IncludeList* list, char* file_path, long start_pos, long end_pos) {
	struct IncludeNode* node = malloc(sizeof(struct IncludeNode));
	if (!node) return;

	node->start_pos = start_pos;
	node->end_pos = end_pos;
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
	list->include_count++;

}

void parse_include_directive(Preprocessor* preprocessor, size_t start_pos) {
	while (*preprocessor->end != '\n') {
		char c = peek(preprocessor);

		if (isspace(c)) {
			advance(preprocessor);
			continue;
		}

		preprocessor->start = preprocessor->end;

		switch (c) {
			case '<':
				while (*preprocessor->end != '>') {
					advance(preprocessor);
				}
				advance(preprocessor);
				break;

			case '"':
				while (*preprocessor->end != '"') {
					advance(preprocessor);
				}
				advance(preprocessor);
				break;

			default:
				return;
		}

	}

	size_t end_pos = preprocessor->end - 1;
	int length = preprocessor->end - preprocessor->start - 2;
	char* file_path = strndup(preprocessor->start + 1, length);
	add_include_node(preprocessor->includes, file_path, start_pos, end_pos);
	free((void*)file_path);

}

bool macro_exists(MacroList* macros, char* name) {
	for (size_t i = 0; i < macros->macro_count; i++) {
		if (strcmp(macros->macro[i].name, name) == 0) {
			return true;
		}
	}
	return false;
}

void add_string_macro_node(MacroList* macros, char* name, char* replacement) {
	Macro macro_node = {
		.name = strdup(name),
		.u.replacement = strdup(replacement),
	};

	if (macros->macro_count >= macros->macro_capacity) {
		size_t new_capacity = macros->macro_capacity * 2;
		Macro* new_macros = realloc(macros->macro, new_capacity * sizeof(Macro));
		if (!macros) return;
		macros->macro = new_macros;
		macros->macro_capacity = new_capacity;

		macros->macro[macros->macro_count++] = macro_node;
	}

	macros->macro[macros->macro_count++] = macro_node;
}

void add_int_macro_node(MacroList* macros, char* name, int value) {
	Macro macro_node = {
		.name = strdup(name),
		.u.value = value,
	};

	if (macros->macro_count >= macros->macro_capacity) {
		size_t new_capacity = macros->macro_capacity * 2;
		Macro* new_macros = realloc(macros->macro, new_capacity * sizeof(Macro));
		if (!macros) return;
		macros->macro = new_macros;
		macros->macro_capacity = new_capacity;

		macros->macro[macros->macro_count++] = macro_node;
	}

	macros->macro[macros->macro_count++] = macro_node;

}

void parse_define_directive(Preprocessor* preprocessor) {
	while (isspace(peek(preprocessor->file))) {
		advance(preprocessor->file);
	}

	preprocessor->start = preprocessor->end;
	if (!isalpha(peek(preprocessor->file))) return;

	while (isalnum(peek(preprocessor->file)) || peek(preprocessor->file) == '_') {
		advance(preprocessor->file);
	}

	int length = preprocessor->end - preprocessor->start;
	char* name = strndup(preprocessor->start, length);

	while (isspace(peek(preprocessor->file))) {
		advance(preprocessor->file);
	} 

	preprocessor->start = preprocessor->end;
	if (!isalnum(peek(preprocessor->file)) || peek(preprocessor->file) != '-') return;

	char c = peek(preprocessor->file);
	if (c == '-') {
		number(preprocessor->file);
	} else {
		while (*preprocessor->end != '\n' && *preprocessor->end != '\0') {
			advance(preprocessor->file);
		}
	}

	int replacement_length = preprocessor->end - preprocessor->start;
	char* replacement_text = strndup(preprocessor->start, length);

	if (c == '-' || isdigit(c)) {
		int value = atoi(replacement_text);
		if (!macro_exists(preprocessor->macros, name)) {
			add_int_macro_node(preprocessor->macros, name, value);
		}


	} else {
		if (!macro_exists(preprocessor->macros, name)) {
			add_string_macro_node(preprocessor->macros, name, replacement_text);
		}
	}

	free(name);
	free(replacement_text);
}

void number(Preprocessor* preprocessor) {
	bool isNegative = false;
	if (*preprocessor->start == '-') {
		isNegative = true;
		advance(preprocessor->file);
	}

	while (isdigit(peek(preprocessor->file))) {
		advance(preprocessor->file);
	}

	int length = preprocessor->end - preprocessor->start;
	char* num_str = strndup(preprocessor->start, length);
	int value = atoi(num_str);
	if (isNegative) value = -value;

	free(num_str);
	
}
void identifier(Preprocessor* preprocessor, size_t start_pos) {
	if (isalpha(peek(file))) {
		while (isalnum(peek(file)) || peek(file) == '_') {
			advance(file);
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
	size_t start_pos = preprocessor->end;
	char c = advance(file);

	switch (c) {
		case '#':
		case '<': 
		case '"':
			identifier(file, start_pos);
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

void update_subsequent_positions(IncludeNode* node, char* curr_file_path) {
	size_t include_length = strlen(curr_file_path);
	node->start_pos += include_length;
	node->end_pos += include_length;
}

void write_to_source(Preprocessor* preprocessor, char* original_file_path, char* source, char* includes_file_path, size_t start_pos, size_t end_pos) {
	char* include_contents = get_file_contents(includes_file_path);
	size_t include_length = strlen(include_contents);

	size_t source_length = strlen(source);
	size_t new_size = include_length + source_length;
	char* new_content = malloc(new_size + 1);

	memcpy(new_content, include_contents, include_length);

	memmove(
		new_content + include_length,
		source,
		source_length
	);

	new_content[new_size] = '\0';		

	preprocessor->file = fopen(, "w+");
	fseek(file, start_pos, SEEK_SET);
	fwrite(new_content, 1, new_size, file);

	free(include_contents);
	free(remaining_contents);
	free(new_content);

}

void generator(Preprocessor* preprocessor, char* original_file_path, char* source) {
	struct IncludeNode* current = preprocessor->includes->head;
	while (current) {
		write_to_source(preprocessor, original_file_path, source, current->file_path,
			current->start_pos, current->end_pos);

		update_subsequent_positions(current->next, current->file_path);
		current = current->next;
	}
}

Preprocessor* preprocess(char* original_file_path, char* source)  {
	Preprocessor* preprocessor = init_preprocessor(source);
	
	while (*preprocessor->end != '\0') {
		char c = peek(preprocessor);

		if (isspace(c)) {
			if (c == '\n') {
				preprocessor->line++;
				preprocessor->column = 1;
			}
			advance(preprocessor);

		}

		if (isdigit(c) || (c == '-' && isdigit(peek_next(preprocessor)))) {
			number(preprocessor);
		} else if (strchr("#<>\"",c)) {
			marker(preprocessor);
		}

	}

	generator(preprocessor, original_file_path, source);

	return preprocessor;

}


void free_macro(Macro macro) {
	free(macro.name);
	free(macro.u.replacement);
}

void free_preprocessor(Preprocessor* preprocessor) {
	if (!preprocessor) return;

	for (size_t i = 0; i < preprocessor->macros->macro_count; i++) {
		Macro temp = preprocessor->macros->macro[i];
		free_macro(temp);
	}
	
	struct IncludeNode* current = preprocessor->includes->head;
	while (current) {
		struct IncludeNode* next = current->next;
		free(current->file_path);
		free(current);
		current = next;
	}

	free(preprocessor->includes);
	free(preprocessor->macros);
	free(preprocessor);
}