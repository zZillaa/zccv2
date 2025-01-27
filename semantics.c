#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include <assert.h>

static struct decl* current_function = NULL;
struct symbol* create_symbol(symbol_t kind, struct type* type, char* name) {
	struct symbol* symbol = malloc(sizeof(struct symbol));
	if (!symbol) return NULL;

	symbol->kind = kind;
	symbol->type = type_copy(type);
	symbol->name = strdup(name);
	if (!symbol->name) {
		fprintf(stderr, "Failed to allocate memory for symbol name\n");
		free(symbol->type);
		free(symbol);
		return NULL;
	}
	printf("\nSymbol creation successful | name: '%s' | type : %d\n", symbol->name, symbol->type->kind);

	return symbol;
}

struct stack* create_stack() {
	struct stack* stack = malloc(sizeof(struct stack));

	if (!stack) {
		fprintf(stderr, "Failed to allocate memory for stack\n");
		return NULL;
	}

	stack->top = -1;
	stack->symbol_tables = malloc(sizeof(struct symbol_table*) * STACK_SIZE);
	if (!stack->symbol_tables) {
		fprintf(stderr, "Failed to allocate memory for stack symbol tables\n");
		free(stack);
		return NULL;
	} 

	return stack;
}

struct symbol_table* create_symbol_table() {
	struct symbol_table* symbol_table = malloc(sizeof(struct symbol_table));

	if (!symbol_table) return NULL;

	symbol_table->symbol = NULL;
	symbol_table->next = NULL;

	return symbol_table;
}

void scope_enter(struct stack* stack, struct symbol* symbol) {
	if (!stack) {
		fprintf(stderr, "Error: Invalid stack pointer\n");
		return;
	}
	if (stack->top == STACK_SIZE - 1) {
		fprintf(stderr, "Error: stack overflow - maximum scope depth exceeded\n");
		return;
	}

	struct symbol_table* new_table = create_symbol_table();
	if (!new_table) {
		fprintf(stderr, "Failed to allocate memory for new table\n");
		return;
	}

	stack->top++;
	stack->symbol_tables[stack->top] = new_table;
	new_table->symbol = symbol;
	printf("\nEntering new scope (level %d)\n", stack->top);
}

bool is_empty(struct stack* stack) {
	return stack->top == -1;
}

void scope_exit(struct stack* stack) {
	if (is_empty(stack)) {
		printf("stack is empty\n");
		return;
	}
	
	// struct symbol_table* current_table = stack->symbol_tables[stack->top];
	// if (current_table) {
	// 	struct symbol* current = current_table->symbol;
	// 	while (current) {
	// 		struct symbol* temp = current;
	// 		current = current->next;
	// 		free_symbol(temp);
	// 	}
	// 	free(current_table);
	// }
	// struct symbol_table* current_table = stack->symbol_tables[stack->top];
	// stack->symbol_tables[stack->top] = NULL;
	stack->top--;

	// if (current_table) {
	// 	free(current_table);
	// }
}


void insert_symbol(struct symbol_table* table, struct symbol* symbol) {
	if (!table || !symbol) return;

	symbol->next = table->symbol;
	table->symbol = symbol;

}

int scope_level(struct stack* stack) {
	return stack->top + 1;
}

struct symbol_table* copy_symbol_table(struct symbol_table* original) {
	if (!original) return NULL;

	struct symbol_table* copy = create_symbol_table();

	struct symbol* current = original->symbol;
	struct symbol* prev_copy = NULL;

	while (current) {
		struct symbol* symbol_copy = create_symbol(
			current->kind, 
			current->type, 
			strdup(current->name));

		if (!symbol_copy) {
			free(copy->symbol);
			return NULL;
		}


		if (prev_copy) {
			prev_copy->next = symbol_copy;
		} else {
			copy->symbol = symbol_copy;
		}


		prev_copy = symbol_copy;
		current = current->next;
	}

	return copy;
}

bool is_symbol_redeclared(struct symbol_table* table, char* name) {
	struct symbol* symbol = table->symbol;
	while (symbol) {
		if (strcmp(symbol->name, name) == 0) {
			return true;
		}
		symbol = symbol->next;
	}

	return false;
}

struct symbol* scope_lookup(struct stack* stack, char* name, int* found_scope) {
	if (!stack || stack->top < 0) {
		fprintf(stderr, "Error: Invalid stack state in scope_lookup\n");
		return NULL;
	}

