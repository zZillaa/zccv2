#include "codegen.h"

struct register* create_register(register_t type, const char* name, register_state_t state) {
	struct register* reg = malloc(sizeof(struct register));
	if (!reg) {
		fprintf(stderr, "Error: Failed to allocate space for register\n");
		return NULL;
	}

	reg->size = REGISTER_WIDTH;
	reg->type = type;
	reg->state = register_state_t;
	reg->name = strdup(name);
	if (!reg->name) {
		fprintf(stderr, "Error: Failed to duplicate name for register\n");
		free(reg);
		return NULL;
	}

	return reg;

}

struct scratch_registers* create_scratch_registers(int capacity) {
	struct scratch_registers* sregs = malloc(sizeof(struct scratch_registers));
	if (!sregs) {
		fprintf(stderr, "Error: Failed to allocate space for scratch registers\n");
		return NULL;
	}

	for (int i = 0; i < MAX_SCRATCH_REGISTERS; i++) {
		sregs->registers[i] = NULL;
	}

	sregs->size = 0;
	sregs->capacity = MAX_SCRATCH_REGISTERS;

	return sregs;
}

int scratch_alloc(struct scratch_registers* s) {
	if (!s) return -1;

	for (int r = 0; r < s->size; r++) {
		if (s->registers[r] && s->registers[r]->state == REGISTER_FREE) {
			s->registers[r]->state = REGISTER_USED;
			return r;
		}
	}
	return -1;
}

void scratch_free(struct scratch_registers* s ,int r) {
	if (s && r >= 0 && r < s->size && s->registers[r]) {
		s->registers[r]->state = REGISTER_FREE;
	}
}


const char* scratch_name(struct scratch_registers* s, int r) {
	if (!s || r < 0 || r >= s->size || !s->registers[r]) return NULL;

	return s->registers[r]->name;
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
	if (!sym) return NULL;

	static char buffer[32];

	switch (s->kind) {
		case SYMBOL_LOCAL:
			snprintf(buffer, sizeof(buffer), "-8(%%rbp)");
			return buffer;

		case SYMBOL_PARAM:
			snprintf(buffer, sizeof(buffer), "16(%%rbp)");
			return buffer;

		case SYMBOL_GLOBAL:
			return sym->name;
		default:
			return NULL;
	}
}

void stmt_codegen(struct scratch_registers* s, struct stmt* stmt) {
	if (!stmt || !s) return;

	switch (stmt->kind) {
		case STMT_EXPR:
			expr_codegen(s, stmt->expr);
			if (stmt->expr && stmt->expr->reg >= 0) scratch_free(s, stmt->expr->reg);
			break;

		case STMT_DECL:
			decl_codegen(s, stmt->decl);
			break;

		case STMT_RETURN:
			expr_codegen(s, stmt->expr);
			if (stmt->expr && stmt->expr->reg >= 0) {

				printf("MOV %s, %%rax\n", scratch_name(s, stmt->expr->reg));
				scratch_free(s, stmt->expr->reg);
			}
			// printf("JMP .%s_epilogue\n", function_name);

			break;


	}
	stmt_codegen(stmt->next);

}

void expr_codegen(struct scratch_registers* s, struct expr* e) {
	if (!e || !s) return;

	int reg;
	switch (e->kind) {
		case EXPR_NAME:
			reg = scratch_alloc(s);
			if (reg >= 0) {
				e->reg = reg;
			printf("MOVQ %s, %s\n",
				symbol_codegen(e->symbol),
				scratch_name(s, e->reg));
			}

			break;

		case EXPR_DIV:
			expr_codegen(s, e->left);
			expr_codegen(s, e->right);
			if (e->left && e->left->reg >= 0) {
				printf("IDIV %s\n",
				scratch_name(s, e->left->reg));
				e->reg = e->right->reg;
				scratch_free(s, e->left->reg);
			}
			break;

		case EXPR_MUL:
			expr_codegen(s, e->left);
			expr_codegen(s, e->right);
			if (e->left && e->left->reg >= 0) {
				printf("IMUL %s\n",
					scratch_name(s, e->left->reg));
				e->reg = e->right->reg;
				scratch_free(s, e->left->reg);
			}
			break;
		
		case EXPR_SUB:
			expr_codegen(s, e->left);
			expr_codegen(s, e->right);
			if (e->left && e->right &&
				e->left->reg >= 0 && e->right->reg >= 0) {
				printf("SUBQ %s, %s\n",
					scratch_name(s, e->left->reg),
					scratch_name(s, e->right->reg));

				e->reg = e->right->reg;
				scratch_free(s, e->left->reg);
			}
			break;

		case EXPR_ADD:
			expr_codegen(s, e->left);
			expr_codegen(s, e->right);
			if (e->left && e->right && 
				e->left->reg >= 0 && e->right->reg >= 0) {
				printf("ADDQ %s, %s\n",
					scratch_name(s, e->left->reg),
					scratch_name(s, e->right->reg));
				e->reg = e->right->reg;
				scratch_free(s, e->left->reg);
			}
			break;

		case EXPR_INCREMENT:
		case EXPR_DECREMENT:
			expr_codegen(s, e->left);
			if (e->left && e->left->reg >= 0) {
				reg = scratch_alloc(s);
				if (reg >= 0) {
					printf("MOVQ %s, %s\n",
						scratch_name(s, e->left->reg),
						scratch_name(s, e->right->reg));
					e->reg = reg;
					scratch_free(s, e->left->reg);

				}
			}
			break;


		case EXPR_ASSIGNMENT:
			expr_codegen(s, e->left);
			if (e->left && e->left->reg >= 0) {
				printf("MOVQ %s, %d\n",
					scratch_name(s, e->left->reg);
					symbol_codegen(e->right->symbol));
				e->reg = e->left->reg;
			}
			break;

		case EXPR_INTEGER:
			reg = scratch_alloc(s);
			if (reg >= 0) {
				e->reg = reg;
				printf("MOVQ $%d, %s\n",
					e->integer_value,
					scratch_name(s, e->reg));
			}
			break;
	}
}

void decl_codegen(struct scratch_registers* s, struct decl* d) {
	if (!d) return;

	while (d) {
		if (d->value) expr_codegen(s, d->value);

		if (d->code) stmt_codegen(s, d->code);

		d = d->next;
	}
}