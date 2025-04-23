#include "codegen.h"

static int label_counter = 0;

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

	static const char* reg_names[] = {"rax", "rbx", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};

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

	writer->data_section = ftell(writer->file);

	writer->text_section = ftell(writer->file);

	writer->current_text_pos = writer->text_section;

	writer->current_data_pos = writer->data_section;

	return writer;
}

long get_pos_from_directive(struct AsmWriter* writer, section_t directive_kind) {
	if (!writer) return -1;

	switch (directive_kind) {
		case DATA_DIRECTIVE:
			return writer->current_data_pos;

		case TEXT_DIRECTIVE:
			return writer->current_text_pos;
		
		default:
			return -1;
	}
}

void asm_to_write_section(struct AsmWriter* writer, const char* content, section_t directive_kind) {
    if (!writer || !content) return;

    // Always append to the current position
    fseek(writer->file, 0, SEEK_END);

    // Write the content with appropriate formatting
    if (directive_kind == DATA_DIRECTIVE) {
        fprintf(writer->file, "%s\n", content);
    } else {
        fprintf(writer->file, "%s\n", content); // Always add a newline for clarity
    }

    // Update position trackers
    if (directive_kind == DATA_DIRECTIVE) {
        writer->current_data_pos = ftell(writer->file);
    } else if (directive_kind == TEXT_DIRECTIVE) {
        writer->current_text_pos = ftell(writer->file);
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

char* symbol_codegen(struct symbol* sym) {
	if (!sym) {
		fprintf(stderr, "Error: symbol is NULL\n");
		return NULL;
	}

	static char buffer[32];

	switch (sym->kind) {
		case SYMBOL_LOCAL:	
			if (sym->type->kind == TYPE_INTEGER) {
				snprintf(buffer, sizeof(buffer), "rbp - %d", 4 * (sym->s.local_var_index + 1));

			}
			return buffer;  

		case SYMBOL_PARAM:
			if (sym->type->kind == TYPE_INTEGER) {
				snprintf(buffer, sizeof(buffer), "rbp - %d", 16 + (sym->s.param_index * 8));
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
			fprintf(stderr, "Error: Cannot retrieve correct number of bytes due to unknown expression type\n");
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
		// case EXPR_DIV:
		// 	expr_codegen(sregs, writer, e->left);
		// 	expr_codegen(sregs, writer, e->right);

		// 	snprintf(buffer, sizeof(buffer), "\tmov rax, %s", scratch_name(sregs, e->left->reg));
		// 	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

		// 	snprintf(buffer, sizeof(buffer), "\tcqo");
		// 	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

		// 	snprintf(buffer, sizeof(buffer), "\tidiv %s", scratch_name(sregs, e->right->reg));
		// 	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

		// 	e->reg = e->left->reg;
		// 	snprintf(buffer, sizeof(buffer), "\tmov %s, rax", scratch_name(sregs, e->reg));
		// 	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

		// 	scratch_free(sregs, e->left->reg);
		// 	scratch_free(sregs, e->right->reg);
		// 	break; 

		case EXPR_MUL:
		    expr_codegen(sregs, writer, e->left);
		    expr_codegen(sregs, writer, e->right);
		    
		    // Save the current value in rax if we need it
		    int save_reg = -1;
		    if (strcmp(scratch_name(sregs, e->left->reg), "rax") != 0) {
		        save_reg = scratch_alloc(sregs);
		        snprintf(buffer, sizeof(buffer), "\tmov %s, rax", scratch_name(sregs, save_reg));
		        asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
		    }
		    
		    // Move left operand to rax if it's not already there
		    if (strcmp(scratch_name(sregs, e->left->reg), "rax") != 0) {
		        snprintf(buffer, sizeof(buffer), "\tmov rax, %s", scratch_name(sregs, e->left->reg));
		        asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
		    }
		    
		    // Perform multiplication
		    snprintf(buffer, sizeof(buffer), "\tmul %s", scratch_name(sregs, e->right->reg));
		    asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
		    
		    // Move result to a new register if needed
		    e->reg = scratch_alloc(sregs);
		    if (strcmp(scratch_name(sregs, e->reg), "rax") != 0) {
		        snprintf(buffer, sizeof(buffer), "\tmov %s, rax", scratch_name(sregs, e->reg));
		        asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
		    }
		    
		    // Restore original rax value if we saved it
		    if (save_reg != -1) {
		        snprintf(buffer, sizeof(buffer), "\tmov rax, %s", scratch_name(sregs, save_reg));
		        asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
		        scratch_free(sregs, save_reg);
		    }
		    
		    scratch_free(sregs, e->left->reg);
		    scratch_free(sregs, e->right->reg);
		    break;
		
	 	case EXPR_SUB:
		case EXPR_ADD:
			expr_codegen(sregs, writer, e->left);
			expr_codegen(sregs, writer, e->right);
			// printf("Left child name: %s\n", e->left->name);
			// printf("Right child name: %s\n", e->right->name);

			snprintf(buffer, sizeof(buffer), "\t%s %s, %s", 
				(e->kind == EXPR_ADD) ? "add" : "sub",
				scratch_name(sregs, e->left->reg), 
				scratch_name(sregs, e->right->reg));

			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
		
			e->reg = e->left->reg;
			scratch_free(sregs, e->right->reg);

			if (e->symbol) {
				snprintf(buffer, sizeof(buffer), "\tmov [%s], %s",
					symbol_codegen(e->symbol),
					scratch_name(sregs, e->reg));

				asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
			}

			break;

		case EXPR_ASSIGNMENT:
		    expr_codegen(sregs, writer, e->right);
		    snprintf(buffer, sizeof(buffer), "\tmov [%s], %s",
		        symbol_codegen(e->left->symbol),
		        scratch_name(sregs, e->right->reg));

		    asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
		    e->reg = e->right->reg;
		    // scratch_free(sregs, e->right->reg);
		    break;

		case EXPR_NAME:
			printf("In EXPR_NAME with %s\n", e->symbol->name);
			e->reg = scratch_alloc(sregs);
			snprintf(buffer, sizeof(buffer), "\tmov %s, [%s]",
				scratch_name(sregs, e->reg),
				symbol_codegen(e->symbol));

			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
			break;

		case EXPR_INTEGER:
			e->reg = scratch_alloc(sregs);
			snprintf(buffer, sizeof(buffer), "\tmov %s, %d",
				scratch_name(sregs, e->reg),
				e->integer_value);

			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

			// if (e->symbol && e->symbol->kind == SYMBOL_LOCAL) {
			// 	snprintf(buffer, sizeof(buffer), "\tmov [%s], %s",
			// 		symbol_codegen(e->symbol),
			// 		scratch_name(sregs, e->reg));

			// 	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
			// }
			break;

		case EXPR_ARRAY_VAL:
			break;

		case EXPR_ARRAY:
			int label_num = label_create();
			const char* array_label = strdup(e->name);

			int array_size = 0;
			if (e->left) {
				expr_codegen(sregs, writer, e->left);
				array_size = e->left->integer_value;
			} else {
				fprintf(stderr, "Error: Array size not specified\n");
			}
			

			// printf("e->right is: %p\n", (void*)e->right);

			byte_size_t byte_t = get_byte_size(e->left->kind);
			request_byte_t request_t = get_request_type(byte_t);

			if (!e->right) {
				// printf("No initialization values\n");
				snprintf(buffer, sizeof(buffer), "\t%s: %s %d",
					array_label,
					request_to_string(byte_t), 
					array_size);

				asm_to_write_section(writer, buffer, DATA_DIRECTIVE);

			} else {
				char temp_buffer[1024] = {0};
				snprintf(temp_buffer, sizeof(temp_buffer), "\t%s %s",
					array_label,
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
    
    char buffer[56];
    switch (s->kind) {
        case STMT_DECL:
            decl_codegen(sregs, writer, s->decl, true);  // Pass true for local declarations
            break;

        case STMT_EXPR:
        	expr_codegen(sregs, writer, s->expr);
        	break;

        // case STMT_RETURN:
		// 	expr_codegen(sregs, writer, s->expr);
		// 	snprintf(buffer, sizeof(buffer), "\tmov rax, %s", scratch_name(sregs, s->expr->reg));
		// 	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
		// 	scratch_free(sregs, s->expr->reg);

		// 	snprintf(buffer, sizeof(buffer), "\n\tleave\n\tret");
		// 	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

        // 	break;
    }
    
    stmt_codegen(sregs, writer, s->next);
}

void decl_codegen(struct RegisterTable* sregs, struct AsmWriter* writer, struct decl* d, bool is_local) {
    if (!sregs || !d) return;
    char buffer[256];
    static bool have_section_data = false;
    static bool have_section_text = false;
    static bool have_declared_start = false;
    
    // Only handle global section setup when not processing local declarations
    if (!is_local) {
        // Handle .data section
        if (!have_section_data) {
            asm_to_write_section(writer, "section .data", DATA_DIRECTIVE);
            have_section_data = true;
        }
        
        // Write all global variables
        struct decl* globals = d;
        while (globals) {
            if (globals->type->kind != TYPE_FUNCTION) {  // Only process non-function declarations
                if (globals->type->kind == TYPE_ARRAY) {
                    if (globals->value && globals->value->kind == EXPR_ARRAY) {
                        expr_codegen(sregs, writer, globals->value);
                        asm_to_write_section(writer, "", DATA_DIRECTIVE);
                    }
                } else if (globals->type->kind == TYPE_INTEGER) {
                    if (globals->value) {
                        snprintf(buffer, sizeof(buffer), "\t%s dq %d", globals->name, globals->value->integer_value);
                    } else {
                        snprintf(buffer, sizeof(buffer), "\t%s dq 0", globals->name);
                    }
                    asm_to_write_section(writer, buffer, DATA_DIRECTIVE);
                }
            }
            globals = globals->next;
        }

        // Handle .text section and function declarations
        if (!have_section_text) {
            asm_to_write_section(writer, "\nsection .text", TEXT_DIRECTIVE);
            have_section_text = true;
        }
        
        // Write all function declarations
        struct decl* funcs = d;
        while (funcs) {
            if (funcs->type->kind == TYPE_FUNCTION) {
                snprintf(buffer, sizeof(buffer), "global %s", funcs->name);
                asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
            }
            funcs = funcs->next;
        }

        // Write _start
        if (!have_declared_start) {
            snprintf(buffer, sizeof(buffer), "global _start\n\n_start:\n");
            asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
            writer->start_section = ftell(writer->file);
            have_declared_start = true;
        }
        
        // Generate function bodies
        struct decl* func_bodies = d;
        while (func_bodies) {
            if (func_bodies->type->kind == TYPE_FUNCTION) {
            	if (strcmp(func_bodies->name, "main") == 0) {
            		snprintf(buffer, sizeof(buffer), "\tpush rbp\n\tmov rbp, rsp\n\tsub rsp, %ld\n", 
	                        func_bodies->symbol->s.total_local_bytes);
            		asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
            	} else {
	                snprintf(buffer, sizeof(buffer), "\n%s:", func_bodies->name);
	                asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
	                snprintf(buffer, sizeof(buffer), "\tpush rbp\n\tmov rbp, rsp\n\tsub rsp, %ld\n", 
	                        func_bodies->symbol->s.total_local_bytes);
	                asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
	                
	                if (func_bodies->code) {
	                    stmt_codegen(sregs, writer, func_bodies->code);
	                }
            	}
            }
            func_bodies = func_bodies->next;
        }
    } else {
        // Handle local variable declarations
        // Generate code for local variable initialization
        if (d->type->kind == TYPE_INTEGER) {
            if (d->value) {
            	expr_codegen(sregs, writer, d->value);

            	snprintf(buffer, sizeof(buffer), "\tmov [%s], %s",
            		symbol_codegen(d->symbol),
            		scratch_name(sregs, d->value->reg));
            	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
            	scratch_free(sregs, d->value->reg);

            }
        }
    }
}

void codegen_dag_node(struct RegisterTable* sregs, struct AsmWriter* writer, struct dag_node* node) {
	if (!node || node->freed || node->reg != - 1) return;

	node->reg = -1;
	if (node->left) codegen_dag_node(sregs, writer, node->left);
	if (node->right) codegen_dag_node(sregs, writer, node->right);

	char buffer[256];

	switch (node->kind) {
		case DAG_INTEGER_VALUE: {
			node->reg = scratch_alloc(sregs);

			if (node->reg == -1) return;

			snprintf(buffer, sizeof(buffer), "\tmov %s, %d",
				scratch_name(sregs, node->reg), node->u.integer_value);
			
			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
			break;
		} 

		case DAG_NAME: {
			if (!node->symbol) {
				fprintf(stderr, "Error: No symbol for DAG_NAME node\n");
				return;
			}
			node->reg = scratch_alloc(sregs);
			if (node->reg == -1) return;
			snprintf(buffer, sizeof(buffer), "\tmov %s, [%s]",
				scratch_name(sregs, node->reg), symbol_codegen(node->symbol));
			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
			break;
		}

		case DAG_ASSIGN: {
			if (!node->left || !node->right || node->left->kind != DAG_NAME || !node->left->symbol) {
				fprintf(stderr, "Error: Invalid assignment node\n");
				return;
			}

			if (node->right->reg == -1) {
				fprintf(stderr, "Error: Right operand not evaluated\n");
				return;
			}
			snprintf(buffer, sizeof(buffer), "\tmov [%s], %s",
				symbol_codegen(node->left->symbol), scratch_name(sregs, node->right->reg));
			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
			node->reg = node->right->reg;
			break;
		}

		case DAG_ADD:
        case DAG_SUB:
        case DAG_MUL: {
            if (!node->left || !node->right || node->left->reg == -1 || node->right->reg == -1) {
                fprintf(stderr, "Error: Invalid arithmetic node\n");
                return;
            }
            if (node->kind == DAG_MUL) {
                int save_reg = -1;
                if (strcmp(scratch_name(sregs, node->left->reg), "rax") != 0) {
                    save_reg = scratch_alloc(sregs);
                    if (save_reg == -1) {
                        fprintf(stderr, "Error: No free registers for MUL\n");
                        return;
                    }
                    snprintf(buffer, sizeof(buffer), "\tmov %s, rax",
                             scratch_name(sregs, save_reg));
                    asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
                }

                if (strcmp(scratch_name(sregs, node->left->reg), "rax") != 0) {
                    snprintf(buffer, sizeof(buffer), "\tmov rax, %s",
                             scratch_name(sregs, node->left->reg));
                    asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
                }

                snprintf(buffer, sizeof(buffer), "\tmul %s",
                         scratch_name(sregs, node->right->reg));
                asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

                node->reg = scratch_alloc(sregs);
                if (node->reg == -1) {
                    fprintf(stderr, "Error: No free registers for MUL result\n");
                    return;
                }
                if (strcmp(scratch_name(sregs, node->reg), "rax") != 0) {
                    snprintf(buffer, sizeof(buffer), "\tmov %s, rax",
                             scratch_name(sregs, node->reg));
                    asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
                }

                if (save_reg != -1) {
                    snprintf(buffer, sizeof(buffer), "\tmov rax, %s",
                             scratch_name(sregs, save_reg));
                    asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
                    scratch_free(sregs, save_reg);
                }
            } else {
             
                snprintf(buffer, sizeof(buffer), "\t%s %s, %s",
                         (node->kind == DAG_ADD) ? "add" : "sub",
                         scratch_name(sregs, node->left->reg),
                         scratch_name(sregs, node->right->reg));
                asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
                node->reg = node->left->reg;
                scratch_free(sregs, node->right->reg);
            }
            scratch_free(sregs, node->left->reg);
            break;
        }

		default:
			break;

	}
}

void codegen_dag(struct RegisterTable* sregs, struct AsmWriter* writer, struct DAG* dag) {
	if (!sregs || !writer || !dag) return;

	char buffer[256];

	for (int i = 0; i < dag->node_count; i++) {
		struct dag_node* node = dag->nodes[i];
		if (!node || node->freed) continue;
		codegen_dag_node(sregs, writer, node);
	}
}

void codegen_block(struct RegisterTable* sregs, struct AsmWriter* writer, struct basic_block* block) {
    if (!sregs || !writer || !block || block->emitted_x86_assembly) return;

    char buffer[256];

    // Generate a unique label for the block
    int label = label_create();
    snprintf(buffer, sizeof(buffer), "%s:", label_name(label));
    asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

    // Generate code for the block's DAG
    if (block->dag) {
        codegen_dag(sregs, writer, block->dag);
    }

    // Handle control flow to successors
    if (block->successor_count > 0) {
        if (block->successor_count == 1) {
            snprintf(buffer, sizeof(buffer), "\tjmp %s",
                     label_name(block->successors[0]->label));
            asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
        } else if (block->successor_count == 2) {
            if (block->dag && block->dag->node_count > 0) {
                struct dag_node* cond_node = block->dag->nodes[0];
                if (cond_node && cond_node->reg != -1) {
                    snprintf(buffer, sizeof(buffer), "\tcmp %s, 0",
                             scratch_name(sregs, cond_node->reg));
                    asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

                    const char* jump_inst = NULL;
                    switch (cond_node->kind) {
                        case DAG_LESS: jump_inst = "jl"; break;
                        case DAG_GREATER: jump_inst = "jg"; break;
                        case DAG_LESS_EQUAL: jump_inst = "jle"; break;
                        case DAG_GREATER_EQUAL: jump_inst = "jge"; break;
                        case DAG_EQUAL: jump_inst = "je"; break;
                        case DAG_NOT_EQUAL: jump_inst = "jne"; break;
                        default: jump_inst = "jnz"; break;
                    }
                    snprintf(buffer, sizeof(buffer), "\t%s %s",
                             jump_inst, label_name(block->successors[0]->label));
                    asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

                    snprintf(buffer, sizeof(buffer), "\tjmp %s",
                             label_name(block->successors[1]->label));
                    asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

                    scratch_free(sregs, cond_node->reg);
                }
            }
        }
    }

    block->emitted_x86_assembly = true;
    block->label = label;
}

void codegen_CFG(struct RegisterTable* sregs, struct AsmWriter* writer, struct CFG* cfg) {
	if (cfg->blocks) {
		for (int i = 0; i < cfg->block_count; i++) {
			if (cfg->blocks[i]->emitted_x86_assembly) continue;

			cfg->blocks[i]->emitted_x86_assembly = true;
			codegen_block(sregs, writer, cfg->blocks[i]);
		}
	}
}

void process_CFG(struct RegisterTable* sregs, struct AsmWriter* writer, struct CFG* cfg) {
	if (!sregs || !writer || !cfg) return;

	while (cfg) {
		struct cfg* next = cfg->next;
		codegen_CFG(sregs, writer, cfg);
		cfg = next;
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