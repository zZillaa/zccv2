// #include <string.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include "ast.h"
// #include <assert.h>

// static struct decl* current_function = NULL;
// struct symbol* create_symbol(symbol_t kind, struct type* type, char* name) {
// 	struct symbol* symbol = malloc(sizeof(struct symbol));
// 	if (!symbol) return NULL;

// 	symbol->kind = kind;
// 	symbol->type = type_copy(type);
// 	symbol->name = strdup(name);
// 	if (!symbol->name) {
// 		fprintf(stderr, "Failed to allocate memory for symbol name\n");
// 		free(symbol->type);
// 		free(symbol);
// 		return NULL;
// 	}
// 	// printf("\nSymbol creation successful | name: '%s' | type : %d\n", symbol->name, symbol->type->kind);
// 	// printf("Creating symbol %s with kind %d and index %d\n", name, kind, symbol->u.local_var_index);
// 	return symbol;
// }

// struct stack* create_stack() {
// 	struct stack* stack = malloc(sizeof(struct stack));
// 	if (!stack) {
// 		fprintf(stderr, "Failed to allocate memory for stack\n");
// 		return NULL;
// 	}

// 	stack->top = -1;
// 	stack->symbol_tables = malloc(sizeof(struct symbol_table*) * STACK_SIZE);
// 	if (!stack->symbol_tables) {
// 		fprintf(stderr, "Failed to allocate memory for stack symbol tables\n");
// 		free(stack);
// 		return NULL;
// 	} 

// 	return stack;
// }

// struct symbol_table* create_symbol_table() {
// 	struct symbol_table* symbol_table = malloc(sizeof(struct symbol_table));

// 	if (!symbol_table) return NULL;

// 	symbol_table->symbol = NULL;
// 	symbol_table->next = NULL;

// 	return symbol_table;
// }

// void scope_enter(struct stack* stack, struct symbol* symbol) {
// 	if (!stack) {
// 		fprintf(stderr, "Error: Invalid stack pointer\n");
// 		return;
// 	}
// 	if (stack->top == STACK_SIZE - 1) {
// 		fprintf(stderr, "Error: stack overflow - maximum scope depth exceeded\n");
// 		return;
// 	}

// 	struct symbol_table* new_table = create_symbol_table();
// 	if (!new_table) {
// 		fprintf(stderr, "Failed to allocate memory for new table\n");
// 		return;
// 	}

// 	stack->top++;
// 	stack->symbol_tables[stack->top] = new_table;
// 	new_table->symbol = symbol;
// 	printf("\nEntering new scope (level %d)\n", stack->top);
// }

// bool is_empty(struct stack* stack) {
// 	return stack->top == -1;
// }

// void scope_exit(struct stack* stack) {
// 	if (is_empty(stack)) {
// 		printf("stack is empty\n");
// 		return;
// 	}
	
// 	stack->top--;
// }

// void insert_symbol(struct symbol_table* table, struct symbol* symbol) {
// 	if (!table || !symbol) return;

// 	symbol->next = table->symbol;
// 	table->symbol = symbol;

// }

// int scope_level(struct stack* stack) {
// 	return stack->top + 1;
// }

// struct symbol_table* copy_symbol_table(struct symbol_table* original) {
// 	if (!original) return NULL;

// 	struct symbol_table* copy = create_symbol_table();

// 	struct symbol* current = original->symbol;
// 	struct symbol* prev_copy = NULL;

// 	while (current) {
// 		struct symbol* symbol_copy = create_symbol(
// 			current->kind, 
// 			current->type, 
// 			strdup(current->name));

// 		if (!symbol_copy) {
// 			free(copy->symbol);
// 			return NULL;
// 		}


// 		if (prev_copy) {
// 			prev_copy->next = symbol_copy;
// 		} else {
// 			copy->symbol = symbol_copy;
// 		}


// 		prev_copy = symbol_copy;
// 		current = current->next;
// 	}

// 	return copy;
// }

// bool is_symbol_redeclared(struct symbol_table* table, char* name) {
// 	struct symbol* symbol = table->symbol;
// 	while (symbol) {
// 		if (strcmp(symbol->name, name) == 0) {
// 			return true;
// 		}
// 		symbol = symbol->next;
// 	}

// 	return false;
// }

// struct symbol* scope_lookup(struct stack* stack, char* name, int* found_scope) {
// 	if (!name) {
// 		fprintf(stderr, "Error: Invalid name\n");
// 		return NULL;
// 	}

// 	if (!stack || stack->top < 0) {
// 		fprintf(stderr, "Error: Invalid stack state in scope_lookup\n");
// 		return NULL;
// 	}

