// #include "IR.h"
// #define BLOCK_COUNT 1000
// #define PREDECESSOR_COUNT 100
// #define SUCCESSOR_COUNT 100
// #define DAG_NODE_COUNT 10

// struct dag_node* create_dag_node(dag_kind_t kind, struct dag_node* left, struct dag_node* right, union dag_value u) {
// 	struct dag_node* node = malloc(sizeof(struct dag_node));
// 	if (!node) {
// 		fprintf(stderr, "Error: Failed to allocate space for dag node\n");
// 		return NULL;
// 	}
// 	node->kind = kind;
// 	node->left = left;
// 	node->right = right;
// 	node->u = u;
// 	node->freed = false;

// 	return node;
// }

// struct dag_node_table* create_dag_node_table() {
// 	struct dag_node_table* dn_table = malloc(sizeof(struct dag_node_table));
// 	if (!dn_table) {
// 		fprintf(stderr, "Error: Failed to allocate space for dag node table\n");
// 		return NULL;
// 	}

// 	dn_table->node = NULL;
// 	dn_table->next = NULL;

// 	return dn_table;
// }

// struct dag_node_table* update_dn_tables(struct dag_node_table* dn_table, struct dag_node* new_node) {
// 	if (!dn_table) return NULL;
	
// 	struct dag_node_table* new_table = create_dag_node_table();
// 	if (!new_table) return NULL;

// 	new_table->next = dn_table;
// 	new_table->node = new_node;
// 	return new_table;
// }

// struct dag_node* find_or_create_dag_node(struct dag_node_table** dn_table, dag_kind_t kind, struct dag_node* l, struct dag_node* r, union dag_value u) {
// 	if (!*dn_table) return NULL;

// 	struct dag_node_table* current = *dn_table;
// 	while (current) {
// 		struct dag_node* node = current->node;
// 		if (node && node->kind == kind && node->left == l && node->right == r) {
// 			if (kind == DAG_NAME || kind == DAG_ARRAY) {
// 				if ((node->u.name && u.name && strcmp(node->u.name, u.name) == 0) || 
//                     (!node->u.name && !u.name)) {
//                     return node;
//                 }
// 			} else if (kind == DAG_INTEGER_VALUE) {
// 				if (node->u.integer_value == u.integer_value) return node;
// 			} else if (kind == DAG_CHARACTER) {
// 				if (node->u.char_value == u.char_value) return node;
// 			} else if (kind == DAG_ADD || kind == DAG_SUB || kind == DAG_MUL || kind == DAG_DIV ||
// 				kind == DAG_ADD_AND_ASSIGN || kind == DAG_SUB_AND_ASSIGN || kind == DAG_MUL_AND_ASSIGN ||
// 				kind == DAG_DIV_AND_ASSIGN || kind == DAG_INCREMENT || kind == DAG_DECREMENT || kind == DAG_NOT ||
// 				kind == DAG_LESS || kind == DAG_GREATER || kind == DAG_LESS_EQUAL || kind == DAG_GREATER_EQUAL || 
// 				kind == DAG_NOT_EQUAL || kind == DAG_EQUAL || kind == DAG_ASSIGN) {
// 					return node;
// 			}
// 		}
// 		current = current->next;
// 	}

// 	struct dag_node* new_node = create_dag_node(kind, l, r, u);
// 	if (!new_node) return NULL;
// 	struct dag_node_table* new_table = update_dn_tables(*dn_table, new_node);
// 	if (!new_table) {
// 		free_dag_node(new_node);
// 		return NULL;
// 	}
// 	*dn_table = new_table;

// 	return new_node;
// }

// struct dag_node* expr_intermediate_representation(struct expr* e, struct dag_node_table** dn_table) {
// 	if (!e || !*dn_table) {
// 		fprintf(stderr, "Error: expression or dn_talbe is null in expr_intermediate_representation()\n");
// 		return NULL;
// 	}

// 	switch (e->kind) {
// 		case EXPR_MUL: {
// 			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
// 			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);

// 			if (!left || !right) {
// 				fprintf(stderr, "Error: Failed to create DAG nodes for multiplication\n");
// 				return NULL;
// 			}

// 			if (left->kind == DAG_INTEGER_VALUE && right->kind == DAG_INTEGER_VALUE) {
// 				int product = left->u.integer_value * right->u.integer_value;
// 				union dag_value val = { .integer_value = product };
// 				return create_dag_node(DAG_INTEGER_VALUE, NULL, NULL, val);
// 			}
// 			union dag_value val = {0};
// 			return find_or_create_dag_node(dn_table, DAG_MUL, left, right, val);
// 		}
// 		case EXPR_SUB: {
// 			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
// 			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);

