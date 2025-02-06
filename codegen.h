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

struct register {
	typedef enum {
	 FREE, 
	 USED 
	} state;

	int64_t size;
	const char* name;
	register_t kind;
};

struct scratch_registers {
	struct register* registers[MAX_SCRATCH_REGISTERS];
	int size;
	int capacity;
}


int scratch_alloc(struct scratch_registers* s, const char* name);
void scratch_free(struct scratch_registers* s, int r);
const char* scratch_name(struct scratch_registers* s, int r);
struct register* create_register(register_t kind, );
struct scratch_registers* create_scratch_registers();

// Need to generate a large number of unique, anonymous labels that indicate the targets of jumps and conditional branches.

int label_create();
const char* label_name( int label );

// symbol to code generation
// Function to generate the address of a symbol
// Returns a string which is a fragment of an instruction, representing the address computatio needed for a given symbol

const char* symbol_codegen(struct scratch_registers* s, struct symbol* sym);
void expr_codegen(struct scratch_registers* s, struct expr* e);
void stmt_codegen(struct scratch_registers* s, struct stmt* s);
void decl_codegen(struct scratch_registers* s, struct decl* d);


// TODO
// Add a reg field to the AST or DAG node structure, which will hold the number of a register returned by scratch_alloc
// When visiting each node, emit an instruction and place into the reg field the number of the register containing that value.
// When the node is no longer needed, call scratch_free to release
#endif