#include "lexer.h"
#define NUM_KEYWORDS 9

const char* keywords[] = {"int", "char", "boolean", "if", "else", "for", "while", "return", "void", "struct",
                    "break", "continue"};
keyword_t get_keyword_type(char* str) {
    for (int i = 0; i < NUM_KEYWORDS; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return (keyword_t)i;
        }
    }
    return KEYWORD_UNKNOWN;
}

TokenType keyword_to_token(Keyword* word) {
    switch (word->type) {
        case KEYWORD_INT: return TOKEN_INT;
        case KEYWORD_CHAR: return TOKEN_CHAR;
        case KEYWORD_BOOL: return TOKEN_BOOLEAN;
        case KEYWORD_IF: return TOKEN_IF;
        case KEYWORD_ELSE: return TOKEN_ELSE;
        case KEYWORD_FOR: return TOKEN_FOR;
        case KEYWORD_WHILE: return TOKEN_WHILE;
        case KEYWORD_RETURN: return TOKEN_RETURN;
        case KEYWORD_VOID: return TOKEN_VOID;
        default:
            fprintf(stderr, "Error: Unknown word\n");
            return TOKEN_UNKNOWN;
    }
}

Token create_token(TokenType type, int line, int column) {
    Token token = {
        .type = type,
        .line = line,
        .column = column
    };
    return token;
}

Token create_char_token(TokenType type, char value, int line, int column) {
    Token token = create_token(type, line, column);
    token.value.character = value;
    return token;
}

Token create_int_token(TokenType type, int value, int line, int column) {
    Token token = create_token(type, line, column);
    token.value.integer_value = value;
    return token;
}

Token create_string_token(TokenType type, char* value, int line, int column) {
    Token token = create_token(type, line, column);
    token.value.string = strdup(value);
    return token;
}

Lexer* init_lexer(char* source) {
    Lexer* lexer = malloc(sizeof(Lexer));
    if (!lexer) return NULL;

    lexer->start = source;
    lexer->end = source;
    lexer->line =  1;
    lexer->column = 1;
    lexer->tokenIdx = 0;
    lexer->capacity = 128;

    lexer->tokens = malloc(sizeof(Token) *  lexer->capacity);
    if (!lexer->tokens) {
        free(lexer);
        return NULL;
    }
    return lexer;
}

bool add_token(Lexer* lexer, Token token) {
    if (lexer->tokenIdx >= lexer->capacity) {
        lexer->capacity *= 2;
        lexer->tokens = realloc(lexer->tokens, sizeof(Token) * lexer->capacity);
        if (!lexer->tokens) return false;
    }

    lexer->tokens[lexer->tokenIdx++] = token;
    return true;
}

bool lexer_at_character(Lexer* lexer, char c) {
    return *lexer->end == c;
}

char advance_lexer(Lexer* lexer) {
    if (lexer_at_end(lexer)) return '\0';
    lexer->column++;
    return *lexer->end++;
}

char peek_lexer(Lexer* lexer) {
    if (lexer_at_end(lexer)) return '\0';
    return *lexer->end;
} 

char peek_lexer_next(Lexer* lexer) {
    if (lexer_at_end(lexer) || peek_lexer_next(lexer) == '\0') return '\0';
    return *(lexer->end + 1);
}

bool match(Lexer* lexer, char expected) {
    if (peek_lexer(lexer) != expected) return false;
    advance_lexer(lexer);
    return true;
}

void identifier(Lexer* lexer) {
    if (isalpha(peek_lexer(lexer))) {
        while (isalnum(peek_lexer(lexer)) || peek_lexer(lexer) == '_') {
            advance_lexer(lexer);
        }
    }

    int length = lexer->end - lexer->start;
    char* text = strndup(lexer->start, length);

    keyword_t keyword_type = get_keyword_type(text);
    if (keyword_type != KEYWORD_UNKNOWN) {
        Keyword word = { .type = keyword_type, .name = text};
        TokenType type = keyword_to_token(&word);
        add_token(lexer, create_string_token(type, text, lexer->line, lexer->column - length));
    } else {
        add_token(lexer, create_string_token(TOKEN_ID, text, lexer->line, lexer->column - length));
    }

    free(text);
}   

