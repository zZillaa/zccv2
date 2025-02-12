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
	writer->start_section = ftell(writer->file);

	writer->current_pos = ftell(writer->file);

	return writer;
}

long get_pos_from_directive(struct AsmWriter* writer, section_t directive_kind) {
	if (!writer) return -1;

	switch (directive_kind) {
		case DATA_DIRECTIVE:
			return writer->data_section;

		case TEXT_DIRECTIVE:
			return writer->text_section;

		case GLOBAL_DIRECTIVE:
			return writer->start_section;

		default:
			return writer->current_pos;
	}
}

void asm_to_write_section(struct AsmWriter* writer, char* content, section_t directive_kind) {
    if (!writer || !content) return;

    // Save current position
    long current_position = ftell(writer->file);
    printf("Current position: %ld\n", current_position);
    
    // Read the entire file content after the insertion point
    long insertion_point = get_pos_from_directive(directive_kind);
    printf("Insertion point: %ld\n", insertion_point);
    fseek(writer->file, insertion_point, SEEK_SET);
    
    // Read existing content after insertion point
    char* buffer = NULL;
    long remaining_size = 0;
    if (fseek(writer->file, 0, SEEK_END) == 0) {
        long end_pos = ftell(writer->file);
        remaining_size = end_pos - insertion_point;
        if (remaining_size > 0) {
            buffer = malloc(remaining_size);
            if (buffer) {
                fseek(writer->file, insertion_point, SEEK_SET);
                fread(buffer, 1, remaining_size, writer->file);
            }
        }
    }
    
    // Write new content
    fseek(writer->file, insertion_point, SEEK_SET);
    fprintf(writer->file, "%s\n", content);
    
    // Update position tracker
    if (section_type == 1) {
        writer->current_pos = ftell(writer->file);
    } else {
        writer->data_section = ftell(writer->file);
    }
    
    // Write back the remaining content
    if (buffer && remaining_size > 0) {
        fwrite(buffer, 1, remaining_size, writer->file);
        free(buffer);
    }
    
    // Restore original position
    fseek(writer->file, current_position, SEEK_SET);
    long back_to_current = ftell(writer->file);
    printf("Back to current: %ld\n", back_to_current);
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

char* symbol_codegen(struct symbol* sym) {
	if (!sym) {
		fprintf(stderr, "Error: symbol is NULL\n");
		return NULL;
	}

	static char buffer[32];

	switch (sym->kind) {
		case SYMBOL_LOCAL:	
			if (sym->type->kind == TYPE_INTEGER) {
				// printf("Debug - Variable: %s, kind: %d, Local index: %d\n",
					// sym->name, sym->kind, sym->u.local_var_index);
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


byte_size_t get_byte_size(expr_t kind) {
	switch (kind) {
		case EXPR_ARRAY_VAL:
			return DQ;

		case EXPR_STRING:
			return DW;

		case EXPR_BOOLEAN:
		case EXPR_CHARACTER:
			return DB;

		default:
			fprintf(stderr, "Error: Cannot retrieve correct number of bytes 	due to unknown expression type\n");
			return BYTE_UNKNOWN;
	}
}

request_byte_t get_request_type(byte_size_t kind) {
	switch (kind) {
		case DB:
			return RESB;
		case DD:
			return RESD;
		case DW:
			return RESW;
		case DQ:
			return RESQ;
		default:
			fprintf(stderr, "Error: Cannot write proper request instruction due to unknown byte size\n");
			return RES_UNKNOWN;
	}
}

char* request_to_string(byte_size_t kind) {
	static char buffer[32];

	switch (kind) {
		case DB:
			snprintf(buffer, sizeof(buffer), "resb");
			return buffer;

		case DD:
			snprintf(buffer, sizeof(buffer), "resd");
			return buffer;

		case DW:
			snprintf(buffer, sizeof(buffer), "resw");
			return buffer;

		case DQ:
			snprintf(buffer, sizeof(buffer), "resq");
			return buffer;
	}
}

char* bytes_to_string(byte_size_t kind) {
	static char buffer[32];

	switch (kind) {
		case DB:
			snprintf(buffer, sizeof(buffer), "db");
			return buffer;
		case DD:
			snprintf(buffer, sizeof(buffer), "dd");
			return buffer;

		case DW:
			snprintf(buffer, sizeof(buffer), "dw");
			return buffer;

		case DQ:
			snprintf(buffer, sizeof(buffer), "dq");
			return buffer;
	}
}

void expr_codegen(struct RegisterTable* sregs, struct AsmWriter* writer, struct expr* e) {
	if (!sregs || !e) return;


	char buffer[256];

	switch (e->kind) {
		case EXPR_DIV:
			expr_codegen(sregs, writer, e->left);
			expr_codegen(sregs, writer, e->right);

			snprintf(buffer, sizeof(buffer), "\tmov rax, %s", scratch_name(sregs, e->left->reg));
			asm_to_write_section(writer, buffer, GLOBAL_DIRECTIVE);

			snprintf(buffer, sizeof(buffer), "\tcqo");
			asm_to_write_section(writer, buffer, GLOBAL_DIRECTIVE);

			snprintf(buffer, sizeof(buffer), "\tidiv %s", scratch_name(sregs, e->right->reg));
			asm_to_write_section(writer, buffer, GLOBAL_DIRECTIVE);

			e->reg = e->left->reg;
			snprintf(buffer, sizeof(buffer), "\tmov %s, rax", scratch_name(sregs, e->reg));
			asm_to_write_section(writer, buffer, GLOBAL_DIRECTIVE);

			scratch_free(sregs, e->left->reg);
			scratch_free(sregs, e->right->reg);
			break; 
		/*

		case EXPR_MUL:
			printf("ENTERING EXPR_MUL codegen\n");

			expr_codegen(sregs, writer, e->left);
			printf("Left reg: %d (%s)\n", e->left->reg, scratch_name(sregs, e->left->reg));
			
			expr_codegen(sregs, writer, e->right);
			printf("Right reg: %d (%s)\n", e->right->reg, scratch_name(sregs, e->right->reg));
			
			snprintf(buffer, sizeof(buffer), "\tmov rax, %s", scratch_name(sregs, e->left->reg));
			asm_to_write_section(writer, buffer, 1);

			snprintf(buffer, sizeof(buffer), "\tmul %s", scratch_name(sregs, e->right->reg));
			asm_to_write_section(writer, buffer, 1);

			e->reg = e->left->reg;
			snprintf(buffer, sizeof(buffer), "\tmov %s, rax", scratch_name(sregs, e->reg));
			asm_to_write_section(writer, buffer, 1);
			
			// scratch_free(sregs, e->left->reg);
			scratch_free(sregs, e->right->reg);
			printf("Exiting EXPR_MUL codegen, e->reg: %d (%s)\n", e->reg, scratch_name(sregs, e->reg));
			break;	
		*/

		case EXPR_MUL:
		    expr_codegen(sregs, writer, e->left);
		    expr_codegen(sregs, writer, e->right);
		    
		    // Save the current value in rax if we need it
		    int save_reg = -1;
		    if (strcmp(scratch_name(sregs, e->left->reg), "rax") != 0) {
		        save_reg = scratch_alloc(sregs);
		        snprintf(buffer, sizeof(buffer), "\tmov %s, rax", scratch_name(sregs, save_reg));
		        asm_to_write_section(writer, buffer, GLOBAL_DIRECTIVE);
		    }
		    
		    // Move left operand to rax if it's not already there
		    if (strcmp(scratch_name(sregs, e->left->reg), "rax") != 0) {
		        snprintf(buffer, sizeof(buffer), "\tmov rax, %s", scratch_name(sregs, e->left->reg));
		        asm_to_write_section(writer, buffer, GLOBAL_DIRECTIVE);
		    }
		    
		    // Perform multiplication
		    snprintf(buffer, sizeof(buffer), "\tmul %s", scratch_name(sregs, e->right->reg));
		    asm_to_write_section(writer, buffer, GLOBAL_DIRECTIVE);
		    
		    // Move result to a new register if needed
		    e->reg = scratch_alloc(sregs);
		    if (strcmp(scratch_name(sregs, e->reg), "rax") != 0) {
		        snprintf(buffer, sizeof(buffer), "\tmov %s, rax", scratch_name(sregs, e->reg));
		        asm_to_write_section(writer, buffer, GLOBAL_DIRECTIVE);
		    }
		    
		    // Restore original rax value if we saved it
		    if (save_reg != -1) {
		        snprintf(buffer, sizeof(buffer), "\tmov rax, %s", scratch_name(sregs, save_reg));
		        asm_to_write_section(writer, buffer, GLOBAL_DIRECTIVE);
		        scratch_free(sregs, save_reg);
		    }
		    
		    scratch_free(sregs, e->left->reg);
		    scratch_free(sregs, e->right->reg);
		    break;
		
	 	case EXPR_SUB:
		case EXPR_ADD:
			expr_codegen(sregs, writer, e->left);
			expr_codegen(sregs, writer, e->right);

			snprintf(buffer, sizeof(buffer), "\t%s %s, %s", 
				(e->kind == EXPR_ADD) ? "add" : "sub",
				scratch_name(sregs, e->left->reg), 
				scratch_name(sregs, e->right->reg));

			asm_to_write_section(writer, buffer, GLOBAL_DIRECTIVE);
		
			e->reg = e->left->reg;
			scratch_free(sregs, e->right->reg);
			break;

		case EXPR_ASSIGNMENT:
		    expr_codegen(sregs, writer, e->right);

		    snprintf(buffer, sizeof(buffer), "\tmov [%s], %s",
		        symbol_codegen(e->left->symbol),
		        scratch_name(sregs, e->right->reg));

		    asm_to_write_section(writer, buffer, GLOBAL_DIRECTIVE);
		    e->reg = e->right->reg;
		    scratch_free(sregs, e->right->reg);
		    break;

		case EXPR_NAME:
			e->reg = scratch_alloc(sregs);
			// printf("Codegen - Symbol address: %p, name: %s, index: %d\n",
			// 	(void*)e->symbol, e->symbol->name, e->symbol->u.local_var_index);

			snprintf(buffer, sizeof(buffer), "\tmov %s, [%s]",
				scratch_name(sregs, e->reg),
				symbol_codegen(e->symbol));

			asm_to_write_section(writer, buffer, GLOBAL_DIRECTIVE);
			break;

		case EXPR_INTEGER:
			e->reg = scratch_alloc(sregs);
			snprintf(buffer, sizeof(buffer), "\tmov %s, %d",
				scratch_name(sregs, e->reg),
				e->integer_value);

			asm_to_write_section(writer, buffer, GLOBAL_DIRECTIVE);
			break;

		case EXPR_ARRAY_VAL:
			printf("Found array value: %d\n", e->integer_value);
			break;

		case EXPR_ARRAY:
			int label_num = label_create();
			// Do not need to generate labels for arrays, just use their names.
			// const char* id = label_name(label_num);
			const char* name = strdup(e->name);
			int array_size;
			if (e->left) {
				array_size = e->left->integer_value;
			}
			expr_codegen(sregs, writer, e->left);
			

			printf("e->right is: %p\n", (void*)e->right);

			byte_size_t byte_t = get_byte_size(e->left->kind);
			request_byte_t request_t = get_request_type(byte_t);

			if (!e->right) {
				printf("No initialization values\n");
				snprintf(buffer, sizeof(buffer), "\t%s: %s %d",
					name,
					request_to_string(byte_t), 
					array_size);

				asm_to_write_section(writer, buffer, DATA_DIRECTIVE);

			} else {
				char temp_buffer[1024] = {0};
				snprintf(temp_buffer, sizeof(temp_buffer), "\t%s %s",
					name,
					bytes_to_string(byte_t));	

				struct expr* current = e->right;

				while (current) {
					char value_buffer[32];
					snprintf(value_buffer, sizeof(value_buffer),
						current->right ? " %d," : " %d",
						current->integer_value);

					strcat(temp_buffer, value_buffer);
					current = current->right;
				}
				asm_to_write_section(writer, temp_buffer, DATA_DIRECTIVE);
			}

			scratch_free(sregs, e->left->reg); 
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
			// scratch_free(sregs, s->expr->reg);
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

		} else if (d->type->kind == TYPE_ARRAY) {
			if (d->value && d->value->kind == EXPR_ARRAY) {
				expr_codegen(sregs, writer, d->value);
			}
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