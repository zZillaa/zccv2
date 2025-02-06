#include "codegen.h"

struct register* create_register(const char* name, state register_s) {
	struct register* memcell_sequence = malloc(sizeof(struct register));
	if (!memcell_sequence) {
		fprintf(stderr, "Error: Failed to allocate space for register\n");
		return NULL;
	}

	register->size = REGISTER_WIDTH;
	register->name = strdup(name);
	if (!register->name) {
		fprintf(stderr, "Error: Failed to duplicate name for register\n");
		return NULL;
	}
	register->state = register_s;

	return register;

}

struct scratch_registers* create_scratch_registers(int capacity) {
	struct scratch_registers* scratch_registers_array = malloc(sizeof(struct scratch_registers));
	if (!scratch_registers_array) {
		fprintf(stderr, "Error: Failed to allocate space for scratch registers\n");
		return NULL;
	}

	scratch_registers_array->registers = malloc(sizeof(struct register* ) * capacity);
	if (!scratch_registers_array->array) {
		fprintf(stderr, "Error: Failed to allocate space for registers array\n");
		return NULL;
	}

	scratch_registers_array->size = 0;
	scratch_register_array->capacity = capacity;


	return scratch_registers;
}

int scratch_alloc(struct scratch_resgisters* s, const char* name) {
	for (int r = 0; r < s->size; r++) {
		struct register* current = s->array[r];
		if (current->state == FREE) {
			current->state = USED;
			return r;
		}
	}
	return -1;
}

void scratch_free(struct scratch_registers* s ,int r) {
	s->registers[r]->state = FREE;
}


const char* scratch_name(struct scratch_registers* s, int r) {
	if (!s) return NULL;

	return s->registers[r]->name;
}

const char* symbol_codegen(struct symbol* s) {
	if (!s) return NULL;

		

}

void stmt_codegen(struct scratch_registers* s, struct stmt* stmt) {
	if (!stmt) return;

	switch (stmt->kind) {
		case STMT_EXPR:
			expr_codegen(s, stmt->expr);
			scratch_free(s, stmt->expr->reg);
			break;

		case STMT_DECL:
			decl_codegen(s, stmt->decl);
			break;

		case STMT_RETURN:
			expr_codegen(s, stmt->expr);
			printf("MOV %s, %%rax\n", scratch_name(s, stmt->expr->reg));
			// printf("JMP .%s_epilogue\n", function_name);

			scratch_free(s, stmt->expr->reg);
			break;


	}
	stmt_codegen(stmt->next);

}

void expr_codegen(struct scratch_registers* s, struct expr* e) {
	if (!e) return;

	switch (e->kind) {
		case EXPR_NAME:
			e->reg = scratch_alloc();
			printf("MOVQ %s, %s\n",
				symbol_codegen(s, e->symbol),
				scratch_name(s, e->reg));

			break;

		case EXPR_DIV:
			expr_codegen(s, e->left->reg);
			printf("IDIVQ %s\n",
				scratch_name(s, e->left->reg));
			e->reg = e->right->reg;
			scratch_name(s, e->left->reg);
			break;

		case EXPR_MUL:
			expr_codegen(s, e->left->reg);
			printf("IMULQ %s\n",
				scratch_name(s, e->left->reg));
			e->reg = e->right->reg;
			scratch_name(s, e->left->reg);
			break;
		
		case EXPR_SUB:
			expr_codegen(s, e->left);
			expr_codegen(s, e->right);
			printf("SUBQ %s, %s\n",
				scratch_name(s, e->left->reg),
				scratch_name(s, e->right->reg));
			e->reg = e->right->reg;
			scratch_name(s, e->left->reg);
			break;

			break;
		case EXPR_ADD:
			expr_codegen(s, e->left);
			expr_codegen(s, e->right);
			printf("ADDQ %s, %s\n",
				scratch_name(s, e->left->reg),
				scratch_name(s, e->right->reg));
			e->reg = e->right->reg;
			scratch_free(s, e->left->reg);
			break;

		case EXPR_INCREMENT:
		case EXPR_DECREMENT:
			expr_codegen(s, e->left);
			printf("MOVQ %s, %s\n",
				scratch_name(s, e->left->reg),
				scratch_name(s, e->right->reg));
			e->reg = e->right->reg;
			scratch_free(s, e->left->reg);
			break;


		case EXPR_ASSIGNMENT:
			expr_codegen(s, e->left);
			printf("MOVQ %s, %s\n",
				scratch_name(s, e->left->reg);
				symbol_codegen(s, e->right->symbol));
			e->reg = e->left->reg;
			scratch_free(s, e->left->reg);
			break;

		case EXPR_FLOAT:
		case EXPR_INTEGER:
			expr_codegen(e);
			printf("MOVQ %s, %s\n",
				scratch_name(s, e->reg),
				symbol_codegen(s, e->symbol));
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