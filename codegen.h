#ifndef CODEGEN_H
#define CODEGEN_H

int scratch_alloc();
void scratch_free( int r);
const char* scratch_name( int r);

// Need to generate a large number of unique, anonymous labels that indicate the targets of jumps and conditional branches.

int label_create();
const char* label_name( int label );

// symbol to code generation
const char* symbol_codegen(struct symbol* s);

// TODO
// Add a reg field to the AST or DAG node structure, which will hold the number of a register returned by scratch_alloc
// When visiting each node, emit an instruction and place into the reg field the number of the register containing that value.
// When the node is no longer needed, call scratch_free to release
#endif