// 			if (!left || !right) {
// 				fprintf(stderr, "Error: Failed to create DAG nodes for subtraction\n");
// 				return NULL;
// 			}

// 			if (left->kind == DAG_INTEGER_VALUE && right->kind == DAG_INTEGER_VALUE) {
// 				int difference = left->u.integer_value - right->u.integer_value;
// 				union dag_value val = { .integer_value = difference };
// 				return create_dag_node(DAG_INTEGER_VALUE, NULL, NULL, val);
// 			}
// 			union dag_value val = {0};
// 			return find_or_create_dag_node(dn_table, DAG_SUB, left, right, val);
// 		}
// 		case EXPR_ADD: {
// 			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
// 			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);

// 			if (!left || !right) {
// 				fprintf(stderr, "Error: Failed to create DAG nodes for addition\n");
// 				return NULL;
// 			}

// 			if (left->kind == DAG_INTEGER_VALUE && right->kind == DAG_INTEGER_VALUE) {
// 				int sum = left->u.integer_value + right->u.integer_value;
// 				union dag_value val = { .integer_value = sum };
// 				return create_dag_node(DAG_INTEGER_VALUE, NULL, NULL, val);
// 			}
// 			union dag_value val = {0};
// 			return find_or_create_dag_node(dn_table, DAG_ADD, left, right, val);
// 		}
// 		case EXPR_DIV: {
// 			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
// 			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);

// 			if (!left || !right) {
// 				fprintf(stderr, "Error: Failed to create DAG nodes for division\n");
// 				return NULL;
// 			}

// 			if (left->kind == DAG_INTEGER_VALUE && right->kind == DAG_INTEGER_VALUE) {
// 				if (right->u.integer_value == 0) {
// 					fprintf(stderr, "Error: Division by zero is nonsense!\n");
// 					return NULL;
// 				}
// 				int quotient = left->u.integer_value / right->u.integer_value;
// 				union dag_value val = { .integer_value = quotient };
// 				return create_dag_node(DAG_INTEGER_VALUE, NULL, NULL, val);
// 			}
// 			union dag_value val = {0};
// 			return find_or_create_dag_node(dn_table, DAG_DIV, left, right, val);
// 		}
// 		case EXPR_ADD_AND_ASSIGN: {
// 			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
// 			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);

// 			if (!left || !right) {
// 				fprintf(stderr, "Error: Failed to create DAG nodes for ADD AND ASSIGN\n");
// 				return NULL;
// 			}

// 			union dag_value val = {0};
// 			return create_dag_node(DAG_ADD_AND_ASSIGN, left, right, val);
// 		}
// 		case EXPR_SUB_AND_ASSIGN: {
// 			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
// 			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);

// 			if (!left || !right) {
// 				fprintf(stderr, "Error: Failed to create DAG nodes for SUB AND ASSIGN\n");
// 				return NULL;
// 			}

// 			union dag_value val = {0};
// 			return create_dag_node(DAG_SUB_AND_ASSIGN, left, right, val);
// 		}
// 		case EXPR_MUL_AND_ASSIGN: {
// 			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
// 			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);

// 			if (!left || !right) {
// 				fprintf(stderr, "Error: Failed to create DAG nodes for MUL AND ASSIGN\n");
// 				return NULL;
// 			}

// 			union dag_value val = {0};
// 			return create_dag_node(DAG_MUL_AND_ASSIGN, left, right, val);
// 		}
// 	 	case EXPR_DIV_AND_ASSIGN: {
// 	 		struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
// 	 		struct dag_node* right = expr_intermediate_representation(e->right, dn_table);

// 	 		if (!left || !right) {
// 	 			fprintf(stderr, "Error: Failed to create DAG nodes for DIV AND ASSIGN\n");
// 	 			return NULL;
// 	 		}

// 	 		if (right && right->kind == DAG_INTEGER_VALUE) {
// 	 			if (right->u.integer_value == 0) {
// 	 				fprintf(stderr, "Error: Dividing by zero in DIV AND ASSIGN\n");
// 	 				return NULL;
// 	 			}
// 	 		}

// 	 		union dag_value val = {0};
// 	 		return create_dag_node(DAG_DIV_AND_ASSIGN, left, right, val);
// 	 	}
// 	 	case EXPR_INCREMENT: {
// 	 		struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
// 	 		if (!left) {
// 	 			fprintf(stderr, "Error: Failed to create DAG node for increment\n");
// 	 			return NULL;
// 	 		}

