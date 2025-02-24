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
    preprocessor->current_pos = 1;
    preprocessor->start = source;
    preprocessor->end = source;
    preprocessor->output = strdup(source);

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

bool match(Preprocessor* preprocessor, char expected) {
    if (peek(preprocessor) != expected) return false;
    advance(preprocessor);
    return true;
}

void add_include(IncludeList* list, char* file_path, int start_pos) {
    struct IncludeNode* node = malloc(sizeof(struct IncludeNode));
    if (!node) return;

    node->start_pos = start_pos;
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
    
    int length = preprocessor->end - preprocessor->start - 1;
    char* file_path = strndup(preprocessor->start + 1, length);
    add_include(preprocessor->includes, file_path, start_pos);
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

void add_macro(MacroList* macros, char* name, char* replacement) {
    Macro macro_node = {
        .name = strdup(name),
        .replacement = strdup(replacement)
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

char* get_filepath(Preprocessor* preprocessor) {
    preprocessor->start = preprocessor->end;

    while (!is_at_end(preprocessor) && isalnum(peek(preprocessor)) || peek(preprocessor) == '_' || peek(preprocessor) == '.') {
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
    
    char* text = get_identifier(preprocessor);
    if (!macro_exists(preprocessor->macros, name)) {
        add_macro(preprocessor->macros, name, text);
    }

    free(text);
    free(name);
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

void skip_whitespace(Preprocessor* preprocessor) {
    while (!is_at_end(preprocessor) && isspace(peek(preprocessor))) {
        if (peek(preprocessor) == '\n') {
            preprocessor->line++;
            preprocessor->column = 1;
        }
        advance(preprocessor);
    }
}

Preprocessor* preprocess(char* source) {
    Preprocessor* preprocessor = init_preprocessor(source);

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
    replace_includes(preprocessor);

    return preprocessor;
}

char* find_macro_replacement(MacroList* macros, const char* name) {
    for (size_t i = 0; i < macros->macro_count; i++) {
        if (strcmp(macros->macro[i].name, name) == 0) {
            return macros->macro[i].replacement;
        }
    }
    return NULL;
}

MacroReplace* init_macro_replace(size_t input_length) {
    MacroReplace* replace = malloc(sizeof(MacroReplace));
    if (!replace) return NULL;

    replace->contents = malloc(input_length * 2);
    if (!replace->contents) {
        free(replace);
        return NULL;
    }

    return replace;
}

void replace_macros(Preprocessor* preprocessor) {
    if (!preprocessor) return;

    char* input = strdup(preprocessor->output);
    size_t input_length = strlen(input);

    MacroReplace* replace = init_macro_replace(input_length);
    char* write_pos = replace->contents;

    preprocessor->start = preprocessor->output;
    preprocessor->end = preprocessor->output;

    while (!is_at_end(preprocessor)) {
        if (peek(preprocessor) == '#') {
            *write_pos = peek(preprocessor);
            write_pos++;
            advance(preprocessor);
            
            char* directive = get_identifier(preprocessor);
            size_t directive_length = strlen(directive);

            if (strcmp(directive, "define") == 0 || strcmp(directive, "include") == 0) {
                strncpy(write_pos, directive, directive_length);
                write_pos += directive_length;
                while (!is_at_end(preprocessor) && !is_at_character(preprocessor, '\n')) {
                    *write_pos = peek(preprocessor);
                    write_pos++;
                    advance(preprocessor);
                }
            }
            free(directive);
        } else if (isalpha(peek(preprocessor))) {
            char* text = get_identifier(preprocessor);
            char* replacement = find_macro_replacement(preprocessor->macros, text);

            if (replacement) {
                size_t replacement_length = strlen(replacement);
                strncpy(write_pos, replacement, replacement_length);
                write_pos += replacement_length;
            } else {
                size_t text_length = strlen(text);
                strncpy(write_pos, text, text_length);
                write_pos += text_length;
            }
            free(text);
        } else {
            *write_pos = peek(preprocessor);
            write_pos++;
            advance(preprocessor);
        }
    }

    *write_pos = '\0';
    preprocessor->output = strdup(replace->contents);
}

void replace_includes(Preprocessor* preprocessor) {
    if (!preprocessor) return;

    char* input = strdup(preprocessor->output);
    if (!input) return;
    size_t input_length = strlen(input);

    char* write_pos = malloc(input_length * 4);
    if (!write_pos) {
        free(input);
        return;
    }
    char* write_head = write_pos;
    
    preprocessor->start = preprocessor->output;
    preprocessor->end = preprocessor->output;

    while (!is_at_end(preprocessor)) {
        if (peek(preprocessor) == '#') {
            advance(preprocessor);

            char* directive = get_identifier(preprocessor);
            size_t directive_length = strlen(directive);

            if (strcmp(directive, "include") == 0) {
                skip_whitespace(preprocessor);
                advance(preprocessor);
                
                char* file_path = get_filepath(preprocessor);
                char* file_contents = get_file_contents(file_path);
                size_t file_length = strlen(file_contents);
                size_t used_length = write_pos - write_head;
                size_t allocated_length = input_length * 4;

                if (file_length + used_length >= allocated_length) {
                    size_t new_length = 4 * (file_length + allocated_length + 1);
                    char* new_write_pos = realloc(write_head, new_length);
                    printf("Here i am\n");
                    if (!new_write_pos) {
                        free(file_contents);
                        free(file_path);
                        free(write_head);
                        free(input);
                        return;
                    }
                    write_pos = new_write_pos + used_length;
                    write_head = new_write_pos;
                    input_length = new_length / 4;
                }               
                strncpy(write_pos, file_contents, file_length);
                write_pos += file_length;
                free(file_contents);
                free(file_path);
            } 
            free(directive);
        } else {
            *write_pos = peek(preprocessor);
            write_pos++;
            advance(preprocessor);
        }
    }

    *write_pos = '\0';
    preprocessor->output = strdup(write_head);
    free(input);
    free(write_head);
}

void free_preprocessor(Preprocessor* preprocessor) {
    if (!preprocessor) return;

    if (preprocessor->macros) {
        for (size_t i = 0; i < preprocessor->macros->macro_count; i++) {
            free(preprocessor->macros->macro[i].replacement);
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
    free(preprocessor->output);
    free(preprocessor);
}