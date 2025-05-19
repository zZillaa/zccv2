#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "ast.h"

struct stmt_t_stack* cf_stack = NULL;

struct expr* expr_create_integer_literal(int i) {
    struct expr* node = expr_create(EXPR_INTEGER, NULL, NULL);
    if (!node) {
        perror("Error allocating space for expression node");
        return NULL;
    }

    node->integer_value = i;
    node->ch_expr = 0;
    node->name = NULL;
    node->string_literal = NULL;
    node->symbol = NULL;
    node->reg = -1;

    return node;
}

struct expr* expr_create_char_literal(char ch) {
    struct expr* node = expr_create(EXPR_CHARACTER, NULL, NULL);
    if (!node) {
        perror("Error allocating space for expression node");
        return NULL;
    }
    node-> ch_expr = ch;
    node->integer_value = 0;
    node->name = NULL;
    node->string_literal = NULL;
    node->symbol = NULL;
    node->reg = -1;

    return node;
}

struct expr* expr_create_boolean_literal(int b) {
    struct expr* node = expr_create(EXPR_BOOLEAN, NULL, NULL);
    if (!node) {
        perror("Error allocating space for expression node");
        return NULL;
    }
    node->integer_value = b;
    node->ch_expr = 0;
    node->name = NULL;
    node->string_literal = NULL;
    node->symbol = NULL;
    node->reg = -1;

    return node;
}

struct expr* expr_create_string_literal(char* str) {
    struct expr* node = (struct expr*)malloc(sizeof(struct expr));
    if (!node) {
        perror("Error allocating space for expression node");
        return NULL;
    }

    node->kind = EXPR_STRING;
    node->left = NULL;
    node->right = NULL;
    node->name = NULL;
    node->integer_value = 0;
    node->ch_expr = 0;
    node->string_literal = strdup(str);
    node->symbol = NULL;
    node->reg = -1;

    return node;
}

struct expr* expr_create(expr_t kind, struct expr* left, struct expr* right) {
    struct expr* node = malloc(sizeof(struct expr));
    if (!node) {
        fprintf(stderr, "Critical: Memory allocation failed for expression\n");
        exit(EXIT_FAILURE);
    }

    node->kind = kind;
    node->left = left;
    node->right = right;
    node->name = NULL;
    node->integer_value = 0;
    node->ch_expr = 0;
    node->string_literal = NULL;
    node->symbol = NULL;
    node->reg = -1;

    return node;
}

struct decl* decl_create(char* name, struct type* type, struct expr* value, struct stmt* code, struct decl* next) {
    struct decl* node = (struct decl*)malloc(sizeof(struct decl));
    if (!node) {
        perror("Error allocating space for desclaration node");
        return NULL;
    }

    node->name = strdup(name);
    if (!node->name) {
        fprintf(stderr, "Error: Unable to duplicate name");
        return NULL;
    }
    node->type = type;
    node->value = value;
    node->code = code;
    node->next = next;
    node->symbol = NULL;
    node->reg = -1;

    return node;
}

struct stmt* stmt_create(stmt_t kind, struct decl* decl, struct expr* init_expr, struct expr* expr, struct expr* next_expr, struct stmt* body, struct stmt* else_body, struct stmt* next) {
    struct stmt* node = (struct stmt*)malloc(sizeof(struct stmt));
    if (node == NULL) {
        perror("Error allocating space for stmt node");
        return NULL;
    }

    node->kind = kind;
    node->decl = decl;
    node->init_expr = init_expr;
    node->expr = expr;
    node->next_expr = next_expr;
    node->body = body;
    node->else_body = else_body;
    node->next = next;
    node->symbol = NULL;
    return node;
}

struct stmt_t_stack* create_stmt_t_stack() {
    struct stmt_t_stack* stack = malloc(sizeof(struct stmt_t_stack));
    if (!stack) {
        fprintf(stderr, "Error: Unable to allocate space for stmt_t_stack\n");
        return NULL;
    }

    for (int i = 0; i < STACK_SIZE; i++) {
        stack->cf_statements[i] = -1;
    }

    stack->top = -1;
    return stack;
}

void push_stmt_t(struct stmt_t_stack* stack, stmt_t type) {
    if (stack->top >= STACK_SIZE - 1) {
        fprintf(stderr, "Error: stmt_t stack is full\n");
        return;
    } else {
        stack->cf_statements[++stack->top] = type;
    }
}

stmt_t pop_stmt_t(struct stmt_t_stack* stack) {
    if (!stack) return -1;

    if (!is_stmt_t_stack_empty(stack)) {
        return stack->cf_statements[stack->top--];
    }

    return -1;
}

stmt_t peek_stmt_t(struct stmt_t_stack* stack) {
    if (!stack) return -1;

    if (!is_stmt_t_stack_empty(stack)) {
        return stack->cf_statements[stack->top];
    }

    return -1;
}

bool is_stmt_t_stack_empty(struct stmt_t_stack* stack) {
    if (!stack) return true;
    return stack->top == -1;
}

type_t get_type(Token* token) {
    switch (token->type) {
        case TOKEN_INT: return TYPE_INTEGER;
        case TOKEN_BOOLEAN: return TYPE_BOOLEAN;
        case TOKEN_CHAR: return TYPE_CHARACTER;
        case TOKEN_VOID: return TYPE_VOID;
        case TOKEN_STRUCT: return TYPE_STRUCT;
        default:
            fprintf(stderr, "Error: Encountered unknown token type\n");
            return TOKEN_UNKNOWN;
    }
}

stmt_t get_stmt_type(Token* token) {
    switch (token->type) {
        case TOKEN_ID: return STMT_DECL;
        case TOKEN_RETURN: return STMT_RETURN;
        case TOKEN_FOR: return STMT_FOR;
        case TOKEN_WHILE: return STMT_WHILE;
    }
}