// 	 		union dag_value val = {0};
// 	 		return create_dag_node(DAG_INCREMENT, left, NULL, val);
// 	 	}
// 	 	case EXPR_DECREMENT: {
// 	 		struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
// 	 		if (!left) {
// 	 			fprintf(stderr, "Error: Failed to create DAG node for decrement\n");
// 	 			return NULL;
// 	 		}

// 	 		union dag_value val = {0};
// 	 		return create_dag_node(DAG_DECREMENT, left, NULL, val);
// 	 	}
// 	 	case EXPR_NOT: {
// 	 		struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
// 	 		if (!right) {
// 	 			fprintf(stderr, "Error: Failed to create DAG node for '!'\n");
// 	 			return NULL;
// 	 		}

// 	 		union dag_value val = {0};
// 	 		return create_dag_node(DAG_NOT, right, NULL, val);
// 		}
// 		case EXPR_LESS: {
// 			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
// 			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
// 			if (!left || !right) {
// 				fprintf(stderr, "Error: Failed to create DAG node for '<'\n");
// 				return NULL;
// 			}
// 			union dag_value val = {0};
// 			return create_dag_node(DAG_LESS, left, right, val);
// 		}
// 		case EXPR_GREATER: {
// 			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
// 			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
// 			if (!left || !right) {
// 				fprintf(stderr, "Error: Failed to create DAG node for '>'\n");
// 				return NULL;
// 			}
// 			union dag_value val = {0};
// 			return create_dag_node(DAG_GREATER, left, right, val);
// 		}
// 		case EXPR_LESS_EQUAL: {
// 			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
// 			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
// 			if (!left || !right) {
// 				fprintf(stderr, "Error: Failed to create DAG node for '<='\n");
// 				return NULL;
// 			}
// 			union dag_value val = {0};
// 			return create_dag_node(DAG_LESS_EQUAL, left, right, val);
// 		}
// 		case EXPR_GREATER_EQUAL: {
// 			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
// 			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
// 			if (!left || !right) {
// 				fprintf(stderr, "Error: Failed to create DAG node for '>='\n");
// 				return NULL;
// 			}
// 			union dag_value val = {0};
// 			return create_dag_node(DAG_GREATER_EQUAL, left, right, val);
// 		}
// 		case EXPR_NOT_EQUAL: {
// 			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
// 			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
// 			if (!left || !right) {
// 				fprintf(stderr, "Error: Failed to create DAG node for '!='\n");
// 				return NULL;
// 			}
// 			union dag_value val = {0};
// 			return create_dag_node(DAG_NOT_EQUAL, left, right, val);
// 		}
// 		case EXPR_EQUAL: {
// 			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
// 			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
// 			if (!left || !right) {
// 				fprintf(stderr, "Error: Failed to create DAG node for '=='\n");
// 				return NULL;
// 			}
// 			union dag_value val = {0};
// 			return create_dag_node(DAG_EQUAL, left, right, val);
// 		}
// 		case EXPR_ARRAY: {
// 			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
// 			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
// 			if (!left || !right) {
// 				fprintf(stderr, "Error: Failed to create DAG node for array\n");
// 				return NULL;
// 			}
// 			union dag_value val = { .name = e->name};
// 			return create_dag_node(DAG_ARRAY, left, right, val);
// 		}
// 		case EXPR_ARRAY_VAL: {
// 			union dag_value val = { .integer_value = e->integer_value};
// 			return create_dag_node(DAG_INTEGER_VALUE, NULL, NULL, val);
// 		}
		
// 		case EXPR_INTEGER: {
// 			union dag_value val = { .integer_value = e->integer_value};
// 			return create_dag_node(DAG_INTEGER_VALUE, NULL, NULL, val);	
// 		}
// 		case EXPR_CHARACTER: {
// 			union dag_value val = { .char_value = e->ch_expr};
// 			return create_dag_node(DAG_CHARACTER, NULL, NULL, val);
// 		}
// 		case EXPR_NAME: {
// 			union dag_value val = { .name = e->name};
// 			struct dag_node* node = create_dag_node(DAG_NAME, NULL, NULL, val);
// 			if (node) {
// 				node->symbol = e->symbol;
// 			}
// 			return node;
// 		}

// 		case EXPR_ASSIGNMENT: {
// 			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
// 			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
// 			if (!left || !right) {
// 				fprintf(stderr, "Error: Failed to create DAG node for assignment\n");
// 				return NULL;
// 			}
// 			union dag_value val = {0};
// 			struct dag_node* node = create_dag_node(DAG_ASSIGN, left, right, val);
// 			if (node) {
// 				node->symbol = e->symbol;
// 			}
// 			return node;
// 		}
// 		case EXPR_CALL:
// 		case EXPR_ARG:
// 		case EXPR_SUBSCRIPT:
// 		case EXPR_BREAK:
// 		case EXPR_CONTINUE:
// 			fprintf(stderr, "Warning: Expression kind %d currently not supported by DAG\n", e->kind);
// 			return NULL;
// 		default:
// 			fprintf(stderr, "Error: Unsupported expr kind %d\n", e->kind);
// 			return NULL;
// 	}

