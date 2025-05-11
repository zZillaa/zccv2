#ifndef CODEGEN_H
#define CODEGEN_H
#include "ast.h"
#include "IR.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_SCRATCH_REGISTERS 10


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
	long current_text_pos;
	long current_data_pos;
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
} section_t;

// For printf calls
struct FormatQueue {
	char** items;
	int capacity;
	int size;
	int front;
	int rear;
};

struct ArgumentQueue {
	struct expr** items;
	int capacity;
	int size;
	int front;
	int rear;
};

struct FormatQueue* create_format_queue(int capacity);
void enqueue_format(struct FormatQueue* q, const char* format);
char* dequeue_format(struct FormatQueue* q);
int is_format_queue_empty(struct FormatQueue* q);
void free_format_queue(struct FormatQueue* q);

struct ArgumentQueue* create_arg_queue(int capacity);
void enqueue_arg(struct ArgumentQueue* q, struct expr* e);
struct expr* dequeue_arg(struct ArgumentQueue* q);
int is_arg_queue_empty(struct ArgumentQueue* q);
void free_argument_queue(struct ArgumentQueue* q);
//

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
void asm_to_write_section(struct AsmWriter* writer, const char* content, section_t directive_kind);
void free_asm_writer(struct AsmWriter* writer);


int label_create();
const char* label_name( int label );
size_t compute_offset(struct symbol* symbol, int* index);

char* symbol_codegen(struct symbol* sym);
void expr_codegen(struct RegisterTable* sregs, struct AsmWriter* writer, struct expr* e);
void stmt_codegen(struct RegisterTable* sregs, struct AsmWriter* writer, struct stmt* s);
void decl_codegen(struct RegisterTable* sregs, struct AsmWriter* writer, struct decl* d, bool is_local);

void codegen_dag_node(struct RegisterTable* sregs, struct AsmWriter* writer, struct dag_node* node);
void codegen_dag(struct RegisterTable* sregs, struct AsmWriter* writer, struct DAG* dag);
void codegen_block(struct RegisterTable* sregs, struct AsmWriter* writer, struct basic_block* block);
void codegen_CFG(struct RegisterTable* sregs, struct AsmWriter* writer, struct CFG* cfg);
void process_CFG(struct RegisterTable* sregs, struct AsmWriter* writer, struct CFG* cfg);

void free_register(struct Register* r);
void free_register_table(struct RegisterTable* sregs);
#endif