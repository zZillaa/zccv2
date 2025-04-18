#include "IR.h"

struct dag_node* create_dag_node(dag_kind_t kind, struct dag_node* left, struct dag_node* right, union {
	const char* name;
	char char_value;
	int integer_value;
} u) {
	struct dag_node* node = malloc(sizeof(struct dag_node));
	if (!node) {
		fprintf(stderr, "Error: Failed to allocate space for dag node\n");
		return NULL;
	}
	node->kind = kind;
	node->left = left;
	node->right = right;
	node->u = u;

	return node;
}

struct dag_node_table* create_dag_node_table() {
	struct dag_node_table* dn_table = malloc(sizeof(struct dag_node_table));
	if (!dn_table) {
		fprintf(stderr, "Error: Failed to allocate space for dag node table\n");
		return NULL;
	}

	dn_table->node = NULL;
	dn_table->next = NULL;

	return dn_table;
}

struct dag_node_table* update_dn_tables(struct dag_node_table* dn_table, struct dag_node_table* new_table, struct dag_node* new_node) {
	if (!dn_table || !new_table) return NULL;

	new_table->next = dn_table;
	new_table->node = new_node;

	return new_table;
}

struct dag_node* find_or_create_dag_node(struct dag_node_table* dn_table, dag_kind_t kind, struct dag_node* l, struct dag_node* r, union {
	const char* name;
	char char_value;
	int integer_value;
} u) {
	if (!dn_table) return NULL;

	struct dag_node_table* current = dn_table;
	while (current) {
		struct dag_node* node = current->node;
		if (node->left == l && node->right && node->u == u) {
			return node;
		}
		current = current->next;
	}

	struct dag_node_table* new_table = create_dag_node_table();	
	struct dag_node* new_node = create_dag_node(kind, l, r, u);
	dn_table = update_dn_tables(dn_table, new_table, new_node);

	return new_node;
}

struct dag_node* expr_intermediate_representation(struct expr* e, struct dag_node_table* dn_table) {
	if (!e) {
		fprintf(stderr, "Error: expression is null in expr_intermediate_representation()\n");
		return NULL;
	}