void number(Lexer* lexer) {
    bool isNegative = false;
    if (*lexer->start == '-') {
        isNegative = true;
        advance_lexer(lexer);
    }

    while (isdigit(peek_lexer(lexer))) {
        advance_lexer(lexer);
    }

    int length = lexer->end - lexer->start;
    char* num_str = strndup(lexer->start, length);
    int value = atoi(num_str);
    if (isNegative) value = -value;

    add_token(lexer, create_int_token(TOKEN_INT_LITERAL, value, lexer->line, lexer->column - length));
    free(num_str);
}

void operator(Lexer* lexer) {
    char c = advance_lexer(lexer);
    TokenType type = TOKEN_UNKNOWN;
    bool isCompound = false;

    switch (c) {
        case '+':
            if (match(lexer, '=')) {
                type = TOKEN_ADD_AND_ASSIGN;
                isCompound = true;
            } else if (match(lexer, '+')) {
                type = TOKEN_INCREMENT;
                isCompound = true;
            } else {
                type = TOKEN_ADD;
            }
            break;

        case '-':
            if (match(lexer, '=')) {
                type = TOKEN_SUBTRACT_AND_ASSIGN;
                isCompound = true;
            } else if (match(lexer, '-')) {
                type = TOKEN_DECREMENT;
                isCompound = true;
            } else {
                type = TOKEN_SUBTRACT;
            }
            break;

        case '*':
            if (match(lexer, '=')) {
                type = TOKEN_MULTIPLY_AND_ASSIGN;
                isCompound = true;
            } else {
                type = TOKEN_MULTIPLY;
            }
            break;

        case '/':
            if (match(lexer, '=')) {
                type = TOKEN_DIVIDE_AND_ASSIGN;
                isCompound = true;
            } else {
                type = TOKEN_DIVIDE;
            }
            break;

        case '<':
            if (match(lexer, '=')) {
                type = TOKEN_LESS_EQUAL;
                isCompound = true;
            } else {
                type = TOKEN_LESS;
            }
            break;

        case '>':
            if (match(lexer, '=')) {
                type = TOKEN_GREATER_EQUAL;
                isCompound = true;
            } else {
                type = TOKEN_GREATER;
            }
            break;

        case '=':
            if (match(lexer, '=')) {
                type = TOKEN_EQUAL;
                isCompound = true;
            } else {
                type = TOKEN_ASSIGNMENT;
            }
            break;

        case '!':
            if (match(lexer, '=')) {
                type = TOKEN_NOT_EQUAL;
                isCompound = true;
            } else {
                type = TOKEN_NOT;
            }
            break;
        }

    if (isCompound) {
        char op_str[3] = {c, lexer->end[-1], '\0'};
        add_token(lexer, create_string_token(type, op_str, lexer->line, lexer->column - 2));
    } else {
        add_token(lexer, create_char_token(type, c, lexer->line, lexer->column - 1));
    }
}

void marker(Lexer* lexer) {
    char c = advance_lexer(lexer);
    TokenType type = TOKEN_UNKNOWN;

    switch (c) {
        case '(': type = TOKEN_LEFT_PARENTHESES; break;
        case ')': type = TOKEN_RIGHT_PARENTHESES; break;
        case '[': type = TOKEN_LEFT_BRACKET; break;
        case ']': type = TOKEN_RIGHT_BRACKET; break;
        case '{': type = TOKEN_LEFT_BRACE; break;
        case '}': type = TOKEN_RIGHT_BRACE; break;
        case ';': type = TOKEN_SEMICOLON; break;
        case ',': type = TOKEN_COMMA; break;
        case '_': type = TOKEN_UNDERSCORE; break;
        case '#': type = TOKEN_POUND; break;
        case '*': type = TOKEN_ASTERISK; break;
        case '"': type = TOKEN_DOUBLE_QUOTE; break;
        case '\'': type = TOKEN_SINGLE_QUOTE; break;
    }

    add_token(lexer, create_char_token(type, c, lexer->line, lexer->column - 1));
}

bool lexer_at_end(Lexer* lexer) {
    return *lexer->end == '\0';
}

bool skip_lexer_whitespace(Lexer* lexer) {
    while (!lexer_at_end(lexer) && isspace(peek_lexer(lexer))) {
        if (*lexer->end == '\n') {
            lexer->line++;
            lexer->column = 1;
        }
        advance_lexer(lexer);
    }
}

