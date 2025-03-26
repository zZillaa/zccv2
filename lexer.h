#ifndef LEXER_H
#define LEXER_H
#define MAX_LENGTH 1024
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

typedef long long integer_t;
typedef enum TokenType {
    // token keywords
    TOKEN_INT,
    TOKEN_CHAR,
    TOKEN_BOOLEAN,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_FOR,
    TOKEN_WHILE,
    TOKEN_RETURN,
    TOKEN_VOID,
    TOKEN_STRUCT,
    TOKEN_BREAK,
    TOKEN_CONTINUE,
    
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
    TOKEN_INCREMENT,
    TOKEN_DECREMENT,

    TOKEN_LEFT_BRACKET,
    TOKEN_RIGHT_BRACKET,
    TOKEN_LEFT_PARENTHESES,
    TOKEN_RIGHT_PARENTHESES,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,

    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_ASTERISK,
    TOKEN_POUND,
    TOKEN_UNDERSCORE,


    TOKEN_ASSIGNMENT,
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
    int line;
    int column;
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
    KEYWORD_STRUCT,
    KEYWORD_UNKNOWN
} keyword_t;

typedef struct {    
    keyword_t type;
    char* name;
} Keyword;

typedef struct {
    char* start;
    char* end;
    Token* tokens;
    int tokenIdx;
    int line;
    int column;
    int capacity;
} Lexer;

char peek_lexer(Lexer* lexer);
char peek_lexer_next(Lexer* lexer);
char advance_lexer(Lexer* lexer);
void operator(Lexer* lexer);
void number(Lexer* lexer);
void identifier(Lexer* lexer);
void marker(Lexer* lexer);
bool add_token(Lexer* lexer, Token token);
bool match(Lexer* lexer, char expected);

bool lexer_at_end(Lexer* lexer);
bool skip_lexer_whitespace(Lexer* lexer);
bool lexer_at_character(Lexer* lexer, char c);

Token create_token(TokenType type, int line, int column);
Token create_int_token(TokenType type, int value, int line, int column);
Token create_string_token(TokenType type, char* value, int line, int column);
Token create_char_token(TokenType type, char value, int line, int column);

Lexer* init_lexer(char* source);
Token* lexical_analysis(char* source);

keyword_t get_keyword_type(char* str);
TokenType keyword_to_token(Keyword* word);
// Token* lexer(char* contents);
void print_tokens(Token* tokens);
void free_token(Token* token);
void free_tokens(Token* tokens);

#endif