	for (int i = stack->top; i >=0; i--) {
		if (!stack->symbol_tables[i]) {
			fprintf(stderr, "Error: Null symbol table at level %d\n", i);
			continue;
		}

		struct symbol* current = stack->symbol_tables[i]->symbol;
		while (current) {
			if (strcmp(current->name, name) == 0) {
				if (found_scope) *found_scope = i;
				return current;
			}
			current = current->next;
		}
	}
	return NULL;
}

struct symbol* scope_lookup_current(struct stack* stack, char* name) {
	struct symbol* current = stack->symbol_tables[stack->top]->symbol;
	while (current) {
		if (strcmp(current->name, name) == 0) {
			printf("Found '%s' in '(%d)'\n", name, stack->top);
			return current;
		}
		current = current->next;
	}
	return NULL;
}


void scope_bind(struct stack* stack, struct symbol* symbol) {
	struct symbol_table* current_table = stack->symbol_tables[stack->top];
	if (!current_table) return;

	if (is_symbol_redeclared(current_table, symbol->name)) {
		fprintf(stderr , "Error: Symbol '%s' redeclared in the name scope\n", symbol->name);
		return;
	}

	symbol->next = current_table->symbol;
	current_table->symbol = symbol;

	printf("Successfully bound Symbol '%s' to '(%d)' with type: %d\n", symbol->name, stack->top, symbol->type->kind);
}

void param_list_resolve(struct param_list* params, struct stack* stack) {
	if (!params) return;

	while (params) {
		symbol_t kind = SYMBOL_PARAM;
		params->symbol = create_symbol(kind, params->type, params->name);

		if (params->symbol) {
			scope_bind(stack, params->symbol);
		}
		params = params->next;
	}
}

void expr_resolve(struct expr* expr, struct stack* stack) {
    if (!expr) return;
    printf("In expr resolve\n");
    switch (expr->kind) {
        case EXPR_NAME: {
        	int found_scope = -1;
        	struct symbol* symbol = scope_lookup(stack, expr->name, &found_scope);
        	if (!symbol) {
        		fprintf(stderr, "Error: undefined symbol '%s'\n", expr->name);
        	} else {
        		printf("Resolved '%s' at %p (scope %d)\n", expr->name, (void*)symbol, found_scope);
        	}
            expr->symbol = symbol;
            break;
        }

    	case EXPR_INCREMENT:
    	case EXPR_DECREMENT: {
    		if (expr->left) expr_resolve(expr->left, stack);
    		break;
    	}

    	case EXPR_LESS:
    	case EXPR_GREATER:
    	case EXPR_LESS_EQUAL:
    	case EXPR_GREATER_EQUAL:
    	case EXPR_EQUAL:
    	case EXPR_NOT_EQUAL: {
    		if (expr->left) expr_resolve(expr->left, stack);
    		if (expr->right) expr_resolve(expr->right, stack);
    		break;
    	}

        case EXPR_ASSIGNMENT:
        case EXPR_ADD_AND_ASSIGN:
        case EXPR_SUB_AND_ASSIGN:
        case EXPR_MUL_AND_ASSIGN:
        case EXPR_DIV_AND_ASSIGN: {
            // Resolve the left and right parts of the assignment expression
            if (expr->left) expr_resolve(expr->left, stack);
            if (expr->right) expr_resolve(expr->right, stack);
            break;
        }
        case EXPR_CALL: {
            // Resolve the function arguments
            if (expr->left) {
                expr_resolve(expr->left, stack);
            }
            struct expr* arg = expr->right;
            while (arg) {
                expr_resolve(arg, stack);
                arg = arg->right;
            }
            break;
        }
        case EXPR_INTEGER:
        case EXPR_CHARACTER:
        case EXPR_BOOLEAN:
        case EXPR_STRING: {
            // For literals, no need for resolution
            break;
        }
        default:
            // Handle other expression types if necessary
            break;
    }
}

void debug_print_scope_stack(struct stack* stack, const char* location) {
    printf("\n=== Scope Stack Debug at %s ===\n", location);
    printf("Current stack top: %d\n", stack->top);
    
    for (int i = 0; i <= stack->top; i++) {
        printf("Scope level %d:\n", i);
        if (!stack->symbol_tables[i]) {
            printf("  WARNING: Null symbol table at level %d\n", i);
            continue;
        }
        
        struct symbol* sym = stack->symbol_tables[i]->symbol;
        int count = 0;
        while (sym) {
            printf("  Symbol %d: %s (type: %d)\n", count++, sym->name, sym->type->kind);
            sym = sym->next;
        }
    }
    printf("=====================================\n\n");
}

