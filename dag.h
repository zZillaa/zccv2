#ifndef DAG_H
#define DAG_H

#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_DAG_SIZE 1000

typedef enum {
	DAG_ASSIGN,
	DAG_IADD,
	DAG_ISUB,
	DAG_IMUL,
	DAG_IDIV,
	DAG_NAME,
	DAG_FLOAT_VALUE,
	DAG_INTEGER_VALUE,
	DAG_RETURN,
	DAG_IF,
	DAG_LOOP
} dag_kind_t;


struct dag_node {
	dag_kind_t kind;
	struct dag_node* left;
	struct dag_node* right;
	union {
		const char* name;
		double float_value;
		int integer_value;
	} u;
};

struct dag_array {
	struct dag_nodes** nodes;
	int size;
	int capacity;
};



struct dag_node* create_dag_node(dag_kind_t kind, struct dag_node* left, struct dag_node* right,
union {
	const char* name;
	double float_value;
	int integer_value;
} u);

struct dat_node* find_or_create_dag_node(struct dag_array* array, dag_kind_t kind, struct dag_node* left, struct dag_node* right,
union {
	const char* name;
	double float_value;
	int integer_value;
} u); 

struct dag_array* create_dag_array(int capacity);
bool dag_node_equals(struct dag_node* a, struct dag_node* b);

struct dag_node* build_dag_from_expr(struct expr* e, struct dag_array* dag);
void build_dag_from_stmt(struct decl* s, struct dag_array* dag);
void build_dag_from_decl(struct decl* d, struct dag_array* dag);
struct dag_array* build_dag(struct program* ast);

#endif