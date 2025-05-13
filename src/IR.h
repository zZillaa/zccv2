#ifndef IR_H
#define IR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ast.h"

typedef enum {
	DAG_ADD,
	DAG_SUB,
	DAG_MUL,
	DAG_DIV,

	DAG_ADD_AND_ASSIGN,
	DAG_SUB_AND_ASSIGN,
	DAG_MUL_AND_ASSIGN,
	DAG_DIV_AND_ASSIGN,

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
	DAG_INTEGER_VALUE,
	DAG_CHARACTER,
	DAG_ARRAY
} dag_kind_t;

union dag_value {
	const char* name;
	char char_value;
	int integer_value;
};

struct dag_node {
	dag_kind_t kind;
	struct dag_node* left;
	struct dag_node* right;
	union dag_value u;
	bool freed;
	int reg;
	struct symbol* symbol;
};

struct dag_node_table {
	struct dag_node* node;
	struct dag_node_table* next;
};

struct DAG {
	struct dag_node** nodes;
	int node_count;
};

struct basic_block {
	struct DAG* dag;
	struct dag_node_table* local_table;

	int predecessor_count;
	struct basic_block** predecessors;

	int successor_count;
	struct basic_block** successors;

	bool block_freed;
	bool emitted_x86_assembly;
	int label;
};

struct CFG {
	int block_count;
	struct basic_block** blocks;
	struct dag_node_table* global_table;

	struct CFG* next;
};

struct dag_node* create_dag_node(dag_kind_t kind, struct dag_node* l, struct dag_node* r, union dag_value u);
struct dag_node_table* create_dag_node_table();

struct dag_node_table* update_dn_tables(struct dag_node_table* dn_table, struct dag_node* new_node);
struct dag_node* find_or_create_dag_node(struct dag_node_table** dn_table, 
	dag_kind_t kind, struct dag_node* l, struct dag_node* r, union dag_value u);

struct dag_node* expr_intermediate_representation(struct expr* e, struct dag_node_table** dn_table);
struct dag_node* stmt_intermediate_representation(struct stmt* s, struct dag_node_table** dn_table);
struct DAG* build_DAG(struct decl* d);

void process_stmt_for_blocks(struct CFG* cfg, struct stmt* body, int* current_block_index);
void identify_basic_blocks(struct CFG* cfg, struct stmt* s);
struct DAG* create_block_dag();
struct basic_block* initialize_block();
struct CFG* initialize_CFG();
struct CFG* build_CFG(struct decl* d);

void free_dag_node(struct dag_node* node);
void free_dag_tables(struct dag_node_table* dn_table);
void free_block(struct basic_block* block);
void free_CFG(struct CFG* cfg);
void free_all_CFG(struct CFG* cfg);

#endif