// 	for (int i = stack->top; i >=0; i--) {
// 		if (!stack->symbol_tables[i]) {
// 			fprintf(stderr, "Error: Null symbol table at level %d\n", i);
// 			continue;
// 		}
// 		struct symbol* current = stack->symbol_tables[i]->symbol;
// 		while (current) {
// 			if (strcmp(current->name, name) == 0) {
// 				if (found_scope) *found_scope = i;
// 				return current;
// 			}
// 			current = current->next;
// 		}
// 	}
// 	return NULL;
// }

// struct symbol* scope_lookup_current(struct stack* stack, char* name) {
// 	struct symbol* current = stack->symbol_tables[stack->top]->symbol;
// 	while (current) {
// 		if (strcmp(current->name, name) == 0) {
// 			printf("Found '%s' in '(%d)'\n", name, stack->top);
// 			return current;
// 		}
// 		current = current->next;
// 	}
// 	return NULL;
// }


// void scope_bind(struct stack* stack, struct symbol* symbol) {
// 	struct symbol_table* current_table = stack->symbol_tables[stack->top];
// 	if (!current_table) return;

// 	// if (is_symbol_redeclared(current_table, symbol->name)) {
// 	// 	fprintf(stderr , "Error: Symbol '%s' redeclared in the name scope\n", symbol->name);
// 	// 	return;
// 	// }

// 	symbol->next = current_table->symbol;
// 	current_table->symbol = symbol;

// 	printf("Successfully bound Symbol '%s' to Scope level: '(%d)' with type: %d\n", symbol->name, stack->top, symbol->type->kind);
// }

// void param_list_resolve(struct param_list* params, struct stack* stack) {
// 	if (!params) return;

// 	static int param_index = 0;

// 	while (params) {
// 		symbol_t kind = SYMBOL_PARAM;
// 		params->symbol = create_symbol(kind, params->type, params->name);
// 		params->symbol->s.param_index = param_index++;

// 		if (params->symbol) {
// 			scope_bind(stack, params->symbol);
// 		}
// 		params = params->next;
// 	}
// }

// void expr_resolve(struct expr* expr, struct stack* stack) {
//     if (!expr) return;

//     int found_scope = -1;

//     switch (expr->kind) {
//         case EXPR_NAME: {
//         	struct symbol* symbol = scope_lookup(stack, expr->name, &found_scope);
//         	if (!symbol) {
//         		fprintf(stderr, "Error: undefined symbol '%s'\n", expr->name);
//         	} 
//         	printf("Resolved '%s' at %p (scope %d)\n", expr->name, (void*)symbol, found_scope);
//             expr->symbol = symbol;
//             break;
//         }

//     	case EXPR_ARRAY: {
//     		struct symbol* symbol = scope_lookup(stack, expr->name, &found_scope);
//     		if (!symbol) {
//     			fprintf(stderr, "Error: Undefined symbol '%s'\n", expr->name);
//     		} 
//     		// else {
//     		// 	if (expr->left) expr_resolve(expr->left, stack);

//     			// if (expr->right) {
// 	    		// 	struct expr* current = expr->right;
// 	    		// 	while (current) {
// 	    		// 		expr_resolve(current, stack);
// 	    		// 		current = current->right;
// 	    		// 	}

//     			// } else {
//     			// 	fprintf(stderr, "Error: array does not have initial values\n");
//     			// }
//     		// }
//     		expr->symbol = symbol;
//     		break;
//     	} 

//     	case EXPR_INCREMENT:
//     	case EXPR_DECREMENT: {
//     		if (expr->left) expr_resolve(expr->left, stack);
//     		break;
//     	}

//     	case EXPR_LESS:
//     	case EXPR_GREATER:
//     	case EXPR_LESS_EQUAL:
//     	case EXPR_GREATER_EQUAL:
//     	case EXPR_EQUAL:
//     	case EXPR_NOT_EQUAL: {
//     		if (expr->left) expr_resolve(expr->left, stack);
//     		if (expr->right) expr_resolve(expr->right, stack);
//     		break;
//     	}

//         case EXPR_ASSIGNMENT:
//         case EXPR_ADD_AND_ASSIGN:
//         case EXPR_SUB_AND_ASSIGN:
//         case EXPR_MUL_AND_ASSIGN:
//         case EXPR_DIV_AND_ASSIGN: {
//             // Resolve the left and right parts of the assignment expression
//             if (expr->left) expr_resolve(expr->left, stack);
//             if (expr->right) expr_resolve(expr->right, stack);
//             break;
//         }
//         case EXPR_CALL: {
//             // Resolve the function arguments
//             if (expr->left) {
//                 expr_resolve(expr->left, stack);
//             }
//             struct expr* arg = expr->right;
//             while (arg) {
//                 expr_resolve(arg, stack);
//                 arg = arg->right;
//             }
//             break;
//         }