// }

// struct dag_node* stmt_intermediate_representation(struct stmt* s, struct dag_node_table** dn_table) {
// 	switch (s->kind) {
// 		case STMT_EXPR:
// 			return expr_intermediate_representation(s->expr, dn_table);
// 		case STMT_DECL: {
// 			if (s->decl && s->decl->value) {
// 				struct dag_node* node = expr_intermediate_representation(s->decl->value, dn_table);
// 				if (node) {
// 					node->symbol = s->decl->symbol;
// 				}
// 				return node;
// 			}
// 			return NULL;
// 		}
// 		case STMT_RETURN:
// 			return s->expr ? expr_intermediate_representation(s->expr, dn_table) : NULL;
// 		case STMT_IF:
// 		case STMT_FOR:
// 		case STMT_WHILE:
// 		default:
// 			fprintf(stderr, "Error: Unsupported stmt kind %d\n", s->kind);
// 			return NULL;
// 	}
// }

// struct DAG* build_DAG(struct decl* d) {
// 	if (!d) return NULL;

// 	struct dag_node_table* dn_table = create_dag_node_table();
// 	if (!dn_table) {
// 		fprintf(stderr, "Error: Failed to create dag node table\n");
// 		return NULL;
// 	}

// 	struct DAG* root = malloc(sizeof(struct DAG));
// 	if (!root) {
// 		fprintf(stderr, "Error: Failed to allocate space for DAG\n");
// 		free_dag_tables(dn_table);
// 		return NULL;
// 	}
// 	root->nodes = NULL;
// 	root->node_count = 0;

// 	int count = 0;
// 	struct decl* temp = d;
// 	while (temp) {
// 		count++;
// 		temp = temp->next;
// 	}

// 	root->nodes = malloc(sizeof(struct dag_node*) * count);
// 	if (!root->nodes) {
// 		fprintf(stderr, "Error: Failed to allocate space for DAG nodes\n");
// 		free(root);
// 		free_dag_tables(dn_table);
// 		return NULL;
// 	}
// 	root->node_count = count;

// 	int i = 0;
// 	while (d) {
// 		if (d->value) {
// 			root->nodes[i] = expr_intermediate_representation(d->value, &dn_table);
// 		} else if (d->code) {
// 			root->nodes[i] = stmt_intermediate_representation(d->code, &dn_table);
// 		} else {
// 			root->nodes[i] = NULL;
// 		}
// 		i++;
// 		d = d->next;
// 	}

// 	free_dag_tables(dn_table);
// 	dn_table = NULL;
// 	return root;
// };

// struct basic_block* initialize_block() {
// 	struct basic_block* block = malloc(sizeof(struct basic_block));
// 	if (!block) {
// 		fprintf(stderr, "Error: Failed to allocate space for block\n");
// 		return NULL;
// 	}

// 	block->dag = NULL;

// 	block->local_table = create_dag_node_table();
// 	if (!block->local_table) {
// 		fprintf(stderr, "Error: Failed to create dag node table for basic block\n");
// 		free(block);
// 		return NULL;
// 	}

// 	block->predecessor_count = 0;
// 	block->predecessors = malloc(sizeof(struct basic_block*) * PREDECESSOR_COUNT);
// 	if (!block->predecessors) {
// 		fprintf(stderr, "Error: Failed to allocate space for predecessor blocks\n");
// 		free_dag_tables(block->local_table);
// 		free(block);
// 		return NULL;
// 	}

// 	block->successor_count = 0;
// 	block->successors = malloc(sizeof(struct basic_block*) * SUCCESSOR_COUNT);
// 	if (!block->successors) {
// 		fprintf(stderr, "Error: Failed to allocate space for successor blocks\n");
// 		free(block->predecessors);
// 		free_dag_tables(block->local_table);
// 		free(block);
// 		return NULL;
// 	}
// 	block->block_freed = false;
// 	block->emitted_x86_assembly = false;
	
// 	return block;
// }

// struct CFG* initialize_CFG() {
// 	struct CFG* cfg = malloc(sizeof(struct CFG));
// 	if (!cfg) {
// 		fprintf(stderr, "Error: Could not allocate space for CFG\n");
// 		return NULL;
// 	}

