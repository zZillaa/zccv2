#include "dag.h"

struct dag_node* create_dag_node(dag_kind_t kind, struct dag_node* left, struct dag_node* right, dag_node_u u) {

	struct dag_node* node = malloc(sizeof(struct dag_node));
	if (!node) {
		fprintf(stderr, "Error: failed to allocate space for dag_node\n");
		return NULL;
	}

	node->kind = kind;
	node->left = left;
	node->right = right;
	node->u = u;

	return node;
} 

struct dag_array* create_dag_array(int capacity) {
	struct dag_array* array = malloc(sizeof(struct dag_array));
	if (!array) return NULL;

	array->nodes = malloc(sizeof(struct dag_node*) * capacity);
	if (!array->nodes) return NULL;

	array->size = 0; 
	array->capacity = capacity;

	return array;
}

bool dag_node_equals(struct dag_node* a, struct dag_node* b) {
	if (!a || !b) return false;

	if (a->kind != b->kind) return false;

	switch (a->kind) {
		case DAG_ASSIGN:
		case DAG_IMUL:
		case DAG_IDIV:
		case DAG_ISUB:
		case DAG_IADD:
			return a->left == b->left && a->right == b->right;
		
		case DAG_NAME:
			return strcmp(a->u.name, b->u.name) == 0;
		case DAG_INTEGER_VALUE:
			return a->u.integer_value == b->u.integer_value;
		case DAG_RETURN:
		case DAG_IF:
		case DAG_LOOP:
			return a->left == b->left && a->right == b->right;
		default:
			return false;
	}
}

struct dag_node* find_or_create_dag_node( struct dag_array* array, dag_kind_t kind, struct dag_node* left, struct dag_node* right, dag_node_u u) {

	struct dag_node* temp = create_dag_node(kind, left, right, u);
	if (!temp) return NULL;

	for (int i = 0; i < array->size; i++) {
		struct dag_node* node = array->nodes[i];
		if (dag_node_equals(node, temp)) {
			if (kind == DAG_NAME && temp->u.name) {
				free((void*)temp->u.name);
			}
			free(temp);
			return node;
		}
	}

	if (array->size >= array->capacity) {
		array->capacity *= 2;
		void* temp = realloc(array->nodes, sizeof(struct dag_node*) * array->capacity);
		if (!temp) {
			free(temp);
			return NULL;
		}
		array->nodes = temp;
	}

	array->nodes[array->size++] = temp;
	return temp;
}

struct dag_node* build_dag_from_expr(struct expr* e, struct dag_array* dag) {
	if (!e) return NULL;

	struct dag_node* left = e->left ? build_dag_from_expr(e->left, dag) : NULL;
	struct dag_node* right = e->right ? build_dag_from_expr(e->right, dag) : NULL;

