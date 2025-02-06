#ifndef CODEGEN_H
#define CODEGEN_H
#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_SCRATCH_REGISTERS 16
#define REGISTER_WIDTH 64

// Every time a expr_codegen is invoked, scracth_free should be invoked
static int label_counter = 0;
// 16(almost) general purpose registers
typedef enum {
	rax,
	rbx,
	rcx,
	rdx,
	rsi,
	rdi,
	rdp,
	rsp,
	r8,
	r9,
	r10,
	r11,
	r12,
	r13,
	r14,
	r15

} register_t;

typedef enum {
	REGISTER_FREE,
	REGISTER_USED
} register_state_t;

struct register {
	register_state_t state;
	int64_t size;
	const char* name;
	register_t type;
};

struct scratch_registers {
	struct register* registers[MAX_SCRATCH_REGISTERS];
	int size;
	int capacity;
}


struct register* create_register(register_t type, const char* name, register_state_t state);
struct scratch_registers* create_scratch_registers();
int scratch_alloc(struct scratch_registers* s);
void scratch_free(struct scratch_registers* s, int r);
const char* scratch_name(struct scratch_registers* s, int r);

int label_create();
const char* label_name( int label );
// %rax & %rdx cannot be used for other purposes while a multiply is in progress.

const char* symbol_codegen(struct symbol* sym);
void expr_codegen(struct scratch_registers* s, struct expr* e);
void stmt_codegen(struct scratch_registers* s, struct stmt* s);
void decl_codegen(struct scratch_registers* s, struct decl* d);

#endif