	switch (e->kind) {
		case EXPR_MUL: {
			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
			if (left->kind == DAG_INTEGER_VALUE && right->kind == DAG_INTEGER_VALUE) {
				int product = left->u.integer_value * right->u.interger_value;

				return create_dag_node(DAG_INTEGER_VALUE, NULL, NULL, (union u){.integer_value = product});
			}
			return find_or_create_dag_node(dn_table, DAG_IMUL, left, right, (union u){0});
		}
		case EXPR_SUB: {
			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
			if (left->kind == DAG_INTEGER_VALUE && right->kind == DAG_INTEGER_VALUE) {
				int difference = left->u.integer_value - right->u.integer_value;

				return create_dag_node(DAG_INTEGER_VALUE, NULL, NULL, (union u){.integer_value = difference});
			}
			return find_or_create_dag_node(dn_table, DAG_ISUB, left, right, (union u){0});
		}
		case EXPR_ADD: {
			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
			if (left->kind == DAG_INTEGER_VALUE && r->knd == DAG_INTEGER_VALUE) {
				int sum = left->u.integer_value + right->u.integer_value;

				return create_dag_node(DAG_INTEGER_VALUE, NULL, NULL, (union u){.integer_value = sum})
			}
			return find_or_create_dag_node(dn_table, DAG_IADD, left, right, (union u){0});
		}
		case EXPR_DIV: {
			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
			if (left->kind == DAG_INTEGER_VALUE && right->kind == DAG_INTEGER_VALUE) {
				if (r->u.integer_value == 0) {
					fprintf(stderr, "Error: Division by zero is nonsense!\n");
					return NULL;
				}
				int quotient = left->u.integer_value / right->u.integer_value;

				return create_dag_node(DAG_INTEGER_VALUE, NULL, NULL, (union u){.integer_value = quotient});
			}
			return create_dag_node(DAG_IDIV, left, right, (union u){0});
		}
		case EXPR_ADD_AND_ASSIGN: {
			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
			return create_dag_node(DAG_IADD_AND_ASSIGN, left, right, (union){0});
		}
		case EXPR_ISUB_AND_ASSIGN: {
			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
			return create_dag_node(DAG_ISUB_AND_ASSIGN, left, right, (union){0});
		}
		case EXPR_IMUL_AND_ASSIGN: {
			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
			return create_dag_node(DAG_IMUL_AND_ASSIGN, left, right, (union u){0});
		}
	 	case EXPR_IDIV_AND_ASSIGN: {
	 		struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
	 		struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
	 		return create_dag_node(DAG_IDIV_AND_ASSIGN, left, right, (union u){0});
	 	}
	 	case EXPR_INCREMENT: {
	 		struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
	 		return create_dag_node(DAG_INCREMENT, left, NULL, (union u){0});
	 	}
	 	case EXPR_DECREMENT: {
	 		struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
	 		return create_dag_node(DAG_DECREMENT, left, NULL, (union u){0});
	 	}
	 	case EXPR_NOT: {
	 		struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
	 		return create_dag_node(DAG_NOT, right, NULL, (union u){0});
		}
		case EXPR_LESS: {
			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
			return create_dag_node(DAG_LESS, left, right, (union u){0});
		}
		case EXPR_GREATER: {
			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
			return create_dag_node(DAG_GREATER, left, right, (union u){0});
		}
		case EXPR_LESS_EQUAL: {
			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
			return create_dag_node(DAG_LESS_EQUAL, left, right, (union u){0});
		}
		case EXPR_GREATER_EQUAL: {
			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
			return create_dag_node(DAG_GREATER_EQUAL, left, right, (union u){0});
		}
		case EXPR_NOT_EQUAL: {
			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
			return create_dag_node(DAG_NOT_EQUAL, left, right, (union u){0});
		}
		case EXPR_EQUAL: {
			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
			return create_dag_node(DAG_EQUAL, left, right, (union u){0});
		}
		case EXPR_ARRAY: {
			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
			return create_dag_node(DAG_ARRAY, left, right, (union u){.name = e->name});
		}
		case EXPR_ARRAY_VAL:
			return create_dag_node(DAG_INTEGER_VALUE, NULL, NULL, (union u){.integer_value = e->interger_value});
		
		case EXPR_INTEGER:
			return create_dag_node(DAG_INTEGER_VALUE, NULL, NULL, (union u){.integer_value = e->integer_value})
		case EXPR_CHARACTER:
			return create_dag_node(DAG_CHARACTER, NULL, NULL, (union u){.char_value = e->ch_expr});
		case EXPR_NAME:
			return create_dag_node(DAG_NAME, NULL, NULL, (union u){.name = e->name});

		case EXPR_ASSIGNMENT: {
			struct dag_node* left = expr_intermediate_representation(e->left, dn_table);
			struct dag_node* right = expr_intermediate_representation(e->right, dn_table);
			return create_dag_node(DAG_ASSIGN, left, right, (union u){0});
		}
		case EXPR_CALL:
		case EXPR_ARG:
		case EXPR_SUBSCRIPT:
		case EXPR_BREAK:
		case EXPR_CONTINUE:
			fprintf(stderr, "Warning: Expression kind $d currently not supported by DAG\n", e->kind);
			return NULL;
		default:
			fprintf(stderr, "Error: Unsupported expr kind $d\n", e->kind);
			return NULL;
	}

}

struct dag_node* stmt_intermediate_representation(struct stmt* s, struct dag_node_table* dn_table) {
	switch (s->kind) {
		case STMT_EXPR:
			return expr_intermediate_representation(s->expr, dn_table);
		case STMT_DECL:
			return s->decl->value ? expr_intermediate_representation(s->decl->value, dn_table) : NULL;
		case STMT_RETURN:
			return s->expr ? expr_intermediate_representation(s->expr, dn_table) : NULL;
		case STMT_IF:
		case STMT_FOR:
		case STMT_WHILE:
		default:
			fprintf(stderr, "Error: Unsupported stmt kind %d\n", s->kind);
			return NULL;
	}
}

struct DAG* build_DAG(struct decl* d) {
	struct DAG* root = malloc(sizeof(struct DAG));
	if (!root) {
		fprintf(stderr, "Error: Failed to allocate space for DAG\n");
		return NULL;
	}

	struct dag_node_table* dn_table = create_dag_node_table();

	while (d) {
		if (d->value) {
			root->node = expr_intermediate_representation(d->value, dn_table);
		} else if (d->code) {
			root->node = stmt_intermediate_representationd(d->code, dn_table);
		}

		d = d->next;
	}

	free_dag_tables(dn_table)
};

void free_dag_node(struct dag_node* node) {
	if (!node) return;
	
	if (node->left) {
		free_dag_node(node->left)
	}

	if (node->right) {
		free_dag_node(node->right);
	};
	free(node);
}	

void free_dag_tables(struct dag_node_table* dn_table) {
	if (!dn_table) return;

	struct dag_node_table* current = dn_table;
	while (current) {
		struct dag_node_table* next = current->next;
		
		if (current->node) {
			free_dag_node(current->node);
		}

		free(current);
		current = next;
	}
}