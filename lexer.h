#ifndef LEXER_H
#define LEXER_H
#define MAX_LENGTH 1024
#include <stdlib.h>

typedef long long integer_t;
typedef enum TokenType {
    TOKEN_INT,
    TOKEN_CHAR,
    TOKEN_BOOLEAN,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_FOR,
    TOKEN_WHILE,
    TOKEN_RETURN,
    TOKEN_VOID,
    
    TOKEN_ID,
    TOKEN_INT_LITERAL,
    TOKEN_BOOLEAN_LITERAL,
    TOKEN_STRING_LITERAL,
    TOKEN_KEYWORD,


    TOKEN_ADD,
    TOKEN_SUBTRACT,
    TOKEN_ADD_AND_ASSIGN,
    TOKEN_SUBTRACT_AND_ASSIGN,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_MULTIPLY_AND_ASSIGN,
    TOKEN_DIVIDE_AND_ASSIGN,
    TOKEN_LEFT_PARENTHESES,
    TOKEN_RIGHT_PARENTHESES,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_ASSIGNMENT,
    TOKEN_INCREMENT,
    TOKEN_DECREMENT,


    TOKEN_GREATER,
    TOKEN_LESS,
    TOKEN_GREATER_EQUAL,
    TOKEN_LESS_EQUAL,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_NOT,

    TOKEN_UNKNOWN,
    TOKEN_EOF
} TokenType;

typedef union TokenValue{
    char* string;
    char character;
    int integer_value;
} TokenValue;

typedef struct Token {
    TokenType type; 
    TokenValue value;
} Token;

typedef enum {
    KEYWORD_INT,
    KEYWORD_CHAR,
    KEYWORD_BOOL,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_FOR,
    KEYWORD_WHILE,
    KEYWORD_RETURN,
    KEYWORD_VOID,
    KEYWORD_UNKNOWN
} keyword_t;

typedef struct {    
    keyword_t type;
    char* name;
} Keyword;

keyword_t get_keyword_type(char* str);
TokenType keyword_to_token(Keyword* word);
Token* lexer(char* contents);
void print_tokens(Token* tokens);
void free_token(Token* token);
void free_tokens(Token* tokens);

#endif