// 	cfg->block_count = 0;
// 	cfg->blocks = malloc(sizeof(struct basic_block*) * BLOCK_COUNT);
// 	if (!cfg->blocks) {
// 		fprintf(stderr, "Error: Failed to allocate space for CFG blocks\n");
// 		free(cfg);
// 		return NULL;
// 	}

// 	for (int i = 0; i < BLOCK_COUNT; i++) {
// 		cfg->blocks[i] = NULL;
// 	}

// 	cfg->global_table = create_dag_node_table();
// 	if (!cfg->global_table) {
// 		fprintf(stderr, "Error: Failed to create global dag node table\n");
// 		free(cfg->blocks);
// 		free(cfg);
// 		return NULL;
// 	}
// 	cfg->next = NULL;

// 	return cfg;
// }

// struct DAG* create_block_dag() {
// 	struct DAG* dag = malloc(sizeof(struct DAG));
// 	if (!dag) {
// 		fprintf(stderr, "Error: Failed to allocate space for dag\n");
// 		return NULL;
// 	}

// 	dag->node_count = 0;
// 	dag->nodes = malloc(sizeof(struct dag_node*) * DAG_NODE_COUNT);
// 	if (!dag->nodes) {
// 		fprintf(stderr, "Error: Failed to allocate space for dag nodes\n");
// 		free(dag);
// 		return NULL;
// 	}

// 	for (int i = 0; i < DAG_NODE_COUNT; i++) {
// 		dag->nodes[i] = NULL;
// 	}

// 	return dag;
// }

// void process_stmt_for_blocks(struct CFG* cfg, struct stmt* s, int* current_block_index) {
// 	if (!cfg || !s || *current_block_index >= cfg->block_count) return;
// 	printf("In Process statements for blocks function\n");

// 	switch (s->kind) {
// 		case STMT_BLOCK: {
// 			printf("In STMT BLOCK case\n");
// 			struct stmt* curr = s->body;
// 			while (curr) {
// 				process_stmt_for_blocks(cfg, curr, current_block_index);
// 				curr = curr->next;
// 			}
// 			break;
// 		}

// 		case STMT_IF: {
// 			printf("In STMT IF case\n");
// 			struct basic_block* condition_block = initialize_block();
// 			if (!condition_block) return;
// 			cfg->blocks[*current_block_index] = condition_block;
// 			(*current_block_index)++;
// 			cfg->block_count = *current_block_index;

// 			if (s->expr) {
// 				condition_block->dag = create_block_dag();
// 				if (!condition_block->dag) {
// 					free_block(condition_block);
// 					return;
// 				};

// 				struct dag_node* node = expr_intermediate_representation(s->expr, &condition_block->local_table);
// 				if (node) {
// 					condition_block->dag->nodes[0] = node;
// 					condition_block->dag->node_count = 1;
// 				}
// 			}

// 			struct basic_block* true_block = initialize_block();
// 			if (!true_block) return;
// 			cfg->blocks[*current_block_index] = true_block;
// 			(*current_block_index)++;
// 			cfg->block_count = *current_block_index;

// 			process_stmt_for_blocks(cfg, s->body, current_block_index);

// 			struct basic_block* convergence_block = initialize_block();
// 			if (!convergence_block) return;
// 			cfg->blocks[*current_block_index] = convergence_block;
// 			(*current_block_index)++;
// 			cfg->block_count = *current_block_index;

// 			condition_block->successors[condition_block->successor_count++] = true_block;
// 			true_block->predecessors[true_block->predecessor_count++] = condition_block;

// 			condition_block->successors[condition_block->successor_count++] = convergence_block;
// 			convergence_block->predecessors[convergence_block->predecessor_count++] = condition_block;

// 			true_block->successors[true_block->successor_count++] = convergence_block;
// 			convergence_block->predecessors[convergence_block->predecessor_count++] = true_block;
// 			break;
// 		}

// 		case STMT_IF_ELSE: {
// 			printf("In STMT_IF_ELSE case\n");
// 			struct basic_block* condition_block = initialize_block();
// 			if (!condition_block) return;
// 			cfg->blocks[*current_block_index] = condition_block;
// 			(*current_block_index)++;
// 			cfg->block_count = *current_block_index;

// 			if (s->expr) {
// 				condition_block->dag = create_block_dag();
// 				if (!condition_block->dag) {
// 					free_block(condition_block);
// 					return;
// 				}

// 				struct dag_node* node = expr_intermediate_representation(s->expr, &condition_block->local_table);
// 				if (node) {
// 					condition_block->dag->nodes[0] = node;
// 					condition_block->dag->node_count = 1;
// 				}
// 			}

// 			struct basic_block* true_block = initialize_block();
// 			if (!true_block) return;
// 			cfg->blocks[*current_block_index] = true_block;
// 			(*current_block_index)++;
// 			cfg->block_count = *current_block_index;

