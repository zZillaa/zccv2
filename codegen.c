#include "codegen.h"

struct Register* create_register(register_state_t state, const char* name) {
	struct Register* reg = malloc(sizeof(struct Register));
	if (!reg) return NULL;

	reg->state = state;
	printf("This register has state: %d\n", state);
	reg->name = strdup(name);
	if (!reg->name) {
		fprintf(stderr, "Error: Could not allocate space for register name\n");
		free(reg);
		return NULL;
	}

	return reg;

}

struct RegisterTable* create_register_table() {
	struct RegisterTable* sregs = malloc(sizeof(struct RegisterTable));
	if (!sregs) return NULL;

	sregs->capacity = MAX_SCRATCH_REGISTERS;

	static const char* reg_names[] = {"%rbx", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"};

	for (int r = 0; r < sregs->capacity; r++) {
		sregs->registers[r] = create_register(REGISTER_FREE, reg_names[r]);
	}

	return sregs;
}

int scratch_alloc(struct RegisterTable* sregs) {
	if (!sregs) return -1;

	for (int r = 0; r < sregs->capacity; r++) {
		if (sregs->registers[r]->state == REGISTER_FREE) {
			sregs->registers[r]->state = REGISTER_USED;
			return r;
		}
	}

	fprintf(stderr, "Error: Cannot find a free register\n");

	return -1;
}

void scratch_free(struct RegisterTable* sregs, int r) {
	if (!sregs || r < 0 || r > sregs->capacity) return;

	sregs->registers[r]->state = REGISTER_FREE;
}

const char* scratch_name(struct RegisterTable* sregs, int r) {
	if (!sregs || r < 0 || r > sregs->capacity) return NULL;

	return sregs->registers[r]->name; 
}

int label_create() {
	return label_counter++;
}

const char* label_name(int label) {
	static char buffer[32];
	snprintf(buffer, sizeof(buffer), ".L%d", label);
	return buffer;
}

const char* symbol_codegen(struct symbol* sym) {
	if (!sym) {
		fprintf(stderr, "Error: symbol is NULL\n");
		return NULL;
	}

	static char buffer[32];
	static int offset = 0;

	switch (sym->kind) {
		case SYMBOL_LOCAL:	
			if (sym->type->kind == TYPE_INTEGER) {
				offset -= 8;
				snprintf(buffer, sizeof(buffer), "%d(%%rbp)", offset);

			}
			return buffer;  

		case SYMBOL_PARAM:
			if (sym->type->kind == TYPE_INTEGER) {
				offset -= 8;
				snprintf(buffer, sizeof(buffer), "%d(%%rbp)", offset);
			}			
			return buffer;

		case SYMBOL_GLOBAL:
			return sym->name;

	}
}

void expr_codegen(struct RegisterTable* sregs, struct expr* e) {
	if (!sregs || !e) return;

	switch (e->kind) {
		case EXPR_NAME:
			e->reg = scratch_alloc(sregs);
			printf("MOVQ %s, %s\n",
				symbol_codegen(e->symbol),
				scratch_name(sregs, e->reg));
			break;

		case EXPR_ADD:
			expr_codegen(sregs,e->left);
			expr_codegen(sregs,e->right);

			printf("ADDQ %s, %s\n",
				scratch_name(sregs, e->right->reg),
				scratch_name(sregs, e->left->reg));

			e->reg = e->left->reg;
			scratch_free(sregs, e->right->reg);
			break;

		case EXPR_SUB:
			expr_codegen(sregs, e->left);
			expr_codegen(sregs, e->right);
			printf("SUBQ %s, %s\n",
				scratch_name(sregs, e->right->reg),
				scratch_name(sregs, e->left->reg));

			e->reg = e->left->reg;
			scratch_free(sregs, e->right->reg);
			break;

		case EXPR_ASSIGNMENT:
			expr_codegen(sregs, e->right);
			printf("MOVQ %s, %s\n",
				scratch_name(sregs, e->right->reg),
				symbol_codegen(e->left->symbol));
			e->reg = e->right->reg;
			break;

		case EXPR_INTEGER:
			e->reg = scratch_alloc(sregs);
			printf("MOVQ $%d, %s\n",
				e->integer_value,
				scratch_name(sregs, e->reg));
			break;
	}
}

void stmt_codegen(struct RegisterTable* sregs, struct stmt* s) {
	if (!sregs || !s) return;

	switch (s->kind) {
		case STMT_DECL:
			decl_codegen(sregs,s->decl);
			break;

		case STMT_EXPR:
			expr_codegen(sregs,s->expr);
			scratch_free(sregs, s->expr->reg);
			break;
	}	
	stmt_codegen(sregs, s->next);
}

void decl_codegen(struct RegisterTable* sregs, struct decl* d) {
	if (!sregs || !d) return;

	while (d) {
		if (d->value) expr_codegen(sregs, d->value);

		if (d->code) stmt_codegen(sregs, d->code);

		d = d->next;
	}

}

void free_register(struct Register* reg) {
	if (!reg) return;

	free((void*)reg->name);
	free(reg);
}

void free_register_table(struct RegisterTable* sregs) {
	if (!sregs) return;

	for (int r = 0; r < sregs->capacity; r++) {
		free_register(sregs->registers[r]);
	}

	free(sregs);
}