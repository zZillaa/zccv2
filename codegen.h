#ifndef CODEGEN_H
#define CODEGEN_H
#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#define MAX_SCRATCH_REGISTERS 16

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
	enum {
	 FREE, 
	 USED 
	} state;

	int64_t size;
	const char* name;
	register_t kind;
};

struct scratch_registers {
	struct register** array;
	int size;
	int capacity;
}


int scratch_alloc();
void scratch_free( int r);
const char* scratch_name( int r);
struct scratch_registers* create_scratch_registers();
// Need to generate a large number of unique, anonymous labels that indicate the targets of jumps and conditional branches.

int label_create();
const char* label_name( int label );

// symbol to code generation
// Function to generate the address of a symbol
// Returns a string which is a fragment of an instruction, representing the address computatio needed for a given symbol

const char* symbol_codegen(struct symbol* s);
void expr_codegen(struct expr* e);
void stmt_codegen(struct stmt* s);
void decl_codegen(struct decl* d);


// TODO
// Add a reg field to the AST or DAG node structure, which will hold the number of a register returned by scratch_alloc
// When visiting each node, emit an instruction and place into the reg field the number of the register containing that value.
// When the node is no longer needed, call scratch_free to release
#endif