// 			process_stmt_for_blocks(cfg, s->body, current_block_index);

// 			struct basic_block* else_block = initialize_block();
// 			if (!else_block) return;
// 			cfg->blocks[*current_block_index] = else_block;
// 			(*current_block_index)++;
// 			cfg->block_count = *current_block_index;

// 			process_stmt_for_blocks(cfg, s->else_body, current_block_index);

// 			struct basic_block* convergence_block = initialize_block();
// 			if (!convergence_block) return;
// 			cfg->blocks[*current_block_index] = convergence_block;
// 			(*current_block_index)++;
// 			cfg->block_count = *current_block_index;

// 			condition_block->successors[condition_block->successor_count++] = true_block;
// 			true_block->predecessors[true_block->predecessor_count++] = condition_block;

// 			condition_block->successors[condition_block->successor_count++] = else_block;
// 			else_block->predecessors[else_block->predecessor_count++] = condition_block;

// 			true_block->successors[true_block->successor_count++] = convergence_block;
// 			convergence_block->predecessors[convergence_block->predecessor_count++] = true_block;

// 			else_block->successors[else_block->successor_count++] = convergence_block;
// 			convergence_block->predecessors[convergence_block->predecessor_count++] = else_block;
// 			break;
// 		}

// 		case STMT_FOR: {
// 			struct basic_block* init_block = initialize_block();
// 			if (!init_block) return;
// 			cfg->blocks[*current_block_index++] = init_block;
// 			(*current_block_index)++;
// 			cfg->block_count = *current_block_index;

// 			if (s->init_expr) {
// 				init_block->dag = create_block_dag();
// 				if (!init_block->dag) {
// 					free_block(init_block);
// 					return;
// 				}

// 				struct dag_node* node = expr_intermediate_representation(s->init_expr, &init_block->local_table);
// 				if (node) {
// 					init_block->dag->nodes[0] = node;
// 					init_block->dag->node_count = 1;
// 				}
// 			}

// 			struct basic_block* condition_block = initialize_block();
// 			if (!condition_block) return;
// 			cfg->blocks[*current_block_index] = condition_block;
// 			(*current_block_index)++;
// 			cfg->block_count = *current_block_index;

// 			if (s->expr) {
// 				condition_block->dag = create_block_dag();
// 				if (!condition_block->dag) {
// 					free_block(condition_block);
// 					return;
// 				}

// 				struct dag_node* node = expr_intermediate_representation(s->expr, &condition_block->local_table);
// 				if (node) {
// 					condition_block->dag->nodes[0] = node;
// 					condition_block->dag->node_count = 1;
// 				}
// 			}

// 			struct basic_block* body_block = initialize_block();
// 			if (!body_block) return;
// 			cfg->blocks[*current_block_index] = body_block;
// 			(*current_block_index)++;
// 			cfg->block_count = *current_block_index;

// 			process_stmt_for_blocks(cfg, s->body, current_block_index);

// 			struct basic_block* update_block = initialize_block();
// 			if (!update_block) return;
// 			cfg->blocks[*current_block_index] = update_block;
// 			(*current_block_index)++;
// 			cfg->block_count = *current_block_index;

// 			if (s->next_expr) {
// 				update_block->dag = create_block_dag();
// 				if (!update_block->dag) {
// 					free_block(update_block);
// 					return;
// 				}

// 				struct dag_node* node = expr_intermediate_representation(s->next_expr, &update_block->local_table);
// 				if (node) {
// 					update_block->dag->nodes[0] = node;
// 					update_block->dag->node_count = 1;
// 				}
// 			}

// 			struct basic_block* exit_block = initialize_block();
// 			if (!exit_block) return;
// 			cfg->blocks[*current_block_index] = exit_block;
// 			(*current_block_index)++;
// 			cfg->block_count = *current_block_index;

// 			init_block->successors[init_block->successor_count++] = condition_block;
// 			condition_block->predecessors[condition_block->predecessor_count++] = init_block;

// 			condition_block->successors[condition_block->successor_count++] = body_block;
// 			body_block->predecessors[body_block->predecessor_count++] = condition_block;

// 			condition_block->successors[condition_block->successor_count++] = exit_block;
// 			exit_block->predecessors[exit_block->predecessor_count++] = condition_block;

// 			body_block->successors[body_block->successor_count++] = update_block;
// 			update_block->predecessors[update_block->predecessor_count++] = body_block;

// 			update_block->successors[update_block->successor_count++] = condition_block;
// 			condition_block->predecessors[condition_block->predecessor_count++] = update_block;
// 			break;