//     	case EXPR_ARRAY_VAL:
//     		printf("Here is the array value: %d\n", expr->integer_value);
//         	break;
//         case EXPR_INTEGER:
//         	printf("No need to resolve: '%d'\n", expr->integer_value);
//         	break;
//         case EXPR_CHARACTER:
//         case EXPR_BOOLEAN:
//         case EXPR_STRING: 
//             break;
//         default:
//             break;
//     }
// }

// void debug_print_scope_stack(struct stack* stack, const char* location) {
//     printf("\n=== Scope Stack Debug at %s ===\n", location);
//     printf("Current stack top: %d\n", stack->top);
    
//     for (int i = 0; i <= stack->top; i++) {
//         printf("Scope level %d:\n", i);
//         if (!stack->symbol_tables[i]) {
//             printf("  WARNING: Null symbol table at level %d\n", i);
//             continue;
//         }
        
//         struct symbol* sym = stack->symbol_tables[i]->symbol;
//         int count = 0;
//         while (sym) {
//             printf("  Symbol %d: %s (type: %d)\n", count++, sym->name, sym->type->kind);
//             sym = sym->next;
//         }
//     }
//     printf("=====================================\n\n");
// }

// void stmt_resolve(struct stmt* stmt, struct stack* stack) {
// 	if (!stmt) return;

// 	debug_print_scope_stack(stack, "Start of stmt_resolve");

// 	while (stmt) {
// 		switch(stmt->kind) {
// 			case STMT_DECL:
// 				if (stmt->decl) {
// 					decl_resolve(stmt->decl, stack);
// 				}
// 				break;

// 			case STMT_EXPR:
// 				if (stmt->expr) {
// 					expr_resolve(stmt->expr, stack);
// 				}
// 				break;

// 			case STMT_IF_ELSE:
// 			case STMT_IF:
// 				if (stmt->expr) {
// 					expr_resolve(stmt->expr, stack);
// 				}

// 				if (stmt->body) {
// 					scope_enter(stack, NULL);
// 					stmt_resolve(stmt->body, stack);
// 					scope_exit(stack);
// 				}

// 				if (stmt->else_body) {
// 					scope_enter(stack, NULL);
// 					stmt_resolve(stmt->else_body, stack);
// 					scope_exit(stack);
// 				}
// 				break;

// 			case STMT_WHILE:
// 			case STMT_FOR:
// 				scope_enter(stack, NULL);

// 				if (stmt->decl) decl_resolve(stmt->decl, stack);
// 				else if (stmt->init_expr) expr_resolve(stmt->init_expr, stack);

// 				if (stmt->expr) expr_resolve(stmt->expr, stack);
// 				if (stmt->next_expr) expr_resolve(stmt->next_expr, stack);

// 				if (stmt->body) stmt_resolve(stmt->body, stack);
// 				scope_exit(stack);

// 				break;

// 			case STMT_BLOCK:
// 				if (stmt->body) {
// 					scope_enter(stack, NULL);
// 					stmt_resolve(stmt->body, stack);
// 					scope_exit(stack);
// 				}
// 				break;

// 			case STMT_RETURN:
// 				if (stmt->expr) {					
// 					expr_resolve(stmt->expr, stack);
// 					debug_print_scope_stack(stack, "After return expr resolve\n");
// 				}
// 				break;
// 			default:
// 				fprintf(stderr, "Failed to recognize this kind of statement\n");
// 				break;
// 		}

// 		stmt = stmt->next;
// 	}
// 	debug_print_scope_stack(stack, "End of stmt_resolven\n");
// }


// size_t get_num_bytes(struct decl* d, struct type* t) {
// 	if (!t || !d) return 0;

// 	if (d->type) {
// 		type_t kind = d->type->kind;
// 		switch (kind) {
// 			case TYPE_INTEGER:
// 			return sizeof(int);

// 			case TYPE_CHARACTER:
// 			case TYPE_BOOLEAN:
// 				return sizeof(char);

// 			case TYPE_ARRAY:
// 				size_t element_size;
// 				if (d->type->subtype->kind == TYPE_INTEGER) {
// 					element_size = sizeof(int);
// 				} else if (d->type->subtype->kind == TYPE_CHARACTER ||
// 						d->type->subtype->kind == TYPE_BOOLEAN) {
// 					element_size = sizeof(char);
// 				} else {
// 					return 0;
// 				}

