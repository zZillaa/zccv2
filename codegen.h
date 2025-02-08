#ifndef CODEGEN_H
#define CODEGEN_H
#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_SCRATCH_REGISTERS 7

static int label_counter = 0;

typedef enum register_state_t {
	REGISTER_FREE,
	REGISTER_USED
} register_state_t;

struct Register {
	register_state_t state;
	const char* name;
};

struct RegisterTable {
	struct Register* registers[MAX_SCRATCH_REGISTERS];
	int capacity;
};

int scratch_alloc(struct RegisterTable* sregs);
void scratch_free(struct RegisterTable* sregs, int r);
const char* scratch_name(struct RegisterTable* sregs, int r);
struct Register* create_register(register_state_t state, const char* name);
struct RegisterTable* create_register_table();

int label_create();
const char* label_name( int label );

const char* symbol_codegen(struct symbol* sym);
void expr_codegen(struct RegisterTable* sregs, struct expr* e);
void stmt_codegen(struct RegisterTable* sregs, struct stmt* s);
void decl_codegen(struct RegisterTable* sregs, struct decl* d);

void free_register(struct Register* r);
void free_register_table(struct RegisterTable* sregs);
#endif