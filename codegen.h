#ifndef CODEGEN_H
#define CODEGEN_H
#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_SCRATCH_REGISTERS 16
#define REGISTER_WIDTH 64

static int label_counter = 0;

// 16(almost) general purpose registers
typedef enum register_state_t {
	REGISTER_FREE,
	REGISTER_USED
} register_state_t;

struct Register {
	register_state_t state;
	int64_t size;
	const char* name;
};



struct scratch_registers {
	struct Register* registers[MAX_SCRATCH_REGISTERS];
	int size;
	int capacity;
};


struct Register* create_register(const char* name, register_state_t state);
struct scratch_registers* create_scratch_registers();
int scratch_alloc(struct scratch_registers* sregs);
void scratch_free(struct scratch_registers* sregs, int r);
const char* scratch_name(struct scratch_registers* sregs, int r);

int label_create();
const char* label_name( int label );
// %rax & %rdx cannot be used for other purposes while a multiply is in progress.

const char* symbol_codegen(struct symbol* sym);
void expr_codegen(struct scratch_registers* sregs, struct expr* e);
void stmt_codegen(struct scratch_registers* sregs, struct stmt* s);
void decl_codegen(struct scratch_registers* sregs, struct decl* d);

#endif