// 				int array_size = 0;
// 				if (d->value && d->value->left) {
// 					array_size = d->value->left->integer_value;
// 				}
				
// 				return element_size * (array_size > 0 ? array_size : 0);

// 			default:
// 				return 0;
// 		}
// 	}

// 	return 0;

// }

// void decl_resolve(struct decl* d, struct stack* stack) {
// 	if (!d || !stack) return;
// 	static int local_var_counter = 0;

// 	if (d->type && d->type->kind == TYPE_FUNCTION) {
// 		local_var_counter = 0;
// 	}

// 	while (d) {
// 		if (!d->name || !d->type) {
// 			fprintf(stderr, "Error: Invalid declaration (missing name or type)\n");
// 			d = d->next;
// 			continue;
// 		}

// 		symbol_t kind = scope_level(stack) > 1 ? SYMBOL_LOCAL : SYMBOL_GLOBAL;
// 		struct symbol* existing_symbol = scope_lookup_current(stack, d->name);
// 		if (existing_symbol) {
// 			fprintf(stderr, "Error: Variable '%s' redeclared in the same scope\n", d->name);
// 			d->symbol = existing_symbol;
// 			d = d->next;
// 			continue;
// 		}
		
// 		size_t bytes = get_num_bytes(d, d->type);
		
// 		d->symbol = create_symbol(kind, d->type, d->name);
// 		if (!d->symbol) {
// 			fprintf(stderr, "Error: Failed to create symbol for '%s'\n", d->name);
// 			d = d->next;
// 			continue;
// 		}

// 		if (kind == SYMBOL_LOCAL) {
// 			d->symbol->s.byte_offset = bytes;
// 			d->symbol->s.local_var_index = local_var_counter++;
// 			printf("Resolution - Symbol address: %p, name: %s, index: %d\n",
// 				(void*)d->symbol, d->name, d->symbol->s.local_var_index);
// 			fprintf(stderr, "Created local variable %s with index %d\n", d->name, d->symbol->s.local_var_index);
// 		}

// 		scope_bind(stack, d->symbol);


// 		if (d->value) {
// 			expr_resolve(d->value, stack);
// 		}
		
// 		if (d->type->kind == TYPE_FUNCTION) {
// 			current_function = d;
// 			int original_scope = stack->top;

// 			scope_enter(stack, NULL);
// 			param_list_resolve(d->type->params, stack);
			
// 			scope_enter(stack, NULL);
// 			stmt_resolve(d->code, stack);

// 			size_t total_local_bytes = 0;
// 			struct symbol_table* local_scope = stack->symbol_tables[stack->top];
// 			struct symbol* current = local_scope->symbol;

// 			while (current) {
// 				total_local_bytes += current->s.byte_offset;
// 				current = current->next;
// 			}

// 			if (total_local_bytes % 16 != 0) {
// 				total_local_bytes += 16 - (total_local_bytes % 16);
// 			}

// 			d->symbol->s.total_local_bytes = total_local_bytes;
			
// 			while (stack->top > original_scope) {
// 				scope_exit(stack);
// 			}

// 			current_function = NULL;
// 			local_var_counter = 0;

// 		} else if (d->type->kind == TYPE_ARRAY) {
// 			if (d->value && d->value->kind == EXPR_ARRAY) {
// 				if (d->value->left) {
// 					expr_resolve(d->value->left, stack);

// 				}
				
// 				if (d->value->right) {
// 					struct expr* current = d->value->right;
// 					while (current) {
// 						expr_resolve(current, stack);
// 						current = current->right;
// 					}
// 				}
// 			}
// 		}
	
// 		d = d->next;
// 	}
// }

// void free_symbol(struct symbol* symbol) {
// 	if (!symbol) return;

// 	free(symbol->name);
// 	if (symbol->type) type_delete(symbol->type);
// 	free(symbol);
// }

// void free_stack(struct stack* stack) {
// 	if (!stack) return;

// 	for (int i = stack->top; i >= 0; i--) {
// 		struct symbol_table* table = stack->symbol_tables[i];
// 		struct symbol* current = table->symbol;
// 		while (current) {
// 			struct symbol* temp = current;
// 			current = current->next;
// 			free_symbol(temp);
// 		}
// 		free(table);
// 	}
// 	free(stack->symbol_tables);
// 	free(stack);
// }

// void print_symbol_table(struct stack* stack) {
// 	for (int i = stack->top; i >= 0; i--) {
// 		printf("Scope level: %d\n", i);
// 		struct symbol* current = stack->symbol_tables[i]->symbol;
// 		while (current) {
// 			printf("   Symbol: %s, Type: %d\n", current->name, current->type->kind);
// 			current = current->next;
// 		}
// 	}
// }

