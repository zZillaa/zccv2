#ifndef CODEGEN_H
#define CODEGEN_H
#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_SCRATCH_REGISTERS 6

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

struct AsmWriter {
	FILE* file;
	const char* filename;
	long data_section;
	long text_section;
	long start_section;
	long current_pos;
};

typedef enum {
	DB,
	DD,
	DW,
	DQ,
	BYTE_UNKNOWN
} byte_size_t;

typedef enum {
	RESB,
	RESD,
	RESW,
	RESQ,
	RES_UNKNOWN
} request_byte_t;

typedef enum {
	DATA_DIRECTIVE,
	TEXT_DIRECTIVE,
	GLOBAL_DIRECTIVE
} section_t;


byte_size_t get_byte_type(expr_t kind);
request_byte_t get_request_type(byte_size_t kind);
char* bytes_to_string(byte_size_t kind);
char* request_to_string(byte_size_t kind);

int scratch_alloc(struct RegisterTable* sregs);
void scratch_free(struct RegisterTable* sregs, int r);
const char* scratch_name(struct RegisterTable* sregs, int r);
struct Register* create_register(register_state_t state, const char* name);
struct RegisterTable* create_register_table();

struct AsmWriter* create_asm_writer(const char* filename);
long get_pos_from_directive(struct AsmWriter* writer, section_t directive_kind);
void asm_write_to_section(struct AsmWriter* writer, const char* content, int section_type);
void free_asm_writer(struct AsmWriter* writer);


int label_create();
const char* label_name( int label );

char* symbol_codegen(struct symbol* sym);
void expr_codegen(struct RegisterTable* sregs, struct AsmWriter* writer, struct expr* e);
void stmt_codegen(struct RegisterTable* sregs, struct AsmWriter* writer, struct stmt* s);
void decl_codegen(struct RegisterTable* sregs, struct AsmWriter* writer, struct decl* d);

void free_register(struct Register* r);
void free_register_table(struct RegisterTable* sregs);
#endif