expr_t get_expr_type(Token* token) {
    switch (token->type) {
        case TOKEN_BREAK: return EXPR_BREAK;
        case TOKEN_CONTINUE: return EXPR_CONTINUE;
        case TOKEN_INT_LITERAL: return EXPR_INTEGER;
        case TOKEN_STRING_LITERAL: return EXPR_STRING;
        case TOKEN_BOOLEAN_LITERAL: return EXPR_BOOLEAN;
        case TOKEN_ID: return EXPR_NAME;
        case TOKEN_ASSIGNMENT: return EXPR_ASSIGNMENT;
        case TOKEN_CHAR: return EXPR_CHARACTER;
        case TOKEN_LESS: return EXPR_LESS;
        case TOKEN_GREATER: return EXPR_GREATER;
        case TOKEN_LESS_EQUAL: return EXPR_LESS_EQUAL;
        case TOKEN_GREATER_EQUAL: return EXPR_GREATER_EQUAL;
        case TOKEN_NOT_EQUAL: return EXPR_NOT_EQUAL;
        case TOKEN_ADD_AND_ASSIGN: return EXPR_ADD_AND_ASSIGN;
        case TOKEN_SUBTRACT_AND_ASSIGN: return EXPR_SUB_AND_ASSIGN;
        case TOKEN_MULTIPLY_AND_ASSIGN: return EXPR_MUL_AND_ASSIGN;
        case TOKEN_DIVIDE_AND_ASSIGN: return EXPR_DIV_AND_ASSIGN;
        case TOKEN_INCREMENT: return EXPR_INCREMENT;
        case TOKEN_DECREMENT: return EXPR_DECREMENT;
        case TOKEN_ADD: return EXPR_ADD;
        case TOKEN_SUBTRACT: return EXPR_SUB;
        case TOKEN_MULTIPLY: return EXPR_MUL;
        case TOKEN_DIVIDE: return EXPR_DIV;
        default:
            fprintf(stderr, "Error: Unrecognized token type %d for expression.\n", token->type);
            exit(EXIT_FAILURE);
    }
}

struct expr* parse_arg_exprs(Token* tokens, int* tokenIdx) {
    struct expr* head = NULL;
    struct expr* current = NULL;

    while (tokens[*tokenIdx].type != TOKEN_RIGHT_PARENTHESES) {
        struct expr* node = parse_expression(tokens, tokenIdx);
        if (!node) {
            fprintf(stderr, "Error: arg expr node is null\n");
            return NULL;
        }

        if (!head) {
            head = node;
            current = node;
        } else {
            current->right = node;
            current = node;
        }

        if (tokens[*tokenIdx].type == TOKEN_COMMA) (*tokenIdx)++;
    }


    return head;
}

struct expr* parse_factor(Token* tokens, int* tokenIdx) {
    struct expr* expr_node = NULL;

    switch(tokens[*tokenIdx].type) {
        case TOKEN_INT_LITERAL:
            (*tokenIdx)++;
            return expr_create_integer_literal(tokens[*tokenIdx-1].value.integer_value);

        case TOKEN_ID:
            (*tokenIdx)++;
            expr_node = expr_create(EXPR_NAME, NULL, NULL);
            expr_node->name = strdup(tokens[*tokenIdx-1].value.string);

            if (tokens[*tokenIdx].type == TOKEN_LEFT_BRACKET) {
                (*tokenIdx)++;
                struct expr* index_expr = parse_expression(tokens, tokenIdx);
                if (tokens[*tokenIdx].type != TOKEN_RIGHT_BRACKET) {
                    fprintf(stderr, "Error: Expected ']' in array subscript\n");
                    return NULL;
                }
                (*tokenIdx)++;
                expr_node = expr_create(EXPR_SUBSCRIPT, expr_node, index_expr);      
                printf("EXPR SUBSCRIPT NODE LEFT KIND: %d\n", expr_node->left->kind);
                printf("EXPR SUBSCRIPT NODE RIGHT KIND: %d\n", expr_node->right->kind);
            
            } else if (tokens[*tokenIdx].type == TOKEN_INCREMENT || tokens[*tokenIdx].type == TOKEN_DECREMENT) {
                expr_t op_kind = get_expr_type(&tokens[*tokenIdx]);
                (*tokenIdx)++;
                struct expr* postfix_epr = expr_create(op_kind, expr_node, NULL);
                return postfix_epr;

            } else if (tokens[*tokenIdx].type == TOKEN_LEFT_PARENTHESES) {
                (*tokenIdx)++;

                struct expr* arguments_expr = parse_arg_exprs(tokens, tokenIdx);
                if (tokens[*tokenIdx].type != TOKEN_RIGHT_PARENTHESES) {
                    fprintf(stderr, "Error: Expected ')' after listing arguments\n");
                    return NULL; 
                }
                (*tokenIdx)++;
                expr_node = expr_create(EXPR_CALL, expr_node, arguments_expr);
            }

            return expr_node;

        case TOKEN_LEFT_PARENTHESES:
            (*tokenIdx)++;
            expr_node= parse_expression(tokens, tokenIdx);
            if (tokens[*tokenIdx].type != TOKEN_RIGHT_PARENTHESES) {
                fprintf(stderr, "Error: Mismatched parentheses expected ')'\n");
                return NULL;
            }

            (*tokenIdx)++;
            return expr_node;

        case TOKEN_LEFT_BRACKET:
            (*tokenIdx)++;
            expr_node = parse_expression(tokens, tokenIdx);
            if (tokens[*tokenIdx].type != TOKEN_RIGHT_BRACKET) {
                fprintf(stderr, "Error: Mismatched brackets, expected ']'\n");
                return NULL;
            }
            (*tokenIdx)++;
            return expr_node;

        case TOKEN_ASSIGNMENT:
        case TOKEN_GREATER:
        case TOKEN_LESS:
            (*tokenIdx)++;
            printf("CHARACTER Value: %c\n", tokens[*tokenIdx-1].value.character);
            return expr_create_char_literal(tokens[*tokenIdx-1].value.character);

        case TOKEN_INCREMENT:
        case TOKEN_DECREMENT:
            expr_t op_kind = get_expr_type(&tokens[*tokenIdx]);
            (*tokenIdx)++;
            struct expr* operand = parse_factor(tokens, tokenIdx);
            return expr_create(op_kind, operand, NULL);

        default:
            fprintf(stderr, "Error: Unexpected token in factor\n");
            return NULL;
    }
}