// void program_resolve(struct program* p, struct stack* stack) {
// 	if (!p) return;

// 	printf("About to resolve program!\n");
// 	if (p->declaration) decl_resolve(p->declaration, stack);
// 	printf("\nFinished resolving program!\n");

// }

// bool type_equals(struct type* a, struct type* b) {
// 	if (!a || !b) return false;

// 	if (a->kind == b->kind) {
// 		switch (a->kind) {
// 			case TYPE_INTEGER:
// 			case TYPE_CHARACTER:
// 			case TYPE_BOOLEAN:
// 			case TYPE_VOID:
// 				return true;

// 			case TYPE_FUNCTION:
// 				struct param_list* a_params = a->params;
// 				struct param_list* b_params = b->params;

// 				while (a_params && b_params) {
// 					if (!type_equals(a_params->type, b_params->type))
// 						return false;

// 					a_params = a_params->next;
// 					b_params = b_params->next;
// 				}

// 				return (a_params == NULL && b_params == NULL) && type_equals(a->subtype, b->subtype);

// 			case TYPE_ARRAY:
// 				if (!a->subtype || !b->subtype) {
// 					return false;
// 				}
// 				return type_equals(a->subtype, b->subtype);

// 			default:
// 				return false;
// 		}
// 	}

// }

// struct type* type_create(type_t kind, struct type* subtype, struct param_list* params) {
// 	struct type* t = malloc(sizeof(struct type));
// 	if (!t) {
// 		fprintf(stderr, "Error: Memory allocation failed in type_create\n");
// 		return NULL;
// 	}

// 	t->kind = kind;
// 	t->subtype = subtype;
// 	t->params = params;

// 	return t;
// }

// struct type* type_copy(struct type* t) {
// 	if (!t) return NULL;

// 	struct type* t_copy = malloc(sizeof(struct type));
// 	if (!t_copy) return NULL;

// 	t_copy->kind = t->kind;
// 	t_copy->subtype = t->subtype ? type_copy(t->subtype) : NULL;

// 	if (t->params) {
// 		struct param_list* current = t->params;
// 		struct param_list* prev_copy = NULL;
// 		struct param_list* first_copy = NULL;

// 		while (current) {
// 			struct param_list* param_copy = malloc(sizeof(struct param_list));
// 			param_copy->name = strdup(current->name);
// 			param_copy->type = type_copy(current->type);
// 			param_copy->next = NULL;

// 			if (prev_copy) {
// 				prev_copy->next = param_copy;
// 			} else {
// 				first_copy = param_copy;
// 			}

// 			prev_copy = param_copy;
// 			current = current->next;
// 		}

// 		t_copy->params = first_copy;
// 	} else {
// 		t_copy->params = NULL;
// 	}

// 	return t_copy;
// }

// void type_delete(struct type* t) {
// 	if (!t) return;

// 	struct param_list* param = t->params;
// 	while (param) {
// 		free(param->name);
// 		type_delete(param->type);
// 		free(param);
// 		param = param->next;
// 	}	

// 	type_delete(t->subtype);

// 	free(t);
// }

// struct type* expr_typecheck(struct expr* e, struct stack* stack) {
//     if (!e) return NULL;

//     struct type* lt = NULL;
//     struct type* rt = NULL;
//     struct type* result = NULL;

//     switch (e->kind) {
//         case EXPR_NAME: {
//             // Lookup the symbol in current scope stack
//             int found_scope;
//             struct symbol* sym = scope_lookup(stack, e->name, &found_scope);
//             if (!sym) {
//                 fprintf(stderr, "Error: Symbol '%s' not found in current scope\n", e->name);
//                 return type_create(TYPE_UNKNOWN, NULL, NULL);
//             }
//             e->symbol = sym;  // Update the symbol reference
//             result = type_copy(sym->type);
//             break;
//         }

//     	case EXPR_ARRAY_VAL:
//     		result = type_create(TYPE_INTEGER, NULL, NULL);
//     		break;

//         case EXPR_INTEGER:
//             result = type_create(TYPE_INTEGER, NULL, NULL);
//             break;

//         case EXPR_CHARACTER:
//             result = type_create(TYPE_CHARACTER, NULL, NULL);
//             break;

//         case EXPR_STRING:
//             result = type_create(TYPE_STRING, NULL, NULL);
//             break;

//         case EXPR_BOOLEAN:
//             result = type_create(TYPE_BOOLEAN, NULL, NULL);
//             break;

