#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include "lexer.h"
#define NUM_KEYWORDS 9
// #include "parser.h"

char* keywords[] = {"int", "char", "boolean", "if", "else", "for", "while", "return", "void"};
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
        case KEYWORD_INT:
            return TOKEN_INT;

        case KEYWORD_CHAR:
            return TOKEN_CHAR;

        case KEYWORD_BOOL:
            return TOKEN_BOOLEAN;

        case KEYWORD_IF:
            return TOKEN_IF;

        case KEYWORD_ELSE:
            return TOKEN_ELSE;

        case KEYWORD_FOR:
            return TOKEN_FOR;

        case KEYWORD_WHILE:
            return TOKEN_WHILE;

        case KEYWORD_RETURN:
            return TOKEN_RETURN;

        case KEYWORD_VOID:
            return TOKEN_VOID;

        default:
            fprintf(stderr, "Error: Unknown word\n");
            return TOKEN_UNKNOWN;
    }
}

Token* lexer(char* contents) {
    unsigned contents_length = strlen(contents);
    unsigned max_tokens = contents_length + 1;
    Token* tokens = (Token*)malloc(sizeof(Token) * max_tokens);
    if (!tokens) {
        perror("Error allocating space for tokens");
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_LENGTH + 1];
    int tokenidx = 0;
    int i = 0;
    char c;

    while ((c = contents[i]) != '\0') {
        if (isalpha(c)) {
            int bufferidx = 0;
            do {
                buffer[bufferidx++] = c;
                c = contents[++i];
            } while (isalnum(c) && bufferidx < MAX_LENGTH);

            buffer[bufferidx] = '\0';

            keyword_t keyword_type = get_keyword_type(buffer);
            if (keyword_type != KEYWORD_UNKNOWN) {
                Keyword word;
                word.name = strdup(buffer);
                word.type = keyword_type;

                tokens[tokenidx].type = keyword_to_token(&word);
                tokens[tokenidx].value.string = strdup(buffer);
                tokenidx++;

                free(word.name);
            } else {
                tokens[tokenidx].type = TOKEN_ID;
                tokens[tokenidx].value.string = strdup(buffer);
                tokenidx++;
            }

        } else if (isdigit(c) || c == '-' && isdigit(contents[i + 1])) {
            int bufferidx = 0;
            
            if (c == '-') {
                buffer[bufferidx++] = c;
                c = contents[++i];
            }

            do {
                buffer[bufferidx++] = c;
                c = contents[++i];
            } while (isdigit(c) && bufferidx < MAX_LENGTH);

            buffer[bufferidx] = '\0';
            tokens[tokenidx].type = TOKEN_INT_LITERAL;
            tokens[tokenidx].value.integer_value = atoi(buffer);
            tokenidx++;

        } else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '<' || c == '>' || c == '!' || c == '=') {
            TokenType type;
            char next_char = contents[i + 1];
            bool is_compound_op = false;
            char op_str[3] = {0};
            
            if (next_char == '=') {
                is_compound_op = true;
                op_str[0] = c;
                op_str[1] = '=';
                op_str[2] = '\0';

                switch (c) {
                    case '<': type = TOKEN_LESS_EQUAL; break;
                    case '>': type = TOKEN_GREATER_EQUAL; break;
                    case '!': type = TOKEN_NOT_EQUAL; break;
                    case '=': type = TOKEN_EQUAL; break;
                    case '+': type = TOKEN_ADD_AND_ASSIGN; break;
                    case '-': type = TOKEN_SUBTRACT_AND_ASSIGN; break;
                    case '*': type = TOKEN_MULTIPLY_AND_ASSIGN; break;
                    case '/': type = TOKEN_DIVIDE_AND_ASSIGN; break;
                    default: type = TOKEN_UNKNOWN; is_compound_op = false; break; 
                }

            } else if (next_char == '+') {
                is_compound_op = true;
                op_str[0] = c;
                op_str[1] = '+';
                op_str[2] = '\0';

                switch (c) {
                    case '+': type = TOKEN_INCREMENT; break;
                    default: type = TOKEN_UNKNOWN; is_compound_op = false; break;
                }

            } else if (next_char == '-') {
                is_compound_op = true;
                op_str[0] = c;
                op_str[1] = '-';
                op_str[2] = '\0';

                switch (c) {
                    case '-': type = TOKEN_DECREMENT; break;
                    default: type = TOKEN_UNKNOWN; is_compound_op = false; break;
                }

            } 

            if (is_compound_op) {
                tokens[tokenidx].type = type;
                tokens[tokenidx].value.string = strdup(op_str);
                tokenidx++;
                i += 2;

            } else {
                switch (c) {
                    case '+': type = TOKEN_ADD; break;
                    case '-': type = TOKEN_SUBTRACT; break;
                    case '*': type = TOKEN_MULTIPLY; break;
                    case '/': type = TOKEN_DIVIDE; break;
                    case '>': type = TOKEN_GREATER; break;
                    case '<': type = TOKEN_LESS; break;
                    case '=': type = TOKEN_ASSIGNMENT; break;
                    case '!': type = TOKEN_UNKNOWN; break;
                    default: type = TOKEN_UNKNOWN; break;
                } 
                
                tokens[tokenidx].type = type;
                tokens[tokenidx].value.character = c;
                tokenidx++;
                i++;

            }

        } else if (c == '=' || c == ';' || c == '(' || c == ')' || c == '{' || c == '}' || c ==  '[' || c == ']' || c == ',' || c == '_') {
            TokenType type;
            switch (c) {
                case '=': type = TOKEN_ASSIGNMENT; break;
                case ';': type = TOKEN_SEMICOLON; break;
                case '(': type = TOKEN_LEFT_PARENTHESES; break;
                case ')': type = TOKEN_RIGHT_PARENTHESES; break;
                case '{': type = TOKEN_LEFT_BRACE; break;
                case '}': type = TOKEN_RIGHT_BRACE; break;
                case '[': type = TOKEN_LEFT_BRACKET; break;
                case ']': type = TOKEN_RIGHT_BRACKET; break;
                case ',': type = TOKEN_COMMA; break;
                case '_': type = TOKEN_UNDERSCORE; break;
                default: type = TOKEN_UNKNOWN; break;
            }

            if (tokenidx >= max_tokens) {
                max_tokens *= 2;
                tokens = (Token*)realloc(tokens, sizeof(Token) * max_tokens);
                if (tokens == NULL) {
                    perror("Error reallocating space for tokens");
                    exit(EXIT_FAILURE);
                }
            }

            tokens[tokenidx].type = type;
            tokens[tokenidx].value.character = c;
            tokenidx++;
            i++;

        } else {
            i++;
        }
    }

    if (tokenidx >= max_tokens) {
        max_tokens += 1;
        tokens = (Token*)realloc(tokens, sizeof(Token) * max_tokens);
        if (tokens == NULL) {
            perror("Error reallocating space for tokens");
            exit(EXIT_FAILURE);
        }
    }

    tokens[tokenidx].type = TOKEN_EOF;

    return tokens;

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
            case TOKEN_ID:
                printf("IDENTIFIER, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_KEYWORD:
                printf("KEYWORD, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_RETURN:
                printf("RETURN, Value: %s\n", tokens[i].value.string);
                break;
            case TOKEN_UNKNOWN:
                printf("UNKNOWN\n");
                break;
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