#include "codegen.h"

struct Register* create_register(register_state_t state, const char* name) {
	struct Register* reg = malloc(sizeof(struct Register));
	if (!reg) return NULL;

	reg->state = state;
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

	static const char* reg_names[] = {"rbx", "r8", "r9", "r10", "r11"};

	for (int r = 0; r < sregs->capacity; r++) {
		sregs->registers[r] = create_register(REGISTER_FREE, reg_names[r]);
	}

	return sregs;
}

struct AsmWriter* create_asm_writer(const char* filename) {
	struct AsmWriter* writer = malloc(sizeof(struct AsmWriter));
	if (!writer) return NULL;

	writer->filename = strdup(filename);
	if (!writer->filename) {
		free(writer);
		return NULL;
	}

	writer->file = fopen(filename, "w+");
	if (!writer->file) {
		free((void*)writer->filename);
		free(writer);
		return NULL;
	}

	fprintf(writer->file, "section .data\n");
	writer->data_section = ftell(writer->file);

	fprintf(writer->file, "\n\nsection .text\n");
	writer->text_section = ftell(writer->file);
	fprintf(writer->file, "global _start\n_start:\n");

	writer->current_pos = ftell(writer->file);

	return writer;
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

	switch (sym->kind) {
		case SYMBOL_LOCAL:	
			if (sym->type->kind == TYPE_INTEGER) {
				snprintf(buffer, sizeof(buffer), "rbp - %d", 8 * (sym->u.local_var_index + 1));

			}
			return buffer;  

		case SYMBOL_PARAM:
			if (sym->type->kind == TYPE_INTEGER) {
				snprintf(buffer, sizeof(buffer), "rbp %d", 16 + (sym->u.param_index * 8));
			}			
			return buffer;

		case SYMBOL_GLOBAL:
			return sym->name;

	}
}

void expr_codegen(struct RegisterTable* sregs, struct expr* e) {
	if (!sregs || !e) return;

	switch (e->kind) {

		case EXPR_ADD:
			expr_codegen(sregs,e->left);
			expr_codegen(sregs,e->right);

			printf("add %s, %s\n",
				scratch_name(sregs, e->left->reg),
				scratch_name(sregs, e->right->reg));

			e->reg = e->left->reg;
			scratch_free(sregs, e->right->reg);
			break;

		case EXPR_SUB:
			expr_codegen(sregs, e->left);
			expr_codegen(sregs, e->right);
			printf("sub %s, %s\n",
				scratch_name(sregs, e->left->reg),
				scratch_name(sregs, e->right->reg));

			e->reg = e->left->reg;
			scratch_free(sregs, e->right->reg);
			break;

		case EXPR_ASSIGNMENT: {
			expr_codegen(sregs, e->right);

			if (e->left->kind == EXPR_NAME && e->left->symbol->kind == SYMBOL_LOCAL) {
				printf("mov [%s], %s\n",
				symbol_codegen(e->left->symbol),
				scratch_name(sregs, e->right->reg));
				e->reg = e->right->reg;

			} else {
				printf("mov [%s], %s\n",
					symbol_codegen(e->left->symbol),
					scratch_name(sregs, e->right->reg));
				e->reg = e->right->reg;
			}
			scratch_free(sregs, e->right->reg);
			break;
		}

		case EXPR_NAME:
			e->reg = scratch_alloc(sregs);
			if (e->symbol->kind == SYMBOL_LOCAL) {
				printf("mov %s, [%s]\n",
					scratch_name(sregs, e->reg),
					symbol_codegen(e->symbol));

			} else if (e->symbol->kind == SYMBOL_PARAM) {
				printf("mov %s, [%s]\n",
					scratch_name(sregs, e->reg),
					symbol_codegen(e->symbol));
			} else {
				printf("mov %s, [%s]\n",
					scratch_name(sregs, e->reg),
					symbol_codegen(e->symbol));
			}
			break;

		case EXPR_INTEGER:
			e->reg = scratch_alloc(sregs);
			printf("mov %s, %d\n",
				scratch_name(sregs, e->reg),
				e->integer_value);
			break;

		// case EXPR_ARRAY:
		// 	int label_num = label_create();
		// 	const char* name = label_create(label_num);


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
		if (d->value) {
			int reg = scratch_alloc(sregs);
			expr_codegen(sregs, d->value);

			printf("mov [%s], %s\n",
				symbol_codegen(d->symbol),
				scratch_name(sregs, d->value->reg));

			scratch_free(sregs, d->value->reg);
			scratch_free(sregs, reg);

		}
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