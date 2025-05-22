#ifndef CODEGEN_H
#define CODEGEN_H
#include "ast.h"
#include "IR.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_SCRATCH_REGISTERS 10
#define MAX_LABELS 100
#define NUM_FUNC_ARGUMENTS 15
#define NUM_PARAMS 15

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

struct FunctionArgumentStack {
	struct expr* args[NUM_FUNC_ARGUMENTS];
	int top;
};

struct FunctionParamStack {
	struct param_list* func_params[NUM_PARAMS];
	int top;
};

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

struct LabelStack {
	int labels[MAX_LABELS];
	int top;
};

struct CodegenContext {
	int loop_condition;
	bool in_loop;
};

struct FunctionParamStack* create_param_stack();
void push_param(struct FunctionParamStack* func_param_stack, struct param_list* params);

struct FunctionArgumentStack* create_arg_stack();
void push_arg(struct FunctionArgumentStack* func_arg_stack, struct expr* e);
int pop_arg(struct FunctionArgumentStack* func_arg_stack);

struct CodegenContext create_codegen_context(int loop_condition, bool in_loop);
struct LabelStack* create_label_stack();
void push_label(struct LabelStack* label_stack, int label);
int pop_label(struct LabelStack* label_stack);
int peek_label(struct LabelStack* label_stack, int offset);

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
void param_codegen(struct RegisterTable* sregs, struct AsmWriter* writer, struct param_list* params);
void expr_codegen(struct RegisterTable* sregs, struct AsmWriter* writer, struct expr* e, bool is_func_argument);
void stmt_codegen(struct RegisterTable* sregs, struct AsmWriter* writer, struct stmt* s, struct CodegenContext* context);
void decl_codegen(struct RegisterTable* sregs, struct AsmWriter* writer, struct decl* d, bool is_local);

void codegen_dag_node(struct RegisterTable* sregs, struct AsmWriter* writer, struct dag_node* node);
void codegen_dag(struct RegisterTable* sregs, struct AsmWriter* writer, struct DAG* dag);
void codegen_block(struct RegisterTable* sregs, struct AsmWriter* writer, struct basic_block* block);
void codegen_CFG(struct RegisterTable* sregs, struct AsmWriter* writer, struct CFG* cfg);
void process_CFG(struct RegisterTable* sregs, struct AsmWriter* writer, struct CFG* cfg);

void free_register(struct Register* r);
void free_register_table(struct RegisterTable* sregs);
#endif