void stmt_resolve(struct stmt* stmt, struct stack* stack) {
	if (!stmt) return;

	debug_print_scope_stack(stack, "Start of stmt_resolve");

	while (stmt) {
		switch(stmt->kind) {
			case STMT_DECL:
				if (stmt->decl) {
					decl_resolve(stmt->decl, stack);
				}
				break;

			case STMT_EXPR:
				if (stmt->expr) {
					expr_resolve(stmt->expr, stack);
				}
				break;

			case STMT_IF_ELSE:
			case STMT_IF:
				if (stmt->expr) {
					expr_resolve(stmt->expr, stack);
				}

				if (stmt->body) {
					scope_enter(stack, NULL);
					stmt_resolve(stmt->body, stack);
					scope_exit(stack);
				}

				if (stmt->else_body) {
					scope_enter(stack, NULL);
					stmt_resolve(stmt->else_body, stack);
					scope_exit(stack);
				}
				break;

			case STMT_WHILE:
			case STMT_FOR:
				scope_enter(stack, NULL);

				if (stmt->decl) decl_resolve(stmt->decl, stack);
				else if (stmt->init_expr) expr_resolve(stmt->init_expr, stack);

				if (stmt->expr) expr_resolve(stmt->expr, stack);
				if (stmt->next_expr) expr_resolve(stmt->next_expr, stack);

				if (stmt->body) stmt_resolve(stmt->body, stack);
				scope_exit(stack);

				break;

			case STMT_BLOCK:
				if (stmt->body) {
					stmt_resolve(stmt->body, stack);
				}
				break;

			case STMT_RETURN:
				if (stmt->expr) {
					printf("I got to return statement\n");
					
					expr_resolve(stmt->expr, stack);
					debug_print_scope_stack(stack, "After return expr resolve");
				}
				break;
			default:
				fprintf(stderr, "Failed to recognize this kind of statement\n");
				break;
		}

		stmt = stmt->next;
	}
	debug_print_scope_stack(stack, "End of stmt_resolve");
}


void decl_resolve(struct decl* d, struct stack* stack) {
	if (!d || !stack) return;

	while (d) {
		if (!d->name || !d->type) {
			fprintf(stderr, "Error: Invalid declaration (missing name or type)\n");
			d = d->next;
			continue;
		}
		
		symbol_t kind = scope_level(stack) > 1 ? SYMBOL_LOCAL : SYMBOL_GLOBAL;
		struct symbol* existing_symbol = scope_lookup_current(stack, d->name);

		if (existing_symbol) {
			fprintf(stderr, "Error: Variable '%s' redeclared in the same scope\n", d->name);
			d->symbol = existing_symbol;
			d = d->next;
			continue;
		}

		d->symbol = create_symbol(kind, d->type, d->name);
		if (!d->symbol) {
			fprintf(stderr, "Error: Failed to create symbol for '%s'\n", d->name);
			d = d->next;
			continue;
		}

		scope_bind(stack, d->symbol);


		if (d->value) {
			expr_resolve(d->value, stack);
		}
		
		if (d->type->kind == TYPE_FUNCTION) {
			current_function = d;
			int original_scope = stack->top;

			scope_enter(stack, NULL);
			param_list_resolve(d->type->params, stack);
			
			scope_enter(stack, NULL);
			stmt_resolve(d->code, stack);
			
			while (stack->top > original_scope) {
				scope_exit(stack);
			}

			current_function = NULL;
		}
	
		d = d->next;
	}
}

void free_symbol(struct symbol* symbol) {
	if (!symbol) return;

	free(symbol->name);
	if (symbol->type) type_delete(symbol->type);
	free(symbol);
}

void free_stack(struct stack* stack) {
	if (!stack) return;

	for (int i = stack->top; i >= 0; i--) {
		struct symbol_table* table = stack->symbol_tables[i];
		struct symbol* current = table->symbol;
		while (current) {
			struct symbol* temp = current;
			current = current->next;
			free_symbol(temp);
		}
		free(table);
	}
	free(stack->symbol_tables);
	free(stack);
}

void print_symbol_table(struct stack* stack) {
	for (int i = stack->top; i >= 0; i--) {
		printf("Scope level: %d\n", i);
		struct symbol* current = stack->symbol_tables[i]->symbol;
		while (current) {
			printf("   Symbol: %s, Type: %d\n", current->name, current->type->kind);
			current = current->next;
		}
	}
}

void program_resolve(struct program* p, struct stack* stack) {
	if (!p) return;

	if (p->declaration) decl_resolve(p->declaration, stack);

}

