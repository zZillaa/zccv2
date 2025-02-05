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

	scratch_registers_array->array = malloc(sizeof(struct register* ) * capacity);
	if (!scratch_registers_array->array) {
		fprintf(stderr, "Error: Failed to allocate space for registers array\n");
		return NULL;
	}

	scratch_registers_array->size = 0;
	scratch_register_array->capacity = capacity;


	return scratch_registers;
}

int scratch_alloc(struct scratch_resgisters* s, const char* name) {
	for (int r = 0; r < array->size; r++) {
		struct register* current = s->array[r]; 
		if (strcmp(current->name, name) == 0 && curent->state == FREE) {
			return r;
		}	
	}

	return -1;
}

void scratch_free(struct scratch_registers* s ,int r) {
	for (int i = 0; i < s->size; i++) {
		if (i == r) {
			s->array[r]->state = FREE;
			free((void*)s->array[r]->name);
			free(s->array[r]);
		}
	}
}


const char* scratch_name(struct scratch_registers* s, int r) {
	for (int i = 0; i < s->size; i++) {
		if (i == r) {
			return s->array[r]->name;
		}
	}

	return NULL;
}

const char* symbol_codegen(struct symbol* s) {
	if (!s) return NULL;

}

void stmt_codegen(struct stmt* s) {
	if (!s) return;

	switch (s->kind) {
		case STMT_EXPR:
			expr_codegen(s->expr);
			scratch_free(s->expr->reg);
			break;

		case STMT_DECL:
			decl_codegen(s->decl);
			break;

		case STMT_RETURN:
			expr_codegen(s->expr);
			printf("MOV %s, %%rax\n", scratch_name(s->expr->reg));
			// printf("JMP .%s_epilogue\n", function_name);

			scratch_free(s->expr->reg);
			break;


	}
	stmt_codegen(s->next);

}

void expr_codegen(struct expr* e) {
	if (!e) return;

	switch (e->kind) {
		case EXPR_NAME:
			e->reg = scratch_alloc();
			printf("MOVQ %s, %s\n",
				symbol_codegen(e->symbol),
				scratch_name(e->reg));

			break;

		case EXPR_ADD:
			expr_codegen(e->left);
			expr_codegen(e->right);
			printf("ADDQ %s, %s\n",
				scratch_name(e->left->reg),
				scratch_name(e->right->reg));
			e->reg = e->right->reg;
			scratch_free(e->left->reg);
			break;



		case EXPR_ASSIGNMENT:
			expr_codegen(e->left);
			printf("MOVQ %s, %s\n",
				scratch_name(e->left->reg);
				symbol_codegen(e->right->symbol));
			e->reg = e->left->reg;
			break;



			
	}
}

void decl_codegen(struct decl* d) {
	if (!d) return;

	while (d) {
		if (d->value) expr_codegen(d->value);

		if (d->code) stmt_codegen(d->code);

		d = d->next;
	}
}