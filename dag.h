#ifndef DAG_H
#define DAG_H

#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_DAG_SIZE 1000
#define UNION_INITIALIZER(val) (dag_node_u ({ .name = (val)})

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

	DAG_LESS,
	DAG_GREATER,
	DAG_LESS_OR_EQUAL,
	DAG_GREATER_OR_EQUAL,
	DAG_EQUAL,
	DAG_NOT_EQUAL,

	DAG_ASSIGN,
	DAG_NAME,
	DAG_FLOAT_VALUE,
	DAG_INTEGER_VALUE,
	DAG_RETURN,
	DAG_ARRAY,
	DAG_IF,
	DAG_LOOP
} dag_kind_t;


typedef union {
	const char* name;
	int integer_value;
} dag_node_u;

struct dag_node {
	dag_kind_t kind;
	struct dag_node* left;
	struct dag_node* right;
	dag_node_u u;
};

struct dag_array {
	struct dag_nodes** nodes;
	int size;
	int capacity;
};

struct dag_node* create_dag_node(dag_kind_t kind, struct dag_node* left, struct dag_node* right, dag_node_u u);
struct dat_node* find_or_create_dag_node(struct dag_array* array, dag_kind_t kind, struct dag_node* left, struct dag_node* right, dag_node_u u); 
struct dag_array* create_dag_array(int capacity);
bool dag_node_equals(struct dag_node* a, struct dag_node* b);

struct dag_node* build_dag_from_expr(struct expr* e, struct dag_array* dag);
void build_dag_from_stmt(struct stmt* s, struct dag_array* dag);
void build_dag_from_decl(struct decl* d, struct dag_array* dag);
struct dag_array* build_dag(struct program* ast);

#endif