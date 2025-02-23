#include "preprocessor.h"

void init_macrolist(Preprocessor* preprocessor) {
	preprocessor->macros = malloc(sizeof(Macro) * MACRO_COUNT);
	if (!preprocessor->macros) return;

	preprocessor->macros->macro = malloc(sizeof(Macro) * MACRO_COUNT);
	if (!preprocessor->macros->macro) {
		free(preprocessor->macros);
		return;
	}

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
<<<<<<< HEAD
	preprocessor->output = strdup(source);
=======
>>>>>>> 8e0126cb350316c8a6962c814ab27d1140b47e05

	init_macrolist(preprocessor);
	init_includelist(preprocessor);

	return preprocessor;
}

bool is_at_end(Preprocessor* preprocessor) {
    return *preprocessor->end == '\0';
}

char advance(Preprocessor* preprocessor) {
    if (is_at_end(preprocessor)) return '\0';
	preprocessor->column++;
    preprocessor->current_pos++;
	return *preprocessor->end++;
}

char peek(Preprocessor* preprocessor) {
    if (is_at_end(preprocessor)) return '\0';
	return *preprocessor->end;
}

char peek_next(Preprocessor* preprocessor) {
	if (is_at_end(preprocessor) || *(preprocessor->end + 1) == '\0') return '\0';
	return *(preprocessor->end + 1);
}

<<<<<<< HEAD
bool match(Preprocessor* preprocessor, char expected) {
	if (peek(preprocessor) != expected) return false;
	advance(preprocessor);
	return true;
}