struct expr* parse_term(Token* tokens, int* tokenIdx) {
    struct expr* expr_left = parse_factor(tokens, tokenIdx);

    while (tokens[*tokenIdx].type == TOKEN_MULTIPLY || tokens[*tokenIdx].type == TOKEN_DIVIDE ||
        tokens[*tokenIdx].type == TOKEN_MULTIPLY_AND_ASSIGN || tokens[*tokenIdx].type == TOKEN_DIVIDE_AND_ASSIGN) {
        expr_t op_kind = get_expr_type(&tokens[*tokenIdx]);
        
        (*tokenIdx)++;

        struct expr* expr_right = parse_factor(tokens, tokenIdx);
        expr_left = expr_create(op_kind, expr_left, expr_right);
    }   

    return expr_left;
}

struct expr* parse_additive(Token* tokens, int* tokenIdx) {
    struct expr* expr_left = parse_term(tokens, tokenIdx);

    while ( tokens[*tokenIdx].type == TOKEN_ADD || 
            tokens[*tokenIdx].type == TOKEN_SUBTRACT ||
            tokens[*tokenIdx].type == TOKEN_ADD_AND_ASSIGN || 
            tokens[*tokenIdx].type == TOKEN_SUBTRACT_AND_ASSIGN ) {
        expr_t op_kind = get_expr_type(&tokens[*tokenIdx]);

        (*tokenIdx)++;
        struct expr* expr_right = parse_term(tokens, tokenIdx);
        expr_left = expr_create(op_kind, expr_left, expr_right);
    }

    return expr_left;
}

struct expr* parse_expression(Token* tokens, int* tokenIdx) {
    struct expr* expr_left = parse_additive(tokens, tokenIdx);
    printf("IN PARSE EXPRESSION\n");
    if (tokens[*tokenIdx].type == TOKEN_ASSIGNMENT ||
        tokens[*tokenIdx].type == TOKEN_ADD_AND_ASSIGN ||
        tokens[*tokenIdx].type == TOKEN_SUBTRACT_AND_ASSIGN ||
        tokens[*tokenIdx].type == TOKEN_MULTIPLY_AND_ASSIGN ||
        tokens[*tokenIdx].type == TOKEN_DIVIDE_AND_ASSIGN) {
        
        printf("TOKEN TYPE: %d\n", tokens[*tokenIdx].type);
        expr_t op_kind = get_expr_type(&tokens[*tokenIdx]);
        (*tokenIdx)++;
        
        struct expr* expr_right = parse_expression(tokens, tokenIdx);
        expr_left = expr_create(op_kind, expr_left, expr_right);
        printf("RETURNING FROM PARSE EXPRESSION\n");
        printf("e->left kind: %d\n", expr_left->left->kind);
        printf("e->right kind: %d\n", expr_left->right->kind);
        return expr_left;
    }

    while (tokens[*tokenIdx].type == TOKEN_LESS || tokens[*tokenIdx].type == TOKEN_GREATER ||
        tokens[*tokenIdx].type == TOKEN_LESS_EQUAL || tokens[*tokenIdx].type == TOKEN_GREATER_EQUAL
        || tokens[*tokenIdx].type == TOKEN_NOT_EQUAL || tokens[*tokenIdx].type == TOKEN_EQUAL) {
        expr_t op_kind;

        switch(tokens[*tokenIdx].type) {
            case TOKEN_LESS:
                op_kind = EXPR_LESS;
                break;
            case TOKEN_GREATER:
                op_kind = EXPR_GREATER;
                break;
            case TOKEN_GREATER_EQUAL:
                op_kind = EXPR_GREATER_EQUAL;
                break;
            case TOKEN_LESS_EQUAL:
                op_kind = EXPR_LESS_EQUAL;
                break;
            case TOKEN_NOT_EQUAL:
                op_kind = EXPR_NOT_EQUAL;
                break;
            case TOKEN_EQUAL:
                op_kind = EXPR_EQUAL;
                break;
        }

        (*tokenIdx)++;
        
        struct expr* expr_right = parse_additive(tokens, tokenIdx);
        expr_left = expr_create(op_kind, expr_left, expr_right);
    }

    return expr_left;
}

struct stmt* parse_block(Token* tokens, int* tokenIdx) {
    struct stmt* head = NULL;
    struct stmt* current = NULL;

    while (tokens[*tokenIdx].type != TOKEN_RIGHT_BRACE) {
        if (tokens[*tokenIdx].type == TOKEN_SEMICOLON) {
            (*tokenIdx)++;
            continue;
        }
        
        struct stmt* new_stmt = parse_statement(tokens, tokenIdx);
        if (!new_stmt) {
            fprintf(stderr, "Error: Unable to parse new statement\n");
            return NULL;
        }

        if (!head) {
            head = new_stmt;
            current = new_stmt;
        } else {
            current->next = new_stmt;
            current = new_stmt;
        }
    }

    (*tokenIdx)++;
    return head;
}


struct stmt* parse_statement(Token* tokens, int* tokenIdx) {
    struct stmt* stmt = NULL;