bool type_equals(struct type* a, struct type* b) {
	if (!a || !b) return false;

	if (a->kind == b->kind) {
		switch (a->kind) {
			case TYPE_INTEGER:
			case TYPE_CHARACTER:
			case TYPE_BOOLEAN:
			case TYPE_VOID:
				return true;

			case TYPE_FUNCTION:
				struct param_list* a_params = a->params;
				struct param_list* b_params = b->params;

				while (a_params && b_params) {
					if (!type_equals(a_params->type, b_params->type))
						return false;

					a_params = a_params->next;
					b_params = b_params->next;
				}

				return (a_params == NULL && b_params == NULL) && type_equals(a->subtype, b->subtype);

			default:
				return false;
		}
	}
}

struct type* type_create(type_t kind, struct type* subtype, struct param_list* params) {
	struct type* t = malloc(sizeof(struct type));
	if (!t) {
		fprintf(stderr, "Error: Memory allocation failed in type_create\n");
		return NULL;
	}

	t->kind = kind;
	t->subtype = subtype;
	t->params = params;

	return t;
}

struct type* type_copy(struct type* t) {
	if (!t) return NULL;

	struct type* t_copy = malloc(sizeof(struct type));
	if (!t_copy) return NULL;

	t_copy->kind = t->kind;
	t_copy->subtype = t->subtype ? type_copy(t->subtype) : NULL;

	if (t->params) {
		struct param_list* current = t->params;
		struct param_list* prev_copy = NULL;
		struct param_list* first_copy = NULL;

		while (current) {
			struct param_list* param_copy = malloc(sizeof(struct param_list));
			param_copy->name = strdup(current->name);
			param_copy->type = type_copy(current->type);
			param_copy->next = NULL;

			if (prev_copy) {
				prev_copy->next = param_copy;
			} else {
				first_copy = param_copy;
			}

			prev_copy = param_copy;
			current = current->next;
		}

		t_copy->params = first_copy;
	} else {
		t_copy->params = NULL;
	}

	return t_copy;
}

void type_delete(struct type* t) {
	if (!t) return;

	struct param_list* param = t->params;
	while (param) {
		free(param->name);
		type_delete(param->type);
		free(param);
		param = param->next;
	}	

	type_delete(t->subtype);

	free(t);
}

struct type* expr_typecheck(struct expr* e, struct stack* stack) {
    if (!e) return NULL;

    if (e->kind == EXPR_NAME && !e->symbol) {
    	fprintf(stderr, "Warning: Attempting to typecheck unresolved symbol\n");
    	return type_create(TYPE_UNKNOWN, NULL, NULL);
    }
    struct type* lt = e->left ? expr_typecheck(e->left, stack) : NULL;
    struct type* rt = e->right ? expr_typecheck(e->right, stack) : NULL;
    struct type* result = NULL;

    switch (e->kind) {
    	case EXPR_INCREMENT:
    	case EXPR_DECREMENT:
    		if (!e->left) {
    			fprintf(stderr, "Error: Invalid increment/decrement operation\n");
    			result = type_create(TYPE_UNKNOWN, NULL, NULL);
    		} else if (lt->kind != TYPE_INTEGER) {
    			fprintf(stderr, "Error: Increment/decrement requires integer type\n");
    			result = type_create(TYPE_UNKNOWN, NULL, NULL);
    		} else {
    			result = type_create(TYPE_INTEGER, NULL, NULL);
    		}
    		break;

    	case EXPR_INTEGER:
    		result = type_create(TYPE_INTEGER, NULL, NULL);
    		break; 

        case EXPR_NAME:
            if (!e->symbol) {
            	fprintf(stderr, "Error: Symbol '%s' unresolved\n", e->name);
            	return type_create(TYPE_UNKNOWN, NULL, NULL);
            }
            result = type_copy(e->symbol->type);
            break;

        case EXPR_LESS:
        case EXPR_GREATER:
        case EXPR_LESS_EQUAL:
        case EXPR_GREATER_EQUAL:
        case EXPR_EQUAL:
        case EXPR_NOT_EQUAL:
            if (!lt || !rt || lt->kind != TYPE_INTEGER || rt->kind != TYPE_INTEGER) {
            	printf("lt type: %d, rt type: %d\n", lt->kind, rt->kind);
                fprintf(stderr, "Error: Comparison requires integer types\n");
                result = type_create(TYPE_UNKNOWN, NULL, NULL);
            } else {
                result = type_create(TYPE_BOOLEAN, NULL, NULL);
            }
            break;

        case EXPR_ADD:
        case EXPR_SUB:
        case EXPR_MUL:
        case EXPR_DIV:
        	if (!lt || !rt || lt->kind != TYPE_INTEGER || rt->kind != TYPE_INTEGER) {
        		fprintf(stderr, "Error: Arithmetic operations require integer types\n");
        		result = type_create(TYPE_UNKNOWN, NULL, NULL);
        	} else {
        		result = type_create(TYPE_INTEGER, NULL, NULL);
        	}
        	break;

        case EXPR_ASSIGNMENT:
        	if (!lt || !rt || !type_equals(lt, rt)) {
        		fprintf(stderr, "Error: Type mismatch in assignment\n");
        		result = type_create(TYPE_UNKNOWN, NULL, NULL);
        	} else {
        		result = type_copy(lt);
        	}
        	break;

        default:
        	fprintf(stderr, "Unhandled expression type in typecheck: %d\n", e->kind);
        	result = type_create(TYPE_UNKNOWN, NULL, NULL);
        	break;

    }