// 		}
// 		case STMT_WHILE: {
// 			struct basic_block* condition_block = initialize_block();
// 			if (!condition_block) return;
// 			cfg->blocks[*current_block_index] = condition_block;
// 			(*current_block_index)++;
// 			cfg->block_count = *current_block_index;

// 			if (s->expr) {
// 				condition_block->dag = create_block_dag();
// 				if (!condition_block->dag) {
// 					free(condition_block);
// 					return;
// 				}

// 				struct dag_node* node = expr_intermediate_representation(s->expr, &condition_block->local_table);
// 				if (node) {
// 					condition_block->dag->nodes[0] = node;
// 					condition_block->dag->node_count = 1;
// 				}
// 			}

// 			struct basic_block* body_block = initialize_block();
// 			if (!body_block) return;
// 			cfg->blocks[*current_block_index] = body_block;
// 			(*current_block_index)++;
// 			cfg->block_count = *current_block_index;

// 			process_stmt_for_blocks(cfg, s->body, current_block_index);

// 			struct basic_block* exit_block = initialize_block();
// 			if (!exit_block) return;
// 			cfg->blocks[*current_block_index] = exit_block;
// 			(*current_block_index)++;
// 			cfg->block_count = *current_block_index;

// 			condition_block->successors[condition_block->successor_count++] = body_block;
// 			body_block->predecessors[body_block->predecessor_count++] = condition_block;

// 			condition_block->successors[condition_block->successor_count++] = exit_block;
// 			exit_block->predecessors[exit_block->predecessor_count++] = condition_block;

// 			body_block->successors[body_block->successor_count++] = condition_block;
// 			condition_block->predecessors[condition_block->predecessor_count++] = body_block;
// 			break;
// 		}

// 		case STMT_DECL: {
// 			printf("In STMT_DECL case\n");
// 			struct basic_block* current_block = NULL;
// 			if (*current_block_index == 0) {
// 				current_block = cfg->blocks[0];
// 				if (!current_block) {
// 					fprintf(stderr, "Error: entry block is NULL\n");
// 					return;
// 				}

// 			} else {
// 				current_block = cfg->blocks[*current_block_index - 1];
// 				if (!current_block) {
// 					printf("No valid current block, initializing new block\n");
// 					current_block = initialize_block();
// 					if (!current_block) return;
// 					cfg->blocks[*current_block_index] = current_block;
// 					(*current_block_index)++;
// 					cfg->block_count = *current_block_index;
// 					if (*current_block_index > 1) {
// 						struct basic_block* prev_block = cfg->blocks[*current_block_index - 2];
// 						if (prev_block) {
// 							prev_block->successors[prev_block->successor_count++] = current_block;
// 							current_block->predecessors[current_block->predecessor_count++] = prev_block;
// 						}
// 					}
// 				}
// 			}
// 			printf("Moving on\n");
// 			if (s->decl && s->decl->value) {
// 				printf("here\n");
// 				if (!current_block->dag) {
// 					printf("Now here\n");
// 					current_block->dag = create_block_dag();
// 					if (!current_block->dag) {
// 						free_block(current_block);
// 						return;
// 					}
// 				}
// 				printf("current_block->dag is not null\n");
// 				struct dag_node* node = expr_intermediate_representation(s->decl->value, &current_block->local_table);
// 				if (node && current_block->dag->node_count < DAG_NODE_COUNT) {
// 					current_block->dag->nodes[current_block->dag->node_count++] = node;
// 				}
// 			}
// 			break;
// 		}
// 		case STMT_EXPR: {
// 			printf("In STMT_EXPR case\n");
// 			struct basic_block* current_block = cfg->blocks[*current_block_index - 1];
// 			if (!current_block) {
// 				current_block = initialize_block();
// 				if (!current_block) return;
// 				cfg->blocks[*current_block_index] = current_block;
// 				(*current_block_index)++;
// 				cfg->block_count = *current_block_index;

// 				if (*current_block_index > 1) {
// 					struct basic_block* prev_block = cfg->blocks[*current_block_index - 2];
// 					prev_block->successors[prev_block->successor_count++] = current_block;
// 					current_block->predecessors[current_block->predecessor_count++] = prev_block;
// 				}
// 			}

// 			if (s->expr) {
// 				if (!current_block->dag) {
// 					current_block->dag = create_block_dag();
// 					if (!current_block->dag) {
// 						free_block(current_block);
// 						return;
// 					}
// 				}
// 				struct dag_node* node = expr_intermediate_representation(s->expr, &current_block->local_table);
// 				if (node && current_block->dag->node_count < DAG_NODE_COUNT) {
// 					current_block->dag->nodes[current_block->dag->node_count++] = node;
// 				}
// 			}
// 			break;
// 		}