void add_include(IncludeList* list, char* file_path, int start_pos) {
=======
void add_include_node(IncludeList* list, char* file_path, size_t start_pos, size_t end_pos) {
>>>>>>> 8e0126cb350316c8a6962c814ab27d1140b47e05
	struct IncludeNode* node = malloc(sizeof(struct IncludeNode));
	if (!node) return;

	node->start_pos = start_pos;
<<<<<<< HEAD
=======
	node->end_pos = end_pos;
>>>>>>> 8e0126cb350316c8a6962c814ab27d1140b47e05
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

bool is_at_character(Preprocessor* preprocessor, char c) {
    return *preprocessor->end == c;
}

void parse_include(Preprocessor* preprocessor, int start_pos) {
	while (*preprocessor->end != '\n') {
		char c = peek(preprocessor);

		skip_whitespace(preprocessor);

		preprocessor->start = preprocessor->end;

		switch (c) {
			case '<':
				while (!is_at_character(preprocessor, '<')) {
					advance(preprocessor);
				}
				break;

			case '"':
				while (!is_at_character(preprocessor, '"')) {
					advance(preprocessor);
				}
				break;

			default:
				return;
		}

	}
<<<<<<< HEAD
	
	int length = preprocessor->end - preprocessor->start - 1;
	char* file_path = strndup(preprocessor->start + 1, length);
	add_include(preprocessor->includes, file_path, start_pos);
=======

	size_t end_pos = preprocessor->end - 1;
	int length = preprocessor->end - preprocessor->start - 2;
	char* file_path = strndup(preprocessor->start + 1, length);
	add_include_node(preprocessor->includes, file_path, start_pos, end_pos);
>>>>>>> 8e0126cb350316c8a6962c814ab27d1140b47e05
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

void add_macro(MacroList* macros, char* name, int value) {
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
	}

	macros->macro[macros->macro_count++] = macro_node;
}

char* get_identifier(Preprocessor* preprocessor) {
    preprocessor->start = preprocessor->end;

    while (!is_at_end(preprocessor) && (isalnum(peek(preprocessor)) || peek(preprocessor) == '_')) {
        advance(preprocessor);
    }

    int length = preprocessor->end - preprocessor->start;
    return strndup(preprocessor->start, length);
}

void parse_define(Preprocessor* preprocessor) {
    skip_whitespace(preprocessor);

	char* name = get_identifier(preprocessor);
	if (!name) return;
    
    skip_whitespace(preprocessor);

	preprocessor->start = preprocessor->end;
    char c = peek(preprocessor);
    if (c != '-' || !isdigit(peek(preprocessor))) return;
    
    int value = get_number(preprocessor);
    if (!macro_exists(preprocessor->macros, name)) {
        add_macro(preprocessor->macros, name, value);
    }
		 
	free(name);
}

int get_number(Preprocessor* preprocessor) {
	bool isNegative = false;
	if (*preprocessor->start == '-') {
		isNegative = true;
		advance(preprocessor);
	}

	while (isdigit(peek(preprocessor))) {
		advance(preprocessor);
	}

    // not consuming '-' as we have bool flag
	int length = preprocessor->end - preprocessor->start + 1;
	char* num_str = strndup(preprocessor->start, length);
	int value = atoi(num_str);
	if (isNegative) value = -value;

	free(num_str);

<<<<<<< HEAD
    return value;
=======
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
	char c = advance(preprocessor);

	switch (c) {
		case '#':
		case '<': 
		case '"':
			identifier(preprocessor, start_pos);
			break; 
	}
>>>>>>> 8e0126cb350316c8a6962c814ab27d1140b47e05
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

<<<<<<< HEAD
void skip_whitespace(Preprocessor* preprocessor) {
    while (!is_at_end(preprocessor) && isspace(peek(preprocessor))) {
        if (peek(preprocessor) == '\n') {
            preprocessor->line++;
            preprocessor->column = 1;
        }
        advance(preprocessor);
    }
=======
void update_subsequent_positions(struct IncludeNode* node, char* curr_file_path) {
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

	preprocessor->file = fopen(original_file_path, "w+");
	if (!preprocessor->file) return;

	fseek(preprocessor->file, start_pos, SEEK_SET);
	fwrite(new_content, 1, new_size, preprocessor->file);

	free(include_contents);
	free(new_content);

>>>>>>> 8e0126cb350316c8a6962c814ab27d1140b47e05
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
<<<<<<< HEAD

	while (!is_at_end(preprocessor)) {
		skip_whitespace(preprocessor);

		if (is_at_end(preprocessor)) break;

        if (peek(preprocessor) == '#') {
            int include_pos = preprocessor->current_pos;
            advance(preprocessor); 
            
            char* directive = get_identifier(preprocessor);
            if (directive && strcmp(directive, "define") == 0) {
                parse_define(preprocessor);
            } else if (directive && strcmp(directive, "include") == 0) {
                parse_include(preprocessor, include_pos);
            }
            free(directive);
        } else {
            advance(preprocessor);
        }
	}
	replace_macros(preprocessor);

=======
	
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
	fclose(preprocessor->file);
>>>>>>> 8e0126cb350316c8a6962c814ab27d1140b47e05
	return preprocessor;

}

<<<<<<< HEAD
=======

void free_macro(Macro macro) {
	free(macro.name);
	free(macro.u.replacement);
}

>>>>>>> 8e0126cb350316c8a6962c814ab27d1140b47e05
void free_preprocessor(Preprocessor* preprocessor) {
	if (!preprocessor) return;

    if (preprocessor->macros) {
    	for (size_t i = 0; i < preprocessor->macros->macro_count; i++) {
    		free(preprocessor->macros->macro[i].name);
            free(preprocessor->macros->macro[i].u.replacement);
    	} 
        free(preprocessor->macros->macro);
        free(preprocessor->macros);
    }
	
	if (preprocessor->includes) {
        struct IncludeNode* current = preprocessor->includes->head;
    	while (current) {
    		struct IncludeNode* next = current->next;
    		free(current->file_path);
    		free(current);
    		current = next;
    	}
        free(preprocessor->includes);
    }
	free(preprocessor);
}

int find_macro_replacement(MacroList* macros, const char* name) {
    for (size_t i = 0; i < macros->macro_count; i++) {
        if (strcmp(macros->macro[i].name, name) == 0) {
            return macros->macro[i].u.value;
        }
    }
    return -1;
}

void replace_macros(Preprocessor* preprocessor) {
    char* input = strdup(preprocessor->output);
    if (!input) return;

    size_t output_size = strlen(input) * 2;
    char* output = malloc(output_size);
    if (!output) {
        free(input);
        return;
    }

    char* current = ouptut;
    char* input_ptr = input;

}