//         case EXPR_ARRAY: {
//             struct symbol* sym = scope_lookup(stack, e->name, NULL);
//             if (!sym) {
//                 fprintf(stderr, "Error: Array '%s' not found\n", e->name);
//                 return type_create(TYPE_UNKNOWN, NULL, NULL);
//             }
//             e->symbol = sym;
            
            
//             // Check array index expression
//             if (e->right) {
//                 struct type* index_type = expr_typecheck(e->right, stack);
//                 if (index_type->kind != TYPE_INTEGER) {
//                     fprintf(stderr, "Error: Array index must be integer type\n");
//                 }
//                 type_delete(index_type);
//             }
            
//             result = type_copy(sym->type->subtype);
//             break;
//         }

//         case EXPR_INCREMENT:
//         case EXPR_DECREMENT:
//             lt = expr_typecheck(e->left, stack);
//             if (!lt || lt->kind != TYPE_INTEGER) {
//                 fprintf(stderr, "Error: Increment/decrement requires integer type\n");
//                 result = type_create(TYPE_UNKNOWN, NULL, NULL);
//             } else {
//                 result = type_create(TYPE_INTEGER, NULL, NULL);
//             }
//             break;

//         case EXPR_ADD:
//         case EXPR_SUB:
//         case EXPR_MUL:
//         case EXPR_DIV:
//             lt = expr_typecheck(e->left, stack);
//             rt = expr_typecheck(e->right, stack);
            
//             if (!lt || !rt || lt->kind != TYPE_INTEGER || rt->kind != TYPE_INTEGER) {
//                 fprintf(stderr, "Error: Arithmetic operations require integer types\n");
//                 result = type_create(TYPE_UNKNOWN, NULL, NULL);
//             } else {
//                 result = type_create(TYPE_INTEGER, NULL, NULL);
//             }
//             break;

//         case EXPR_LESS:
//         case EXPR_GREATER:
//         case EXPR_LESS_EQUAL:
//         case EXPR_GREATER_EQUAL:
//         case EXPR_EQUAL:
//         case EXPR_NOT_EQUAL:
//             lt = expr_typecheck(e->left, stack);
//             rt = expr_typecheck(e->right, stack);
            
//             if (!lt || !rt || lt->kind != TYPE_INTEGER || rt->kind != TYPE_INTEGER) {
//                 fprintf(stderr, "Error: Comparison requires integer types\n");
//                 result = type_create(TYPE_UNKNOWN, NULL, NULL);
//             } else {
//                 result = type_create(TYPE_BOOLEAN, NULL, NULL);
//             }
//             break;

//         case EXPR_ASSIGNMENT:
//             lt = expr_typecheck(e->left, stack);
//             rt = expr_typecheck(e->right, stack);
            
//             if (!lt || !rt || !type_equals(lt, rt)) {
//                 fprintf(stderr, "Error: Type mismatch in assignment\n");
//                 result = type_create(TYPE_UNKNOWN, NULL, NULL);
//             } else {
//                 result = type_copy(lt);
//             }
//             break;

//         case EXPR_CALL: {
//             // Typecheck function name
//             struct symbol* sym = scope_lookup(stack, e->name, NULL);
//             if (!sym || sym->type->kind != TYPE_FUNCTION) {
//                 fprintf(stderr, "Error: '%s' is not a function\n", e->name);
//                 return type_create(TYPE_UNKNOWN, NULL, NULL);
//             }
            
//             // Check arguments
//             struct expr* arg = e->right;
//             struct param_list* param = sym->type->params;
            
//             while (arg && param) {
//                 struct type* arg_type = expr_typecheck(arg, stack);
//                 if (!type_equals(arg_type, param->type)) {
//                     fprintf(stderr, "Error: Argument type mismatch in function call\n");
//                 }
//                 type_delete(arg_type);
//                 arg = arg->right;
//                 param = param->next;
//             }
            
//             if (arg || param) {
//                 fprintf(stderr, "Error: Wrong number of arguments in function call\n");
//             }
            
//             result = type_copy(sym->type->subtype);
//             break;
//         }

//         default:
//             fprintf(stderr, "Error: Unknown expression type in typecheck\n");
//             result = type_create(TYPE_UNKNOWN, NULL, NULL);
//             break;
//     }

//     if (lt) type_delete(lt);
//     if (rt) type_delete(rt);
//     return result;
// }

// // void decl_typecheck(struct decl* d, struct stack* stack) {
// //     if (!d) return;

// //     while (d) {
// //         if (d->type->kind == TYPE_FUNCTION) {
// //             current_function = d;
            
// //             scope_enter(stack, NULL);
            