// 		default:
// 			fprintf(stderr, "Warning: Unsupported statement kind %d\n", s->kind);
// 			break;
// 	}
// }


// void identify_basic_blocks(struct CFG* cfg, struct stmt* s) {
// 	if (!cfg || !s) return;
// 	printf("In basic blocks, going to invoke function to construct blocks\n");

// 	int current_block_index = 0;
// 	printf("Now im here\n");
// 	process_stmt_for_blocks(cfg, s, &current_block_index);
// }

// struct CFG* build_function_CFG(struct decl* function_decl) {
// 	struct CFG* cfg = initialize_CFG();
// 	if (!cfg) return NULL;
// 	printf("Initialized CFG\n");

// 	struct basic_block* entry_block = initialize_block();
// 	printf("CFG initialized, block_count: %d\n", cfg->block_count);
// 	if (!entry_block) {
// 		free_CFG(cfg);
// 		return NULL;
// 	}

// 	cfg->blocks[0] = entry_block;
// 	cfg->block_count = 1;

// 	printf("About to identify basic blocks\n");
// 	if (function_decl->code) {

// 		identify_basic_blocks(cfg, function_decl->code);
// 	}
// 	printf("Returning CFG\n");
// 	printf("CFG built with %d blocks\n", cfg->block_count);
// 	return cfg;
// }

// struct CFG* build_CFG(struct decl* d) {
// 	if (!d) {
// 		fprintf(stderr, "Error: Cannot build CFG because d is NULL\n");
// 		return NULL;
// 	}

// 	struct CFG* head = NULL;
// 	struct CFG* current = NULL;
// 	printf("About to build CFG\n");

// 	while (d) {
// 		if (d->type->kind == TYPE_FUNCTION) {
// 			struct CFG* function_cfg = build_function_CFG(d);
// 			if (function_cfg) {
// 				if (!head) {
// 					head = function_cfg;
// 					current = function_cfg;
// 				} else {
// 					current->next = function_cfg;
// 					current = function_cfg;
// 				}
// 			}
// 		}

// 		d = d->next;
// 	}

// 	return head;
// }

// void free_dag_node(struct dag_node* node) {
// 	if (!node || node->freed) return;

// 	if (node->left && !node->left->freed) {
// 		node->left->freed = true;
// 		free_dag_node(node->left);
// 	}

// 	if (node->right && !node->right->freed) {
// 		node->right->freed = true;
// 		free_dag_node(node->right);
// 	}
// 	node->freed = true;
// 	free(node);
// }	

// void free_dag_tables(struct dag_node_table* dn_table) {
// 	if (!dn_table) return;

// 	struct dag_node_table* current = dn_table;
// 	while (current) {
// 		struct dag_node_table* next = current->next;
		
// 		if (current->node) {
// 			free_dag_node(current->node);
// 		}

// 		free(current);
// 		current = next;
// 	}
// }

// void free_block(struct basic_block* block) {
// 	if (!block) return;

// 	free_dag_tables(block->local_table);
// 	if (block->dag) {
// 		for (int i = 0; i < block->dag->node_count; i++) {
// 			free_dag_node(block->dag->nodes[i]);
// 		}
// 		free(block->dag->nodes);
// 		free(block->dag);
// 	}

// 	if (block->predecessors) {
// 		for (int i = 0; i < block->predecessor_count; i++) {
// 			if (block->predecessors[i]->block_freed) continue;

// 			free_block(block->predecessors[i]);
// 			block->predecessors[i]->block_freed = true;
// 		}
// 	}

// 	if (block->successors) {
// 		for (int i = 0; i < block->successor_count; i++) {
// 			if (block->successors[i]->block_freed) continue;

// 			free_block(block->successors[i]);
// 			block->successors[i]->block_freed = true;
// 		}
// 	}

// 	free(block->predecessors);
// 	free(block->successors);
// 	free(block);
// }

// void free_CFG(struct CFG* cfg) {
// 	if (!cfg) return;

// 	for (int i = 0; i < cfg->block_count; i++) {
// 		free_block(cfg->blocks[i]);
// 	}

// 	free(cfg);
// }

// void free_all_CFG(struct CFG* cfg) {
// 	if (!cfg) return;

// 	struct CFG* current = cfg;
// 	while (current) {
// 		struct CFG* next = current->next;
// 		for (int i = 0; i < current->block_count; i++) {
// 			free_block(current->blocks[i]);
// 		}
// 		free(current->blocks);
// 		free_dag_tables(current->global_table);
// 		free(current);
// 		current = next;
// 	}
// }