	switch (e->kind) {
		case EXPR_INTEGER: {
			dag_node_u u;
			u.integer_value = e->integer_value;
			return find_or_create_dag_node(dag, DAG_INTEGER_VALUE, NULL, NULL, u);
		}

		case EXPR_NAME: {
			dag_node_u u;
			u.name = strdup(e->name);

			struct dag_node* node = find_or_create_dag_node(dag, DAG_NAME, NULL, NULL, u);
			if (node && node->u.name != u.name) {
				free((void*)u.name);
			}
			return node;
		}

		case EXPR_ASSIGNMENT:
			return find_or_create_dag_node(dag, DAG_ASSIGN, left, right, UNION_INITIALIZER(0));

		case EXPR_LESS:
			return find_or_create_dag_node(dag, DAG_LESS, left, right, UNION_INITIALIZER(0));

		case EXPR_GREATER:
			return find_or_create_dag_node(dag, DAG_GREATER, left, right, UNION_INITIALIZER(0));

		case EXPR_GREATER_EQUAL:
			return find_or_create_dag_node(dag, DAG_GREATER_OR_EQUAL, left, right, UNION_INITIALIZER(0));

		case EXPR_LESS_EQUAL:
			return find_or_create_dag_node(dag, DAG_LESS_OR_EQUAL, left, right, UNION_INITIALIZER(0));

		case EXPR_EQUAL:
			return find_or_create_dag_node(dag, DAG_EQUAL, left, right, UNION_INITIALIZER(0));

		case EXPR_NOT_EQUAL:
			return find_or_create_dag_node(dag, DAG_NOT_EQUAL, left, right, UNION_INITIALIZER(0));

		case EXPR_DECREMENT:
			return find_or_create_dag_node(dag, DAG_DECREMENT, left, right, UNION_INITIALIZER(0));
		
		case EXPR_INCREMENT:  
			return find_or_create_dag_node(dag, DAG_INCREMENT, left, right, UNION_INITIALIZER(0));
		
		case EXPR_ADD_AND_ASSIGN:
			return find_or_create_dag_node(dag, DAG_IADD_AND_ASSIGN, left, right, UNION_INITIALIZER(0));

		case EXPR_SUB_AND_ASSIGN:
			return find_or_create_dag_node(dag, DAG_ISUB_AND_ASSIGN, left, right, UNION_INITIALIZER(0));

		case EXPR_MUL_AND_ASSIGN:
			return find_or_create_dag_node(dag, DAG_IMUL_AND_ASSIGN, left, right, UNION_INITIALIZER(0));

		case EXPR_DIV_AND_ASSIGN:
			return find_or_create_dag_node(dag, DAG_IDIV_AND_ASSIGN, left, right, UNION_INITIALIZER(0));

		case EXPR_ADD:
            return find_or_create_dag_node(dag, DAG_IADD, left, right, UNION_INITIALIZER(0));

        case EXPR_SUB:
            return find_or_create_dag_node(dag, DAG_ISUB, left, right, UNION_INITIALIZER(0));

        case EXPR_MUL:
            return find_or_create_dag_node(dag, DAG_IMUL, left, right, UNION_INITIALIZER(0));

        case EXPR_DIV:
            return find_or_create_dag_node(dag, DAG_IDIV, left, right, UNION_INITIALIZER(0));

        default:
        	fprintf(stderr, "Error: Unhandled expression kind: %d\n", e->kind);
        	return NULL;
	}
}

void build_dag_from_stmt(struct stmt* s, struct dag_array* dag) {
	if (!s) return;

	while (s) {
		switch (s->kind) {
			case STMT_EXPR:
				build_dag_from_expr(s->expr, dag);
				break;
			case STMT_RETURN:
				build_dag_from_expr(s->expr, dag);
				break;
			case STMT_IF:
			case STMT_IF_ELSE:
				build_dag_from_expr(s->expr, dag);
				build_dag_from_stmt(s->body, dag);
				if (s->else_body) build_dag_from_stmt(s->else_body, dag);
				break;
			case STMT_WHILE:
				build_dag_from_expr(s->expr, dag);
				build_dag_from_stmt(s->body, dag);
				break;
			case STMT_FOR:
				if (s->init_expr) build_dag_from_expr(s->init_expr, dag);
				if (s->expr) build_dag_from_expr(s->expr, dag);
				if (s->next_expr) build_dag_from_expr(s->next_expr, dag);
				build_dag_from_stmt(s->body, dag);
				break;
			case STMT_BLOCK:
				build_dag_from_stmt(s->body, dag);
				break;
			default:
				fprintf(stderr, "Unhandled statement kind: %d\n", s->kind);
				break;
		}
		s = s->next;
	}
}

void build_dag_from_decl(struct decl* d, struct dag_array* dag) {
	if (!d) return;

	while (d) {
		if (d->value) {
			build_dag_from_expr(d->value, dag);
		}

		if (d->code) {
			build_dag_from_stmt(d->code, dag);
		}

		d = d->next;
	}
}

struct dag_array* build_dag(struct program* ast) {
	if (!ast) return NULL;

	struct dag_array* dag = create_dag_array(MAX_DAG_SIZE);
	if (!dag) return NULL;

	build_dag_from_decl(ast->declaration, dag);

	return dag;
}