    switch (tokens[*tokenIdx].type) {
        case TOKEN_CHAR:
        case TOKEN_INT: {
            type_t kind = get_type(&tokens[*tokenIdx]);   
            (*tokenIdx)++;

            if (tokens[*tokenIdx].type != TOKEN_ID) {
                fprintf(stderr, "Error: Expected identifier after type\n");
                return NULL;
            }

            char* id = strdup(tokens[*tokenIdx].value.string);
            (*tokenIdx)++;

            struct type* var_type = type_create(kind, NULL, NULL);
            if (!var_type) return NULL;

            struct decl* decl = NULL;

            if (tokens[*tokenIdx].type == TOKEN_ASSIGNMENT) {
                decl = decl_create(id, var_type, NULL, NULL, NULL);
                (*tokenIdx)++;
                decl->value = parse_expression(tokens, tokenIdx);
                stmt = stmt_create(STMT_DECL, decl, NULL, NULL, NULL, NULL, NULL, NULL);
            } else if (tokens[*tokenIdx].type == TOKEN_LEFT_BRACKET) {
                (*tokenIdx)++;
                decl = parse_array(tokens, tokenIdx, id, var_type);
                // printf("Current token type: %d\n", tokens[*tokenIdx].type);
                stmt = stmt_create(STMT_DECL, decl, NULL, NULL, NULL, NULL, NULL, NULL);
            }

            break;
        }

        case TOKEN_ELSE: {
            if (is_stmt_t_stack_empty(cf_stack) ||
                peek_stmt_t(cf_stack) != STMT_IF && peek_stmt_t(cf_stack) != STMT_ELSE_IF) {
                fprintf(stderr, "Error: 'else' without matching 'if' or 'else if'\n");
                return NULL;
            }

            pop_stmt_t(cf_stack);
            (*tokenIdx)++;

            if (tokens[*tokenIdx].type == TOKEN_KEYWORD && strcmp(tokens[*tokenIdx].value.string, "if") == 0) {
                push_stmt_t(cf_stack, STMT_ELSE_IF);

                (*tokenIdx)++;

                if (tokens[*tokenIdx].type != TOKEN_LEFT_PARENTHESES) {
                    fprintf(stderr, "Error: Expected '(' after 'else if'\n");
                    return NULL;
                }
                
                (*tokenIdx)++;

                struct expr* condition = parse_expression(tokens, tokenIdx);
                if (!condition) {
                    return NULL;
                }

                if (tokens[*tokenIdx].type != TOKEN_RIGHT_PARENTHESES) {
                    fprintf(stderr, "Error: Expected ')' after condition\n");
                    return NULL;
                }

                (*tokenIdx)++;

                if (tokens[*tokenIdx].type != TOKEN_LEFT_BRACE) {
                    fprintf(stderr, "Error: Expected '{' after condition for 'else if'\n");
                    return NULL;
                }

                (*tokenIdx)++;

                struct stmt* body = parse_block(tokens, tokenIdx);
                if (!body) {
                    return NULL;
                }

                struct stmt* else_body = NULL;

                if (tokens[*tokenIdx].type == TOKEN_ELSE) {
                    else_body = parse_statement(tokens, tokenIdx);
                    if (!else_body) {
                        return NULL;
                    }
                } else {
                    pop_stmt_t(cf_stack);
                }

                stmt = stmt_create(STMT_ELSE_IF, NULL, NULL, condition, NULL, body, else_body, NULL);
            
            } else if (tokens[*tokenIdx].type == TOKEN_LEFT_BRACE) {
                (*tokenIdx)++;

                struct stmt* body = parse_block(tokens, tokenIdx);
                if (!body) {
                    return NULL;
                }

                stmt = stmt_create(STMT_ELSE, NULL, NULL, NULL, NULL, body, NULL, NULL);
                printf("IN TOKEN_ELSE CREATED STMT_ELSE STATEMENT\n");
            } else {
                fprintf(stderr, "Error: Expected 'if' or '{' after 'else'\n");
                return NULL;
            }
            break;
        }

        case TOKEN_IF: {
            push_stmt_t(cf_stack, STMT_IF);

            (*tokenIdx)++;
            if (tokens[*tokenIdx].type != TOKEN_LEFT_PARENTHESES) {
                fprintf(stderr, "Error: Expected '(' after 'if' keyword\n");
                return NULL;
            }

            (*tokenIdx)++;
            struct expr* condition = parse_expression(tokens, tokenIdx);
            if (tokens[*tokenIdx].type != TOKEN_RIGHT_PARENTHESES) {
                fprintf(stderr, "Error: Mismatched parentheses expected ')'\n");
                return NULL;
            }
            (*tokenIdx)++;

            if (tokens[*tokenIdx].type != TOKEN_LEFT_BRACE) {
                fprintf(stderr, "Error: Expected '{' after if expression\n");
                return NULL;
            }
            (*tokenIdx)++;

            struct stmt* body = parse_block(tokens, tokenIdx);
            struct stmt* else_body = NULL;

            if (tokens[*tokenIdx].type == TOKEN_ELSE) {
                printf("Current token type: %d\n", tokens[*tokenIdx].type);
                else_body = parse_statement(tokens, tokenIdx);
            } else {
                pop_stmt_t(cf_stack);
            }

            stmt = stmt_create(STMT_IF, NULL, NULL, condition, NULL, body, else_body, NULL);
            break;
        } 

        case TOKEN_FOR: {
            (*tokenIdx)++;
            if (tokens[*tokenIdx].type != TOKEN_LEFT_PARENTHESES) {
                fprintf(stderr, "Expected '(' after 'for' keyword\n");
                return NULL;
            }

            (*tokenIdx)++;
            type_t type_kind = get_type(&tokens[*tokenIdx]);   
            (*tokenIdx)++;

            if (tokens[*tokenIdx].type != TOKEN_ID) {
                fprintf(stderr, "Error: Expected identifier after type\n");
                return NULL;
            }

            char* id = strdup(tokens[*tokenIdx].value.string);
            (*tokenIdx)++;

            struct type* var_type = (struct type*)malloc(sizeof(struct type));
            var_type->kind = type_kind;
            var_type->subtype = NULL;
            var_type->params = NULL;

            struct decl* decl = decl_create(id, var_type, NULL, NULL, NULL);

            if (tokens[*tokenIdx].type == TOKEN_ASSIGNMENT) {
                (*tokenIdx)++;
                decl->value = parse_expression(tokens, tokenIdx);
            }

            if (tokens[*tokenIdx].type != TOKEN_SEMICOLON) {
                fprintf(stderr, "Error: Expected ';' after for loop initialization\n");
                return NULL;
            }
            (*tokenIdx)++;

            struct expr* condition = parse_expression(tokens, tokenIdx);
            if (tokens[*tokenIdx].type != TOKEN_SEMICOLON) {
                fprintf(stderr, "Error: Expected a 2nd ';' after main expression\n");
                return NULL;
            }
            (*tokenIdx)++;

            struct expr* next_expr = parse_expression(tokens, tokenIdx);
            if (tokens[*tokenIdx].type != TOKEN_RIGHT_PARENTHESES) {
                fprintf(stderr, "Error: Expectec ')' after for loop increment\n");
                return NULL;
            }
            (*tokenIdx)++;

            if (tokens[*tokenIdx].type != TOKEN_LEFT_BRACE) {
                fprintf(stderr, "Error: Expected '{' after for loop header\n");
                return NULL;
            }
            (*tokenIdx)++;

            struct stmt* body = parse_block(tokens, tokenIdx);
            stmt = stmt_create(STMT_FOR, decl, NULL, condition, next_expr, body, NULL, NULL);
            break;
        }

        case TOKEN_WHILE: {
            (*tokenIdx)++;
            if (tokens[*tokenIdx].type != TOKEN_LEFT_PARENTHESES) {
                fprintf(stderr, "Error: Expected '(' after 'while' keyword\n");
                return NULL;
            } 
            (*tokenIdx)++;

            struct expr* condition = parse_expression(tokens, tokenIdx);

            if (tokens[*tokenIdx].type != TOKEN_RIGHT_PARENTHESES) {
                fprintf(stderr, "Error: Mismatched parentheses expected ')'\n");
                return NULL;
            }
            (*tokenIdx)++;

            if (tokens[*tokenIdx].type != TOKEN_LEFT_BRACE) {
                fprintf(stderr, "Error: Expected '{' after 'while loop' initialization\n");
                return NULL;
            }
            (*tokenIdx)++;

            struct stmt* body = parse_block(tokens, tokenIdx);
            stmt = stmt_create(STMT_WHILE, NULL, NULL, condition, NULL, body, NULL, NULL);
            break;
        }
        case TOKEN_ID: {
            struct expr* expr = parse_expression(tokens, tokenIdx);
            printf("FINISHED PARSING STATEMENT STARTING WITH TOKEN_ID\n");
            printf("EXPR KIND: %d\n", expr->kind);
            if (expr->name) {
                printf("Expression name: %s\n", expr->name);
            }

            stmt = stmt_create(STMT_EXPR, NULL, NULL, expr, NULL, NULL, NULL, NULL);
            break;
        }

        case TOKEN_BREAK: {
            (*tokenIdx)++;

            if (tokens[*tokenIdx].type != TOKEN_SEMICOLON) {
                fprintf(stderr, "Error: Expected ';' after 'break' statement\n");
                return NULL;
            }

            stmt = stmt_create(STMT_BREAK, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            break;
        } 

        case TOKEN_CONTINUE: {
            (*tokenIdx)++;

            if (tokens[*tokenIdx].type != TOKEN_SEMICOLON) {
                fprintf(stderr, "Error: Expected ';' after 'continue' statement\n");
                return NULL;
            }

            stmt = stmt_create(STMT_CONTINUE, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
            break;
        }

        case TOKEN_RETURN: {
            (*tokenIdx)++;
            struct expr* return_expr = NULL;

            if (tokens[*tokenIdx].type != TOKEN_SEMICOLON) {
                return_expr = parse_expression(tokens, tokenIdx);
                if (tokens[*tokenIdx].type != TOKEN_SEMICOLON) {
                    fprintf(stderr, "Error: Expected semicolon after return expression\n");
                    return NULL;
                }
                (*tokenIdx)++;
            } else {
                (*tokenIdx)++;
            }
            
            stmt = stmt_create(STMT_RETURN, NULL, NULL, return_expr, NULL, NULL, NULL, NULL);
            break;
        }

        // DEFAULT CASE IS EXECUTING WHEN TOKEN_RETURN IS A VALID TOKEN TYPE?
        default:
            fprintf(stderr, "Error: Unexpected token in statement\n");
            printf("Current Token type: %d\n", tokens[*tokenIdx].type);
            printf("Prev token type: %d\n", tokens[*tokenIdx - 1].type);
            printf("And token type one more time: %d\n", tokens[*tokenIdx - 2].type);
            return NULL;
    }

    if (stmt->kind != STMT_IF && stmt->kind != STMT_FOR && stmt->kind != STMT_WHILE &&
        stmt->kind != STMT_DECL && stmt->kind != STMT_RETURN && stmt->kind != STMT_EXPR && 
        stmt->kind != STMT_ELSE && stmt->kind != STMT_ELSE_IF) {
        printf("Token type: %d\n", tokens[*tokenIdx].type);
        if (tokens[*tokenIdx].type != TOKEN_SEMICOLON) {
            fprintf(stderr, "Error: Expected semicolon\n");
            return NULL;
        }
        (*tokenIdx)++;
    }

    return stmt;
}

struct param_list* parse_parameters(Token* tokens, int* tokenIdx) {
    struct param_list* head = NULL;
    struct param_list* current = NULL;

    while (tokens[*tokenIdx].type != TOKEN_RIGHT_PARENTHESES) {
        if (tokens[*tokenIdx].type != TOKEN_INT) {
            fprintf(stderr, "Error: Expected type keyword in parameter.\n");
            return NULL;
        }

        type_t param_list_type = get_type(&tokens[*tokenIdx]);
        (*tokenIdx)++;

        if (tokens[*tokenIdx].type != TOKEN_ID) {
            fprintf(stderr, "Error: Expected identifier in paramter.\n");
            exit(EXIT_FAILURE);
        } 

        struct param_list* node = (struct param_list*)malloc(sizeof(struct param_list));
        
        node->name = strdup(tokens[*tokenIdx].value.string);
        node->type = (struct type*)malloc(sizeof(struct type));
        if (node->type == NULL) {
            perror("Error allocating type for parameter");
            free(node);
            return NULL;
        }

        node->type->kind = param_list_type;
        node->type->subtype = NULL;
        node->type->params = NULL;
        node->next = NULL;

        (*tokenIdx)++;

        if (head == NULL) {
            head = node;
            current = node;
        } else {
            current->next = node;
            current = current->next; 
        }

        if (tokens[*tokenIdx].type == TOKEN_COMMA) {
            (*tokenIdx)++;
        } else if (tokens[*tokenIdx].type != TOKEN_RIGHT_PARENTHESES) {
            fprintf(stderr, "Error: Expected ',' or ')' in parameter list.\n");
            return NULL;
        }

    }

    (*tokenIdx)++;

    return head;
}

struct decl* parse_function(Token* tokens, int* tokenIdx, char* name, struct type* return_type) {   

    if (!return_type) {
        fprintf(stderr, "Error: Function '%s' has no return type\n", name);
        return NULL;
    }

    struct param_list* params = parse_parameters(tokens, tokenIdx);
    struct type* func_type = type_create(TYPE_FUNCTION, return_type, params);

    if (!func_type) {
        fprintf(stderr, "Error: Failed to create funciton type for '%s'\n", name);
        return NULL;
    }

    if (tokens[*tokenIdx].type != TOKEN_LEFT_BRACE) {
        fprintf(stderr, "Expected '{' after initializing parameters\n");
        return NULL;
    }
    (*tokenIdx)++;

    struct stmt* body = parse_block(tokens, tokenIdx);
    if (!body) {
        fprintf(stderr, "Error: Failed to parse function body\n");
        return NULL;
    }

    return decl_create(name, func_type, NULL, body, NULL);
} 

struct expr* parse_array_init_list(Token* tokens, int* tokenIdx) {
    struct expr* head = NULL;
    struct expr* current = NULL;

    while (tokens[*tokenIdx].type != TOKEN_RIGHT_BRACE) {
        struct expr* init_expr = parse_expression(tokens, tokenIdx);
        if (!init_expr) return NULL;

        if (!head) {
            head = init_expr;
            current = init_expr;
        } else {
            current->right = init_expr;
            current = init_expr;
        }

        if (tokens[*tokenIdx].type == TOKEN_COMMA) (*tokenIdx)++;
    }

    (*tokenIdx)++;

    return head;
}

struct decl* parse_array(Token* tokens, int* tokenIdx, char* name, struct type* element_type) {
    if (!element_type) {
        fprintf(stderr, "Error: Element type for array is not known\n");
        return NULL;
    }

    struct type* array_type = type_create(TYPE_ARRAY, element_type, NULL);
    if (!array_type) {
        fprintf(stderr, "Error: Unable to create type for array\n");
        return NULL;
    }

    struct expr* size_expr = parse_expression(tokens, tokenIdx);
    if (!size_expr) {
        fprintf(stderr, "Error: Unable to parse expression for array\n");
        type_delete(array_type);
        return NULL;
    }
    size_expr->kind = EXPR_ARRAY_VAL;

    struct expr* array_expr = expr_create(EXPR_ARRAY, size_expr, NULL);
    if (!array_expr) {
        fprintf(stderr, "Error: Unable to create array expression\n");
        type_delete(array_type);
        return NULL;
    }

    array_expr->name = strdup(name);

    (*tokenIdx)++;
    if (tokens[*tokenIdx].type == TOKEN_SEMICOLON) {
        return decl_create(name, array_type, array_expr, NULL, NULL);
    } else if (tokens[*tokenIdx].type == TOKEN_ASSIGNMENT) {
        (*tokenIdx)++;
        if (tokens[*tokenIdx].type == TOKEN_LEFT_BRACE) {
            (*tokenIdx)++;
            array_expr->right = parse_array_init_list(tokens, tokenIdx);
            
            struct expr* current = array_expr->right;
            while (current) {
                current->kind = EXPR_ARRAY_VAL;
                printf("DEGENERATE TREE NODE VALUE: '%d' type: ('%d')\n", current->integer_value, current->kind);
                current = current->right;
            }
        }

        struct decl* d = decl_create(name, array_type, array_expr, NULL, NULL);
        printf("Successfully created array decl with type: %d EXPR KIND: %d and EXPR KIND VAL: %d\n " ,d->type->kind, d->value->kind, d->value->right->kind);

        if (tokens[*tokenIdx].type != TOKEN_SEMICOLON) {
            fprintf(stderr, "Error: Expected semicolon after array initialization\n");
            type_delete(array_type);
            free(array_expr->name);
            free(array_expr);
            free(d);
            return NULL;
        }
        (*tokenIdx)++;

        return d;
    }

    type_delete(array_type);

    return NULL;
}

struct decl* parse_declaration(Token* tokens, int* tokenIdx) {
    type_t kind = get_type(&tokens[*tokenIdx]);
    struct type* type = type_create(kind, NULL, NULL);
    if (!type) {
        perror("Error allocating type in 'parse_declaration()'\n");
        return NULL;
    }

    (*tokenIdx)++;

    if (tokens[*tokenIdx].type != TOKEN_ID) {
        fprintf(stderr, "Error: Expected Identifier\n");
        type_delete(type);
        return NULL;
    }

    char* name = strdup(tokens[*tokenIdx].value.string);
    (*tokenIdx)++;

    struct expr* value = NULL;
    if (tokens[*tokenIdx].type == TOKEN_LEFT_PARENTHESES) {
        (*tokenIdx)++;
        return parse_function(tokens, tokenIdx, name, type);
    } else if (tokens[*tokenIdx].type == TOKEN_LEFT_BRACKET) {
        (*tokenIdx)++;
        return parse_array(tokens, tokenIdx, name, type);
    } else {
        if (tokens[*tokenIdx].type == TOKEN_ASSIGNMENT) {
            (*tokenIdx)++;
            value = parse_expression(tokens, tokenIdx);
        }

        return decl_create(name, type, value, NULL, NULL);
    }
   
    return NULL;
}

struct program* build_ast(Token* tokens) {
    struct program* program = (struct program*)malloc(sizeof(struct program));
    if (program == NULL) {
        perror("Error allocating space for program");
        exit(EXIT_FAILURE);
    }

    cf_stack = create_stmt_t_stack();
    if (!cf_stack) {
        fprintf(stderr, "Error: Failed to create control flow stack\n");
        free(program);
        exit(EXIT_FAILURE);
    }

    int tokenIdx = 0;
    struct decl* head = NULL;
    struct decl* current = NULL;

    while (tokens[tokenIdx].type != TOKEN_EOF) {
        if (tokens[tokenIdx].type == TOKEN_SEMICOLON) {
            tokenIdx++;
            continue;
        }

        struct decl* new_decl = parse_declaration(tokens, &tokenIdx);
        if (!new_decl) {
            fprintf(stderr, "Fatal: Failed to parse declaration\n");
            free(program);
            exit(EXIT_FAILURE);
        }

        if (!head) {
            head = new_decl;
            current = new_decl;
        } else {
            current->next = new_decl;
            current = new_decl;
        }
    }

    free(cf_stack);

    program->declaration = head;
    printf("Program built successfully\n");

    return program;
}

void free_node(struct decl* declaration) {
    if (!declaration) return;

    free(declaration->name);

    if (declaration->type) {
        free(declaration->type->subtype);

        while (declaration->type->params) {
            struct param_list* param_next = declaration->type->params->next;
            free(declaration->type->params->name);
            free(declaration->type->params->type);
            free(declaration->type->params);
            declaration->type->params = param_next;
        }

        free(declaration->type);   
    }   

    if (declaration->value) {
        while (declaration->value->left) {
            struct expr* expr_next = declaration->value->left->left;

            free(declaration->value->left->name);
            free(declaration->value->left->string_literal);
            
            free(declaration->value->left);

            declaration->value->left = expr_next;
        }

        while (declaration->value->right) {
            struct expr* expr_next = declaration->value->right->right;
            
            free(declaration->value->right->name);
            free(declaration->value->right->string_literal);

            free(declaration->value->right);

            declaration->value->right = expr_next;
        }

        free(declaration->value->name);
        free(declaration->value->string_literal);
        free(declaration->value);
    }

    while (declaration->code) {
        struct stmt* stmt_next = declaration->code->next;

        free(declaration->code->decl);
        free(declaration->code->init_expr);
        free(declaration->code->expr);
        free(declaration->code->next_expr);
        free(declaration->code->body);
        free(declaration->code->else_body);
        free(declaration->code);
        declaration->code = stmt_next;
    }

    free(declaration);
}

void free_ast(struct program* root) {
    if (!root) return;

    while (root->declaration) {
        struct decl* next = root->declaration->next;
        free_node(root->declaration);
        root->declaration = next;
    }

    free(root);
}

void print_type(struct type* type, int indent) {
    if (!type) return;
    
    switch(type->kind) {
        case TYPE_INTEGER:
            printf("int");
            break;
        case TYPE_BOOLEAN:
            printf("boolean");
            break;
        case TYPE_CHARACTER:
            printf("char");
            break;
        case TYPE_STRING:
            printf("string");
            break;
        case TYPE_FUNCTION:
            print_type(type->subtype, indent);
            printf(" function(");
            struct param_list* param = type->params;
            while (param) {
                print_type(param->type, indent);
                printf(" %s", param->name);
                if (param->next) printf(", ");
                param = param->next;
            }
            printf(")");
            break;
        case TYPE_ARRAY:
            printf("ARRAY");
            break;
        case TYPE_VOID:
            printf("void");
            break;
        default:
            printf("unknown_type");
    }
}

void print_expr(struct expr* expr, int indent) {
    if (!expr) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    switch(expr->kind) {
        case EXPR_ARRAY:
            printf("ARRAY:\n");
            for (int i = 0; i < indent + 1; i++) printf(" ");
            for (int i = 0; i < indent + 1; i++) printf(" ");
                printf("SIZE:\n");

            if (expr->left) print_expr(expr->left, indent + 2);
            if (expr->right) {
                for (int i = 0; i < indent + 1; i++) printf(" ");
                    printf("INIT VALUES:\n");
                struct expr* current = expr->right;
                while (current) {
                    print_expr(current, indent + 2);
                    current = current->right;
                }
            }
            break;
        case EXPR_NAME:
            printf("NAME: %s\n", expr->name);
            break;
        case EXPR_ARRAY_VAL:
        case EXPR_INTEGER:
            printf("INTEGER: %d\n", expr->integer_value);
            break;
        case EXPR_STRING:
            printf("STRING: %s\n", expr->string_literal);
            break;
        case EXPR_CHARACTER:
            printf("CHAR: '%c'\n", expr->ch_expr);
            break;
        case EXPR_BOOLEAN:
            printf("BOOLEAN: %s\n", expr->integer_value ? "true" : "false");
            break;
        case EXPR_ADD:
            printf("ADD:\n");
            print_expr(expr->left, indent + 1);
            print_expr(expr->right, indent + 1);
            break;
        case EXPR_SUB:
            printf("SUBTRACT:\n");
            print_expr(expr->left, indent + 1);
            print_expr(expr->right, indent + 1);
            break;
        case EXPR_MUL:
            printf("MULTIPLY:\n");
            print_expr(expr->left, indent + 1);
            print_expr(expr->right, indent + 1);
            break;
        case EXPR_DIV:
            printf("DIVIDE:\n");
            print_expr(expr->left, indent + 1);
            print_expr(expr->right, indent + 1);
            break;
        case EXPR_LESS:
            printf("LESS THAN:\n");
            print_expr(expr->left, indent + 1);
            print_expr(expr->right, indent + 1);
            break;
        case EXPR_GREATER:
            printf("GREATER THAN:\n");
            print_expr(expr->left, indent + 1);
            print_expr(expr->right, indent + 1);
            break;
        case EXPR_NOT_EQUAL:
            printf("NOT EQUAL:\n");
            print_expr(expr->left, indent + 1);
            print_expr(expr->right, indent + 1);
            break;
        case EXPR_GREATER_EQUAL:
            printf("GREATER THAN OR EQUALA:\n");
            print_expr(expr->left, indent + 1);
            print_expr(expr->right, indent + 1);
            break;
        case EXPR_LESS_EQUAL:
            printf("LESS THAN OR EQUAL:\n");
            print_expr(expr->left, indent + 1);
            print_expr(expr->right, indent + 1);
            break;
        case EXPR_EQUAL:
            printf("EQUAL TO:\n");
            print_expr(expr->left, indent + 1);
            print_expr(expr->right, indent + 1);
            break;
        case EXPR_ADD_AND_ASSIGN:
            printf("ADD AND ASSIGN:\n");
            print_expr(expr->left, indent + 1);
            print_expr(expr->right, indent + 1);
            break;
        case EXPR_SUB_AND_ASSIGN:
            printf("SUBTRACT AND ASSIGN:\n");
            print_expr(expr->left, indent + 1);
            print_expr(expr->right, indent + 1);
            break;
        case EXPR_MUL_AND_ASSIGN:
            printf("MULTIPLY AND ASSIGN:\n");
            print_expr(expr->left, indent + 1);
            print_expr(expr->right, indent + 1);
            break;
        case EXPR_DIV_AND_ASSIGN:
            printf("DIVIDE AND ASSIGN:\n");
            print_expr(expr->left, indent + 1);
            print_expr(expr->right, indent + 1);
            break;
        case EXPR_ASSIGNMENT:
            printf("ASSIGN:\n");
            print_expr(expr->left, indent + 1);
            print_expr(expr->right, indent + 1);
            break;
        case EXPR_INCREMENT:
            printf("INCREMENT:\n");
            print_expr(expr->left, indent + 1);
            print_expr(expr->right, indent + 1);
            break;
        case EXPR_DECREMENT:
            printf("DECREMENT:\n");
            print_expr(expr->left, indent + 1);
            print_expr(expr->right, indent + 1);
            break;
        case EXPR_SUBSCRIPT:
            printf("ARRAY SUBSCRIPT\n");
            for (int i = 0; i < indent + 1; i++) printf(" ");
            printf("ARRAY\n");
            print_expr(expr->left, indent + 2);
            for (int i = 0; i < indent + 1; i++) printf(" ");
                printf("INDEX\n");
            print_expr(expr->right, indent + 2);
            break;
        default:
            printf("UNKNOWN EXPRESSION TYPE\n");
    }
}

void print_stmt(struct stmt* stmt, int indent) {
    if (!stmt) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    switch(stmt->kind) {
        case STMT_DECL:
            printf("DECLARATION:\n");
            if (stmt->decl) {
                print_decl(stmt->decl, indent + 1);
            }
            break;
        case STMT_EXPR:
            printf("EXPRESSION STATEMENT:\n");
            print_expr(stmt->expr, indent + 1);
            break;
        case STMT_IF:
            printf("IF:\n");
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("CONDITION:\n");
            print_expr(stmt->expr, indent + 2);
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("THEN:\n");
            print_stmt(stmt->body, indent + 2);
            if (stmt->else_body) {
                for (int i = 0; i < indent + 1; i++) printf("  ");
                printf("ELSE:\n");
                print_stmt(stmt->else_body, indent + 2);
            }
            break;
        case STMT_FOR:
            printf("FOR:\n");
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("INIT:\n");
            print_expr(stmt->init_expr, indent + 2);
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("CONDITION:\n");
            print_expr(stmt->expr, indent + 2);
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("INCREMENT:\n");
            print_expr(stmt->next_expr, indent + 2);
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("BODY:\n");
            print_stmt(stmt->body, indent + 2);
            break;
        case STMT_WHILE:
            printf("WHILE:\n");
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("CONDITION:\n");
            print_expr(stmt->expr, indent + 2);
            for (int i = 0; i < indent + 1; i++) printf("  ");
            printf("BODY:\n");
            print_stmt(stmt->body, indent + 2);
            break;
        case STMT_RETURN:
            printf("RETURN:\n");
            if (stmt->expr) {
                print_expr(stmt->expr, indent + 1);
            }
            break;
        default:
            printf("UNKNOWN STATEMENT TYPE\n");
    }
    
    if (stmt->next) {
        print_stmt(stmt->next, indent);
    }
}

void print_decl(struct decl* decl, int indent) {
    if (!decl) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    printf("NAME: %s\n", decl->name);
    for (int i = 0; i < indent; i++) printf("  ");
    printf("TYPE: ");
    print_type(decl->type, indent);
    printf("\n");
    
    if (decl->value) {
        for (int i = 0; i < indent; i++) printf("  ");
        printf("VALUE:\n");
        print_expr(decl->value, indent + 1);
    }
    
    if (decl->code) {
        for (int i = 0; i < indent; i++) printf("  ");
        printf("CODE:\n");
        print_stmt(decl->code, indent + 1);
    }
    
    if (decl->next) {
        print_decl(decl->next, indent);
    }
}

void print_ast(struct program* program) {
    if (!program) {
        printf("Empty program\n");
        return;
    }
    
    printf("PROGRAM:\n");
    print_decl(program->declaration, 1);
}