#ifndef IR_H
#define IR_H

#include "stdio.h"
#include "stdlib.h"
#include <stdbool.h>

typedef enum {
	DAG_IADD,
	DAG_ISUB,
	DAG_IMUL,
	DAG_IDIV,

	DAG_IADD_AND_ASSIGN,
	DAG_ISUB_AND_ASSIGN,
	DAG_IMUL_AND_ASSIGN,
	DAG_IDIV_AND_ASSIGN,

	DAG_INCREMENT,
	DAG_DECREMENT,
	DAG_NOT,

	DAG_LESS,
	DAG_GREATER,
	DAG_LESS_EQUAL,
	DAG_GREATER_EQUAL,
	DAG_NOT_EQUAL,
	DAG_EQUAL,

	DAG_NAME,
	DAG_ASSIGN,
	DAG_INTEGER_VALUE
	DAG_ARRAY,
} dag_kind_t;

typedef struct dag_node {
	dag_kind_t kind;
	struct dag_node* left;
	struct dag_node* right;

	union {
		const char* name;
		char char_value;
		int integer_value;
	} u;
} dag_node;

struct dag_node_table {
	struct dag_node* node;
	struct dag_node_table* next;
}

struct DAG {
	struct dag_node* node;
};

struct dag_node* create_dag_node(dag_kind_t kind, struct dag_node* l, struct dag_node* r, union {
	const char* name;
	char char_value;
	int integer_value;
} u);

struct dag_node_table* create_dag_node_table();

void update_dn_tables(struct dag_node_table* dn_table, struct dag_node_table* new_table, struct dag_node* new_node);
struct dag_node* find_or_create_dag_node(struct dag_node_table* dn_table, 
	dag_kint_t kind, struct dag_node* l, struct dag_node* r union {
		const char* name;
		char char_value;
		int integer_value;
	} u);

struct dag_node* expr_intermediate_representation(struct expr* e, struct dag_node_table* dn_table);
struct dag_node* stmt_intermediate_representation(struct stmt* s, struct dag_node_table* dn_table);
struct DAG* build_DAG(struct decl* d);

void free_dag_node(struct dag_node* node);
void free_dag_tables(struct dag_node_table* dn_table);


#endif