    type_delete(lt);
    type_delete(rt);
    return result;
}

void stmt_typecheck(struct stmt* s, struct stack* stack) {
    if (!s) return;

    while (s) {
        switch (s->kind) {
        	case STMT_DECL:
        		if (s->decl) {
        			decl_typecheck(s->decl, stack);
        		}
        		break;
        	case STMT_BLOCK:
        		stmt_typecheck(s->body, stack);
        		break;

            case STMT_EXPR:
                if (s->expr) {
                    struct type* t = expr_typecheck(s->expr, stack);
                    type_delete(t);
                }
                break;

            case STMT_IF:
            case STMT_IF_ELSE:

                if (s->expr) {
                    struct type* t = expr_typecheck(s->expr, stack);
                    if (!t || t->kind != TYPE_BOOLEAN) {
                        fprintf(stderr, "Error: Condition must be boolean type\n");
                    }
                    type_delete(t);
                }

                if (s->body) {
                    stmt_typecheck(s->body, stack);
                }

                if (s->else_body) {
                    stmt_typecheck(s->else_body, stack);
                }
                break;

            case STMT_FOR:
            	
                if (s->init_expr) {
                	struct type* t_init = expr_typecheck(s->init_expr, stack);
                    type_delete(t_init);
                }
                
                if (s->expr) {
                    struct type* t_cond = expr_typecheck(s->expr, stack);
                    if (!t_cond || t_cond->kind != TYPE_BOOLEAN) {
                        fprintf(stderr, "Error: Loop condition must be boolean type\n");
                    }
                    type_delete(t_cond);
                }
                
                if (s->next_expr) {

                	struct type* t_next = expr_typecheck(s->next_expr, stack);
                	type_delete(t_next);
                }
                
                if (s->body) {
                    stmt_typecheck(s->body, stack);
                }
                break;

            case STMT_RETURN:
            	if (!current_function) {
            		fprintf(stderr, "Error: Return statement outside function\n");
            		break;
            	}
               	
               	if (!current_function->type || !current_function->type->subtype) {
               		fprintf(stderr, "Error: Invalid function return type\n");
               		break;
               	}

               	if (s->expr) {
               		struct type* return_type = expr_typecheck(s->expr, stack);
               		if (current_function->type->subtype->kind == TYPE_VOID) {
               			fprintf(stderr, "Error: Void function cannot return a value\n");
               		} else if (!return_type || !type_equals(return_type, current_function->type->subtype)) {
               			fprintf(stderr, "Error: Return type mismatch in funciton '%s'\n", current_function->name);
               		}

               		type_delete(return_type);

               	} else {
               		if (current_function->type->subtype->kind != TYPE_VOID) {
               			fprintf(stderr, "Error: Non-void function missing a return value\n");
               		}
               	}

                break;

            default:
                fprintf(stderr, "Error: Unhandled statement type in type checking\n");
                break;
        }

        s = s->next;
    }
}

void decl_typecheck(struct decl* d, struct stack* stack) {
	if (!d) return;

	while (d) {
		if (!d->type) {
			fprintf(stderr, "Error: Declaration missing type\n");
			d = d->next;
			continue;
		}

		if (d->type->kind == TYPE_FUNCTION) {
			current_function = d;

			if (d->code) {
				stmt_typecheck(d->code, stack);
			}
			
			current_function = NULL;
		} else {
			if (d->value) {
				struct type* t = expr_typecheck(d->value, stack);
				if (!type_equals(t, d->symbol->type)) {
					fprintf(stderr, "Error: Type mismatch in declaration\n");
				}
				type_delete(t);
			}
			if (d->code) {
				stmt_typecheck(d->code, stack);
			}

		}
		d = d->next;
	}
}

void program_typecheck(struct program* p, struct stack* stack) {
	if (!p) return;

	debug_print_scope_stack(stack, "Start of program_typecheck");

	if (p->declaration) decl_typecheck(p->declaration, stack);

	debug_print_scope_stack(stack, "End of program_typecheck");
}