Token* lexical_analysis(char* source) {
    Lexer* lexer = init_lexer(source);
    if (!lexer) return NULL;

    while (!lexer_at_end(lexer)) {
        skip_lexer_whitespace(lexer);

        if (lexer_at_end(lexer)) break;

        lexer->start = lexer->end;

        if (isalpha(peek_lexer(lexer))) {
            identifier(lexer);
        } else if (isdigit(peek_lexer(lexer)) || (peek_lexer(lexer) == '-' && isdigit(peek_lexer_next(lexer)))) {
            number(lexer);
        } else if (strchr("+-*/<>=!", peek_lexer(lexer))) {
            operator(lexer);
        } else if (strchr("#*_;(){}[],", peek_lexer(lexer))) {
            marker(lexer);
        } else {
            advance_lexer(lexer);
        }
    }

    add_token(lexer, create_token(TOKEN_EOF, lexer->line, lexer->column));
    Token* tokens = lexer->tokens;
    free(lexer);
    return tokens;
}

void free_token(Token* token) {
    if (token->type == TOKEN_ID || token->type == TOKEN_KEYWORD) {
        free(token->value.string);
    }
}

void free_tokens(Token* tokens) {
    for (int i = 0; tokens[i].type != TOKEN_EOF; i++) {
        free_token(&tokens[i]);
    }
    free(tokens);
}

void print_tokens(Token* tokens) {
    for (int i = 0; tokens[i].type != TOKEN_EOF; i++) {
        // printf("Token type: ");
        switch (tokens[i].type) { 
            case TOKEN_ADD_AND_ASSIGN:
                printf("ADD AND ASSIGN, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_SUBTRACT_AND_ASSIGN:
                printf("SUBTRACT AND ASSSIGN, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_MULTIPLY_AND_ASSIGN:
                printf("MULTIPLY AND ASSIGN, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_DIVIDE_AND_ASSIGN:
                printf("DIVIDE AND ASSIGN, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_INCREMENT:
                printf("INCREMENT, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_DECREMENT:
                printf("DECREMENT, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_EQUAL:
                printf("EQUAL, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_GREATER_EQUAL:
                printf("GREATER THAN OR EQUAL, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_LESS_EQUAL:
                printf("LESS THAN OR EQUAL, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_NOT_EQUAL:
                printf("NOT EQUAL, Value: %s\n", tokens[i].value.string);
                break; 
            case TOKEN_IF:
                printf("IF, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_ELSE:
                printf("ELSE, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_WHILE:
                printf("WHILE, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_FOR:
                printf("FOR, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_VOID:
                printf("VOID, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_INT:
                printf("INT, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_BOOLEAN:
                printf("BOOLEAN, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_CHAR:
                printf("CHAR, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_INT_LITERAL:
                printf("INT LITERAL, Value: %d\n", tokens[i].value.integer_value);
                break;
            case TOKEN_STRUCT:
                printf("STRUCT, Value: %s\n", tokens[i].value.string);
            case TOKEN_ID:
                printf("IDENTIFIER, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_KEYWORD:
                printf("KEYWORD, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_RETURN:
                printf("RETURN, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_BREAK:
                printf("BREAK, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_CONTINUE:
                printf("CONTINUE, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_DOUBLE_QUOTE:
                printf("DOUBLE QUOTE, Value: %c", tokens[i].value.character);
                break;
            case TOKEN_SINGLE_QUOTE:
                printf("SINGLE QUOTE, Value: %c", tokens[i].value.character);
                break;
            case TOKEN_UNKNOWN:
                printf("UNKNOWN\n");
                break;
            case TOKEN_ASTERISK:
            case TOKEN_POUND:
            case TOKEN_ADD:
            case TOKEN_SUBTRACT:
            case TOKEN_MULTIPLY:
            case TOKEN_DIVIDE:
            case TOKEN_COMMA:
            case TOKEN_SEMICOLON:
            case TOKEN_LEFT_BRACE:
            case TOKEN_RIGHT_BRACE:
            case TOKEN_LEFT_PARENTHESES:
            case TOKEN_RIGHT_PARENTHESES:
            case TOKEN_LEFT_BRACKET:
            case TOKEN_RIGHT_BRACKET:
            case TOKEN_ASSIGNMENT:
            case TOKEN_UNDERSCORE:
            case TOKEN_LESS:
            case TOKEN_GREATER:
                printf("CHARACTER, Value: %c\n", tokens[i].value.character);
                break;

        }
    }
}
