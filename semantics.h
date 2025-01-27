#ifndef SEMANTICS_H
#define SEMANTICS_H
#include "parser.h"
#include <stdbool.h>
#define STACK_SIZE 100


void scope_enter(struct stack* stack, struct symbol* symbol);
void scope_exit(struct stack* stack);
void scope_bind(struct stack* stack, struct symbol* symbol);
bool is_empty(struct stack* stack);
int scope_level(struct stack* stack);

void insert_symbol(struct symbol_table* table, struct symbol* symbol);
bool is_symbol_redeclared(struct symbol_table* table, char* name);
struct symbol_table* copy_symbol_table(struct symbol_table* original); 
void free_stack(struct stack* stack);
void free_symbol(struct symbol* symbol);

struct stack* create_stack();
struct symbol* create_symbol(symbol_t kind, struct type* type, char* name);
struct symbol* scope_lookup(struct stack* stack, char* name);
struct symbol* scope_lookup_current(struct stack* stack, char* name);

void print_symbol_table(struct stack* stack);

// Name resolution methods
void program_resolve(struct program* p, struct stack* stack);
void decl_resolve(struct decl* d, struct stack* stack);
void expr_resolve(struct expr* e, struct stack* stack);
void stmt_resolve(struct stmt* stmt, struct stack* stack);
void param_list_resolve(struct param_list* params, struct stack* stack);


struct type* type_create(type_t kind, struct type* subtype, struct param_list* params);
bool type_equals(struct type* a, struct type* b);
void type_delete(struct type* t);
struct type* expr_typecheck(struct expr* e, struct stack* stack);
void decl_typecheck(struct decl* d, struct stack* stack);
void stmt_typecheck(struct stmt* s, struct stack* stack);
void program_typecheck(struct program* p, struct stack* stack);
struct type* type_copy(struct type* t); // FOR Compound types
#endif