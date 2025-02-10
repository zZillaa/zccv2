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

	static const char* reg_names[] = {"rax", "rbx", "r8", "r9", "r10", "r11"};

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

	fprintf(writer->file, "\nsection .text\n");
	writer->text_section = ftell(writer->file);
	fprintf(writer->file, "global _start\n\n_start:\n");
	writer->current_pos = ftell(writer->file);


	return writer;
}

void asm_to_write_section(struct AsmWriter* writer, const char* content, int section_type) {
	if (!writer || !content) return;

	long prev_location = ftell(writer->file);

	if (section_type == 1) {
		fseek(writer->file, writer->current_pos, SEEK_SET);
		fprintf(writer->file, "%s\n", content);

		writer->current_pos = ftell(writer->file);
		fseek(writer->file, prev_location, SEEK_SET);

	} else {
		fseek(writer->file, writer->data_section, SEEK_SET);
		fprintf(writer->file, "%s\n", content);

		writer->data_section = ftell(writer->file);
		fseek(writer->file, prev_location, SEEK_SET);
	}
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
				printf("Debug - Variable: %s, kind: %d, Local index: %d\n",
					sym->name, sym->kind, sym->u.local_var_index);
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

void expr_codegen(struct RegisterTable* sregs, struct AsmWriter* writer, struct expr* e) {
	if (!sregs || !e) return;

	printf("Expression kind: %d\n", e->kind);

	char buffer[256];

	switch (e->kind) {
		case EXPR_DIV:
			expr_codegen(sregs, writer, e->left);
			expr_codegen(sregs, writer, e->right);

			snprintf(buffer, sizeof(buffer), "\tmov rax, %s", scratch_name(sregs, e->left->reg));
			asm_to_write_section(writer, buffer, 1);

			snprintf(buffer, sizeof(buffer), "\tcqo");
			asm_to_write_section(writer, buffer, 1);

			snprintf(buffer, sizeof(buffer), "\tidiv %s", scratch_name(sregs, e->right->reg));
			asm_to_write_section(writer, buffer, 1);

			e->reg = e->left->reg;
			// snprintf(buffer, sizeof(buffer), "\tmov %s, rax", scratch_name(sregs, e->reg));
			// asm_to_write_section(writer, buffer, 1);

			// scratch_free(sregs, e->left->reg);
			scratch_free(sregs, e->right->reg);
			break; 

		case EXPR_MUL:
			expr_codegen(sregs, writer, e->left);
			expr_codegen(sregs, writer, e->right);

			snprintf(buffer, sizeof(buffer), "\tmov rax, %s", scratch_name(sregs, e->left->reg));
			asm_to_write_section(writer, buffer, 1);

			snprintf(buffer, sizeof(buffer), "\timul %s", scratch_name(sregs, e->right->reg));
			asm_to_write_section(writer, buffer, 1);

			e->reg = e->left->reg;
			// snprintf(buffer, sizeof(buffer), "\tmov %s, rax", scratch_name(sregs, e->reg));
			// asm_to_write_section(writer, buffer, 1);

			// scratch_free(sregs, e->left->reg);
			scratch_free(sregs, e->right->reg);
			break;	
			
	 	case EXPR_SUB:
		case EXPR_ADD:
			expr_codegen(sregs, writer, e->left);
			int left_reg = e->left->reg;

			expr_codegen(sregs, writer, e->right);
			int right_reg = e->right->reg;

			snprintf(buffer, sizeof(buffer), "\t%s %s, %s", 
				(e->kind == EXPR_ADD) ? "add" : "sub",
				scratch_name(sregs, left_reg), 
				scratch_name(sregs, right_reg));

			asm_to_write_section(writer, buffer, 1);
		
			e->reg = left_reg;
			scratch_free(sregs, right_reg);
			break;

		case EXPR_ASSIGNMENT:
		    expr_codegen(sregs, writer, e->right);

		    snprintf(buffer, sizeof(buffer), "\tmov [%s], %s",
		        symbol_codegen(e->left->symbol),
		        scratch_name(sregs, e->right->reg));

		    asm_to_write_section(writer, buffer, 1);
		    e->reg = e->right->reg;
		    scratch_free(sregs, e->right->reg);
		    break;

		case EXPR_NAME:
			e->reg = scratch_alloc(sregs);
			printf("Codegen - Symbol address: %p, name: %s, index: %d\n",
				(void*)e->symbol, e->symbol->name, e->symbol->u.local_var_index);

			snprintf(buffer, sizeof(buffer), "\tmov %s, [%s]",
				scratch_name(sregs, e->reg),
				symbol_codegen(e->symbol));

			asm_to_write_section(writer, buffer, 1);
			break;

		case EXPR_INTEGER:
			e->reg = scratch_alloc(sregs);
			snprintf(buffer, sizeof(buffer), "\tmov %s, %d",
				scratch_name(sregs, e->reg),
				e->integer_value);

			asm_to_write_section(writer, buffer, 1);
			break;
	}
}

void stmt_codegen(struct RegisterTable* sregs, struct AsmWriter* writer, struct stmt* s) {
	if (!sregs || !s) return;

	char buffer[256];

	switch (s->kind) {
		case STMT_DECL:
			decl_codegen(sregs, writer, s->decl);
			break;

		case STMT_EXPR:
			expr_codegen(sregs, writer, s->expr);
			scratch_free(sregs, s->expr->reg);
			break;
		// case STMT_RETURN:
		// 	expr_codegen(sregs, writer, s->expr);
		// 	snprintf(buffer, sizeof(buffer), "\tmov rax, %s", scratch_name(sregs, s->expr->reg));
		// 	asm_to_write_section(writer, buffer, 1);

		// 	scratch_free(sregs, s->expr->reg);
		// 	break;
	}	
	stmt_codegen(sregs, writer, s->next);
}

void decl_codegen(struct RegisterTable* sregs, struct AsmWriter* writer, struct decl* d) {
	if (!sregs || !d) return;

	char buffer[256];
	
	while (d) {
		if (d->type->kind == TYPE_FUNCTION) {
			
			snprintf(buffer, sizeof(buffer), "\tpush rbp\n\tmov rbp, rsp\n\tsub rsp, %d\n", 32);
			asm_to_write_section(writer, buffer, 1);

			stmt_codegen(sregs, writer, d->code);

			int z_reg = -1;
            if (d->code->kind == STMT_EXPR && d->code->expr->kind == EXPR_ASSIGNMENT && d->code->expr->left->symbol != NULL && strcmp(d->code->expr->left->symbol->name, "z") == 0) {
                z_reg = d->code->expr->reg;
            }

            if(z_reg != -1) {
                snprintf(buffer, sizeof(buffer), "\tmov rax, %s", scratch_name(sregs, z_reg));
                asm_to_write_section(writer, buffer, 1);
            }

            snprintf(buffer, sizeof(buffer), "\n\tmov rsp, rbp\n\tpop rbp\n\tret\n\n\tmov rax, 60\n\txor rdi, rdi\n\tsyscall");
            asm_to_write_section(writer, buffer, 1);

		} else if (d->value) {
			expr_codegen(sregs, writer, d->value);

			snprintf(buffer, sizeof(buffer), "\tmov [%s], %s",
				symbol_codegen(d->symbol),
				scratch_name(sregs, d->value->reg));

			asm_to_write_section(writer, buffer, 1);

			scratch_free(sregs, d->value->reg);

		}

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

void free_asm_writer(struct AsmWriter* writer) {
	if (!writer) return;

	free((void*)writer->filename);
	if (writer->file) fclose(writer->file);
	free(writer);
}