// //             // Rebuild parameter scope with bindings
// //             struct param_list* param = d->type->params;
// //             while (param) {
// //                 if (param->symbol) {      
// //                 	scope_bind(stack, param->symbol);
// //                 }
// //                 param = param->next;
// //             }

// //             if (d->code) {
// //                 scope_enter(stack, NULL);  
// //                 struct stmt* s = d->code;

// //                 while (s) {
// //                     if (s->kind == STMT_DECL && s->decl) {
// //                     	scope_bind(stack, s->decl->symbol);
                    	
// //                     }
// //                 }
// //                 s = s->next;
// //             }
                
// //             stmt_typecheck(d->code, stack);
// //             scope_exit(stack);         // Exit function body scope
// //         }
            
// //         scope_exit(stack);  // Exit parameter scope
// //         current_function = NULL;
// //         } else {
// //             if (!d->symbol) {
// //                 symbol_t kind = scope_level(stack) > 1 ? SYMBOL_LOCAL : SYMBOL_GLOBAL;
// //                 d->symbol = create_symbol(kind, d->type, d->name);
// //                 if (d->symbol) {
// //                     scope_bind(stack, d->symbol);
// //                 }
// //             }

// //             if (d->type->kind == TYPE_ARRAY) {
// //             	printf("Array typecheck - value kind: %d\n", d->value ? d->value->kind : - 1);
// //             	if (d->value && d->value->kind == EXPR_ARRAY) {
            		
// //             		if (d->value->left) {
// //             			struct type* size_type = expr_typecheck(d->value->left, stack);
// //             			if (size_type->kind != TYPE_INTEGER) {
// //             				fprintf(stderr, "Error: Array size must be integer\n");
// //             			}
// //             			type_delete(size_type);
// //             		}

// //             		if (d->value->right) {
// //             			struct expr* init_value = d->value->right;
         
// //             			while (init_value) {
// //             				struct type* value_type = expr_typecheck(init_value, stack);
// //             				if (!type_equals(value_type, d->type->subtype)) {
// //             					fprintf(stderr, "Error: Array initialization value type mismatch\n");
// //             				}
// //             				type_delete(value_type);

            				
// //             				init_value = init_value->right;
// //             			}
// //             		}
// //             	}

// //             } else if (d->value) {
// //                 struct type* value_type = expr_typecheck(d->value, stack);
                
// //                 if (!type_equals(value_type, d->type)) {
// //                     fprintf(stderr, "Error: Type mismatch in declaration of '%s'\n", d->name);
// //                 }
// //                 type_delete(value_type);
// //             }
            
// //         }
        
// //         d = d->next;
// //     }
// // }

// void decl_typecheck(struct decl* d, struct stack* stack) {
//     if (!d) return;

//     while (d) {
//         if (d->type->kind == TYPE_FUNCTION) {
//             current_function = d;
            
//             scope_enter(stack, NULL);
            
//             // Rebuild parameter scope with bindings
//             struct param_list* param = d->type->params;
//             while (param) {
//                 if (param->symbol) {
//                     scope_bind(stack, param->symbol);
//                 }
//                 param = param->next;
//             }

//             if (d->code) {
//                 scope_enter(stack, NULL);  
//                 struct stmt* s = d->code;

//                 while (s) {
//                     if (s->kind == STMT_DECL && s->decl) {
//                         // The symbol should already exist from resolve phase
//                         if (s->decl->symbol) {
//                             scope_bind(stack, s->decl->symbol);
//                         }
//                     }
//                     s = s->next;
//                 }
                
//                 stmt_typecheck(d->code, stack);
//                 scope_exit(stack);         // Exit function body scope
//             }
            
//             scope_exit(stack);  // Exit parameter scope
//             current_function = NULL;
//         } else {
//             if (d->type->kind == TYPE_ARRAY) {
//                 printf("Array typecheck - value kind: %d\n", d->value ? d->value->kind : -1);
//                 if (d->value && d->value->kind == EXPR_ARRAY) {
//                     if (d->value->left) {
//                         struct type* size_type = expr_typecheck(d->value->left, stack);
//                         if (size_type->kind != TYPE_INTEGER) {
//                             fprintf(stderr, "Error: Array size must be integer\n");
//                         }
//                         type_delete(size_type);
//                     }

//                     if (d->value->right) {
//                         struct expr* init_value = d->value->right;
         
//                         while (init_value) {
//                             struct type* value_type = expr_typecheck(init_value, stack);
//                             if (!type_equals(value_type, d->type->subtype)) {
//                                 fprintf(stderr, "Error: Array initialization value type mismatch\n");
//                             }
//                             type_delete(value_type);
//                             init_value = init_value->right;
//                         }
//                     }
//                 }
//             } else if (d->value) {
//                 struct type* value_type = expr_typecheck(d->value, stack);
                
