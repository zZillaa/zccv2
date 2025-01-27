#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"

// FOR DECLARATIONS
struct decl {
    char* name;
    struct type* type;
    struct expr* value;
    struct stmt* code;
    struct decl* next;
    struct symbol* symbol;
};

// FOR STATEMENTS // 
typedef enum {
    STMT_DECL,
    STMT_EXPR,
    STMT_IF_ELSE,
    STMT_IF,
    STMT_FOR,
    STMT_WHILE,
    STMT_PRINT,
    STMT_RETURN,
    STMT_BLOCK
} stmt_t;

struct stmt {
    stmt_t kind;
    struct decl* decl;
    struct expr* init_expr;
    struct expr* expr;
    struct expr* next_expr;
    struct stmt* body;
    struct stmt* else_body;
    struct stmt* next;
    struct symbol* symbol;
};
// FOR STATEMENTS // 


// FOR EXPRESSIONS //
typedef enum {
    EXPR_ADD,
    EXPR_SUB,
    EXPR_MUL,
    EXPR_DIV,

    EXPR_ADD_AND_ASSIGN,
    EXPR_SUB_AND_ASSIGN,
    EXPR_MUL_AND_ASSIGN,
    EXPR_DIV_AND_ASSIGN,
    
    EXPR_INCREMENT,
    EXPR_DECREMENT,
    EXPR_NOT,
    
    EXPR_LESS,
    EXPR_GREATER,
    EXPR_GREATER_EQUAL,
    EXPR_LESS_EQUAL,
    EXPR_EQUAL,
    EXPR_NOT_EQUAL,

    EXPR_NAME,
    EXPR_ASSIGNMENT,
    
    EXPR_CALL,
    EXPR_ARG,
    
    EXPR_SUBSCRIPT,

    EXPR_INTEGER,
    EXPR_CHARACTER,
    EXPR_BOOLEAN,
    EXPR_STRING,
    
    EXPR_UNKNOWN

} expr_t;


struct expr {
    expr_t kind;
    struct expr* left;
    struct expr* right;

    int integer_value;
    char ch_expr;
    char* name;
    char* string_literal;
    struct symbol* symbol;
};

// FOR EXPRESSIONS

// TYPE STRUCTURES
typedef enum type_t{
    TYPE_VOID,
    TYPE_OPERATOR,
    TYPE_BOOLEAN,
    TYPE_CHARACTER,
    TYPE_INTEGER,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_FUNCTION,
    TYPE_UNKNOWN
} type_t;


struct type {
    type_t kind;
    struct type* subtype;
    struct param_list* params;
};

struct param_list {
    char* name;
    struct type* type;
    struct param_list* next;
    struct symbol* symbol;
};

// TYPE STRUCTURES

struct program {
    struct decl* declaration;
};

typedef enum {
    SYMBOL_LOCAL,
    SYMBOL_PARAM,
    SYMBOL_GLOBAL
} symbol_t;


struct symbol {
    symbol_t kind;
    struct type* type;
    char* name;
    struct symbol* next;
};

struct symbol_table {
    struct symbol* symbol;
    struct symbol_table* next;
};

struct stack {
    int top;
    struct symbol_table** symbol_tables;
};

typedef enum type_t{
    TYPE_VOID,
    TYPE_OPERATOR,
    TYPE_BOOLEAN,
    TYPE_CHARACTER,
    TYPE_INTEGER,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_FUNCTION,
    TYPE_UNKNOWN
} type_t;

struct type {
    type_t kind;
    struct type* subtype;
    struct param_list* params;
};

struct param_list {
    char* name;
    struct type* type;
    struct param_list* next;
    struct symbol* symbol;
};


expr_t get_expr_type(Token* token);
stmt_t get_stmt_type(Token* token);
type_t get_type(Token* token);

struct expr* expr_create_integer_literal( int i );
struct expr* expr_create_boolean_literal( int b );
struct expr* expr_create_char_literal( char ch );
struct expr* expr_create_string_literal( char* str );
struct expr* expr_create(expr_t kind, struct expr* L, struct expr* R );


struct expr* parse_additive(Token* tokens, int* tokenIdx);
struct expr* parse_factor(Token* tokens, int* tokenIdx);
struct expr* parse_term(Token* tokens, int* tokenIdx);
struct expr* parse_expression(Token* tokens, int* tokenIdx);
struct stmt* parse_block(Token* tokens, int* tokenIdx); 
struct stmt* parse_statement(Token* tokens, int* tokenIdx);
struct param_list* parse_parameters(Token* tokens, int* tokenIdx);
struct decl* parse_function(Token* tokens, int* tokenIdx, char* name, struct type* return_type);
struct decl* parse_declaration(Token* tokens, int* tokenIdx);


struct stmt* stmt_create(stmt_t kind, struct decl* decl,
    struct expr* init_expr, struct expr* expr,
    struct expr* next_expr, struct stmt* body,
    struct stmt* else_body, struct stmt* next );


struct decl* decl_create(char* name, struct type* type,
    struct expr* value, struct stmt* code, 
    struct decl* next );

struct program* build_ast(Token* tokens);
void free_ast(struct program* root);
void free_node(struct decl* declaration);
void print_type(struct type* type, int indent);
void print_expr(struct expr* expr, int indent);
void print_stmt(struct stmt* stmt, int indent);
void print_decl(struct decl* decl, int indent);
void print_ast(struct program* program);
#endif