//                 if (!type_equals(value_type, d->type)) {
//                     fprintf(stderr, "Error: Type mismatch in declaration of '%s'\n", d->name);
//                 }
//                 type_delete(value_type);
//             }
//         }
        
//         d = d->next;
//     }
// }


// void stmt_typecheck(struct stmt* s, struct stack* stack) {
//     if (!s) return;

//     while (s) {
//         switch (s->kind) {
//             case STMT_DECL:
//                 if (s->decl) {
//                     // Create and bind symbol for the declaration
//                     if (!s->decl->symbol) {
//                         symbol_t kind = scope_level(stack) > 1 ? SYMBOL_LOCAL : SYMBOL_GLOBAL;
//                         s->decl->symbol = create_symbol(kind, s->decl->type, s->decl->name);
//                         if (s->decl->symbol) {
//                             scope_bind(stack, s->decl->symbol);
//                         }
//                     }
//                     decl_typecheck(s->decl, stack);
//                 }
//                 break;

//             // Rest of the cases remain the same
//             case STMT_EXPR:
//                 if (s->expr) {
//                     struct type* t = expr_typecheck(s->expr, stack);
//                     type_delete(t);
//                 }
//                 break;

//             case STMT_IF:
//             case STMT_IF_ELSE:
//                 if (s->expr) {
//                     struct type* t = expr_typecheck(s->expr, stack);
//                     if (!t || t->kind != TYPE_BOOLEAN) {
//                         fprintf(stderr, "Error: If condition must be boolean type\n");
//                     }
//                     type_delete(t);
//                 }

//                 if (s->body) {
//                     scope_enter(stack, NULL);
//                     stmt_typecheck(s->body, stack);
//                     scope_exit(stack);
//                 }

//                 if (s->else_body) {
//                     scope_enter(stack, NULL);
//                     stmt_typecheck(s->else_body, stack);
//                     scope_exit(stack);
//                 }
//                 break;

//             case STMT_FOR:
//                 if (s->init_expr) {
//                     struct type* t_init = expr_typecheck(s->init_expr, stack);
//                     type_delete(t_init);
//                 }

//                 if (s->expr) {
//                     struct type* t_cond = expr_typecheck(s->expr, stack);
//                     if (!t_cond || t_cond->kind != TYPE_BOOLEAN) {
//                         fprintf(stderr, "Error: For loop condition must be boolean type\n");
//                     }
//                     type_delete(t_cond);
//                 }

//                 if (s->next_expr) {
//                     struct type* t_next = expr_typecheck(s->next_expr, stack);
//                     type_delete(t_next);
//                 }

//                 if (s->body) {
//                     scope_enter(stack, NULL);
//                     stmt_typecheck(s->body, stack);
//                     scope_exit(stack);
//                 }
//                 break;

//             case STMT_WHILE:
//                 if (s->expr) {
//                     struct type* t = expr_typecheck(s->expr, stack);
//                     if (!t || t->kind != TYPE_BOOLEAN) {
//                         fprintf(stderr, "Error: While condition must be boolean type\n");
//                     }
//                     type_delete(t);
//                 }

//                 if (s->body) {
//                     scope_enter(stack, NULL);
//                     stmt_typecheck(s->body, stack);
//                     scope_exit(stack);
//                 }
//                 break;

//             case STMT_BLOCK:
//                 scope_enter(stack, NULL);
//                 stmt_typecheck(s->body, stack);
//                 scope_exit(stack);
//                 break;

//             case STMT_RETURN:
//                 if (!current_function) {
//                     fprintf(stderr, "Error: Return statement outside function\n");
//                     break;
//                 }

//                 if (s->expr) {
//                     struct type* return_type = expr_typecheck(s->expr, stack);
//                     if (!return_type || !type_equals(return_type, current_function->type->subtype)) {
//                         fprintf(stderr, "Error: Return type mismatch in function '%s'\n", 
//                                 current_function->name);
//                     }
//                     type_delete(return_type);
//                 } else if (current_function->type->subtype->kind != TYPE_VOID) {
//                     fprintf(stderr, "Error: Non-void function '%s' missing return value\n",
//                             current_function->name);
//                 }
//                 break;
//         }

//         s = s->next;
//     }
// }

// void program_typecheck(struct program* p, struct stack* stack) {
//     if (!p) return;
    
//     debug_print_scope_stack(stack, "Start of program_typecheck");
    
//     if (p->declaration) {
//         decl_typecheck(p->declaration, stack); 
//     }
    
//     debug_print_scope_stack(stack, "End of program_typecheck");
// }