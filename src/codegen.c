#include "codegen.h"

#define MAX_QUEUE_SIZE 100

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
			size_t offset;	
			if (sym->type->kind == TYPE_INTEGER) {
				snprintf(buffer, sizeof(buffer), "rbp - %zu", sym->s.byte_offset);
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

size_t compute_offset(struct symbol* symbol, int* index) {
	if (!symbol) return 1;

	size_t offset;
	size_t element_size;

	printf("Computing offset for symbol: %s, kind: %d, byte_offset: %zu\n",
		symbol->name, symbol->kind, symbol->s.byte_offset);

	switch (symbol->kind) {
		case SYMBOL_LOCAL: {
			if (symbol->type->kind == TYPE_ARRAY) {
				if (symbol->type->subtype && symbol->type->subtype->kind == TYPE_INTEGER) {
					element_size = sizeof(int);

					offset = symbol->s.byte_offset;
					
					if (index && *index >= 0) {
						offset += (*index) * element_size;
					} 
					printf("In TYPE ARRAY case with subtype TYPE INTEGER with offset: %d\n", offset);
					return offset;
				} else if (symbol->type->subtype && symbol->type->subtype->kind == TYPE_CHARACTER || 
					symbol->type->subtype->kind == TYPE_BOOLEAN) {
						
						element_size = sizeof(char);
						offset = symbol->s.byte_offset;
						
						if (index && *index >= 0) {
							offset += (*index) * element_size;
						} 
					
					return offset;
				}
			} else if (symbol->type->kind == TYPE_INTEGER) {
				printf("Returning byte offset for %s: %zu\n", symbol->name, symbol->s.byte_offset);
				return symbol->s.byte_offset;
			}
		}
		default: {
			fprintf(stderr, "Error: Unkown symbol kind in computing offset\n");
			return 1;
		}

	}
	return 1;
}

void expr_codegen(struct RegisterTable* sregs, struct AsmWriter* writer, struct expr* e) {
	if (!sregs || !e) return;

	char buffer[256];

	switch (e->kind) {
	 	// case EXPR_DIV: {
	 	// 	expr_codegen(sregs, writer, e->left);
	 	// 	expr_codegen(sregs, writer, e->right);

	 	// 	snprintf(buffer, sizeof(buffer), "\tidiv %s",
	 	// 		scratch_name(sregs, e->right->reg));
	 	// 	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
	 	
	 	// 	break;
	 	// }

		case EXPR_MUL: {
		    expr_codegen(sregs, writer, e->left);
		    expr_codegen(sregs, writer, e->right);
		    
		    snprintf(buffer, sizeof(buffer), "\timul %s, %s",
		     scratch_name(sregs, e->left->reg),
		     scratch_name(sregs, e->right->reg));
		    asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
		    
		    e->reg = scratch_alloc(sregs);
		    // if (strcmp(scratch_name(sregs, e->reg), "rax") != 0) {
		    // 	snprintf(buffer, sizeof(buffer), "\tmov, %s, %s",
		    // 		scratch_name(sregs, e->reg),
		    // 		scratch_name(sregs, e->left->reg));
		    // 	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
		    // }

		    snprintf(buffer, sizeof(buffer), "\tmov %s, %s",
		    	scratch_name(sregs, e->reg),
		    	scratch_name(sregs, e->left->reg));
		   	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

		    scratch_free(sregs, e->left->reg);
		    scratch_free(sregs, e->right->reg);
		    break;
		}
		
	 	case EXPR_SUB:
		case EXPR_ADD: {
			expr_codegen(sregs, writer, e->left);
			expr_codegen(sregs, writer, e->right);

			snprintf(buffer, sizeof(buffer), "\t%s %s, %s", 
				(e->kind == EXPR_ADD) ? "add" : "sub",
				scratch_name(sregs, e->left->reg), 
				scratch_name(sregs, e->right->reg));

			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
		
			e->reg = e->left->reg;
			scratch_free(sregs, e->right->reg);

			break;

		}

		case EXPR_EQUAL:
		case EXPR_NOT_EQUAL: 
		case EXPR_GREATER_EQUAL:
		case EXPR_LESS_EQUAL:
		case EXPR_GREATER:
		case EXPR_LESS: {
			expr_codegen(sregs, writer, e->left);
			expr_codegen(sregs, writer, e->right);

			snprintf(buffer, sizeof(buffer), "\tcmp %s, %s",
				scratch_name(sregs, e->left->reg),
				scratch_name(sregs, e->right->reg));
			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

			scratch_free(sregs, e->left->reg);
			scratch_free(sregs, e->right->reg);
			break;
		}


		case EXPR_DECREMENT: {
			size_t offset = compute_offset(e->left->symbol, NULL);
			snprintf(buffer, sizeof(buffer), "\tdec [rbp - %zu]",
				offset);
			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

			break;	

		}
		
		case EXPR_INCREMENT: {
			size_t offset = compute_offset(e->left->symbol, NULL);

			snprintf(buffer, sizeof(buffer), "\tinc [rbp - %zu]",
				offset);
			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

			break;

		} 
		
		case EXPR_ASSIGNMENT:
			printf("In EXPR_ASSIGNMENT\n");
		    expr_codegen(sregs, writer, e->right);
		  	expr_codegen(sregs, writer, e->left);
		
		    	

		    // 	if (index >= 0) {
		    // 		size_t offset = compute_offset(e->left->left->symbol, &index);
		    // 		if (offset != 1) {
		    // 			snprintf(buffer, sizeof(buffer), "\tmov [rbp - %zu], %s",
		    // 				offset,
		    // 				scratch_name(sregs, e->right->reg));
		    // 			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
		    // 		} 
		    // 	} else {
            //        int temp_reg = scratch_alloc();

            //        	snprintf(buffer, sizeof(buffer), "\tmov %s, %s",
            //        		scratch_name(sregs, temp_reg),
            //        		scratch_name(sregs, e->right->reg));
            //       	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

            //       	snprintf(buffer, sizeof(buffer))

                
                    
                 
            //     }
		    // 	size_t offset = compute_offset(e->left->left->symbol, &e->right->integer_value);
		    // 	snprintf(buffer, sizeof(buffer), "\tmov [rbp - %zu], %s",
		    // 		offset,
		    // 		scratch_name(sregs, e->right->reg));
		    // 	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

		    // } else {
			//     snprintf(buffer, sizeof(buffer), "\tmov [%s], %s",
			//         symbol_codegen(e->left->symbol),
			//         scratch_name(sregs, e->right->reg));

			//     asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
			//     e->reg = e->right->reg;
			//     printf("Leaving EXPR_ASSIGNMENT\n");

		    // }

		    break;


		case EXPR_NAME: {
			printf("In EXPR_NAME with %s\n", e->symbol->name);
			
			e->reg = scratch_alloc(sregs);
			size_t offset = compute_offset(e->symbol, NULL);
			snprintf(buffer, sizeof(buffer), "\tmov %s, [rbp - %zu]",
				scratch_name(sregs, e->reg),
				offset);
			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
				
			break;			
		}

		case EXPR_CALL: {
			static const char* regs[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
			if (!e->left || !e->right) {
				fprintf(stderr, "Error: EXPR_CALL does not have valid children\n");
				return;
			}

			struct expr* current_arg = e->right;
			while (current_arg) {
				struct expr* next = current_arg->right;
				expr_codegen(sregs, writer, current_arg);
				current_arg = next;
			}

			int count = 0;
			
			current_arg = e->right;		
			while (current_arg && count < 6) {
				struct expr* next = current_arg->right;
				char* curr_reg = regs[count]; 

				switch (current_arg->kind) {
					case EXPR_NAME:
					case EXPR_INTEGER: {
						snprintf(buffer, sizeof(buffer), "\tmov %s, %s",
							curr_reg,
							scratch_name(sregs, current_arg->reg)
						);

						break;
					}

					
				} 

				asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

				count++;
				current_arg = next;	
			}

			while (current_arg) {
				struct expr* next = current_arg->right;
				snprintf(buffer, sizeof(buffer), "")

				current_arg = next;
			}

			// if there are more than 6 arguments to function

			// while (current_arg) {

			// }

			snprintf(buffer, sizeof(buffer), "\tcall %s", e->left->name);
			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
		}

		case EXPR_INTEGER: {
			e->reg = scratch_alloc(sregs);
			snprintf(buffer, sizeof(buffer), "\tmov %s, %d",
				scratch_name(sregs, e->reg),
				e->integer_value);

			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

			break;			
		}

		case EXPR_SUBSCRIPT: {
			if (!e->left || !e->right) {
				fprintf(stderr, "Error: Invalid EXPR_SUBSCRIPT node\n");
				break;
			}

			if (e->right->kind == EXPR_NAME && !e->right->symbol && e->right->name) {
				fprintf(stderr, "Warning: Index symbol is missing for variable '%s'\n", e->right->name);
			}

			expr_codegen(sregs, writer, e->right);
			if (e->right->reg == -1) {
				fprintf(stderr, "Error: No register allocated for index in EXPR_SUBSCRIPT\n");
				break;
			}

			e->reg = scratch_alloc(sregs);
			if (e->reg == -1) {
				fprintf(stderr, "Error: No free register for EXPR_SUBSCRIPT\n");
				break;
			}

			size_t element_size;
			if (e->left->symbol->type->subtype) {
				element_size = sizeof(int);
			} else if (e->left->symbol->type->subtype == TYPE_CHARACTER ||
				e->left->symbol->type->subtype == TYPE_BOOLEAN) {
				element_size = sizeof(char);
			} else {
				fprintf(stderr, "Error: Unknown element type for array\n");
				element_size = sizeof(int);
			}

			// snprintf(buffer, sizeof(buffer), "\tmov %s, %s",
			// 	scratch_name(sregs, e->reg),
			// 	scratch_name(sregs, e->right->reg));
			// asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

			if (element_size > 1) {
				snprintf(buffer, sizeof(buffer), "\timul %s, %zu",
					scratch_name(sregs, e->right->reg),
					element_size
				);
				asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
			}

			int temp_reg = scratch_alloc(sregs);
			if (temp_reg == -1) {
				fprintf(stderr, "Error: No free register for temp in EXPR_SUBSCRIPT\n");
				break;
			}

			size_t offset = compute_offset(e->left->symbol, &e->right->integer_value);
			snprintf(buffer, sizeof(buffer), "\tlea %s, [rbp - %zu]",
				scratch_name(sregs, temp_reg),
				offset);
			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

			snprintf(buffer, sizeof(buffer), "\tadd %s, %s",
				scratch_name(sregs, temp_reg),
				scratch_name(sregs, e->right->reg));
			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

			snprintf(buffer, sizeof(buffer), "\tmov %s, [%s]",
				scratch_name(sregs, e->reg),
				scratch_name(sregs, temp_reg));
			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);


			scratch_free(sregs, temp_reg);
			scratch_free(sregs, e->right->reg);
			scratch_free(sregs, e->left->reg);

			break;
		}


		case EXPR_ARRAY_VAL:
			break;

		case EXPR_ARRAY:
			printf("ARRRRRRRRRRRRRRRRRRRAY\n");
			int label_num = label_create();
			const char* array_label = strdup(e->name);

			int array_size;
			if (e->left) {
				expr_codegen(sregs, writer, e->left);
				array_size = e->left->integer_value;
			} else {
				fprintf(stderr, "Error: Array size not specified\n");
			}

			byte_size_t byte_t = get_byte_size(e->left->kind);
			char* byte_kind = bytes_to_string(byte_t);
			request_byte_t request_t = get_request_type(byte_t);
			
			if (e->symbol && e->symbol->kind == SYMBOL_GLOBAL) {
				if (e->right) {
					int offset = snprintf(buffer, sizeof(buffer), "\t%s: \t%s",
						array_label,
						byte_kind
					);

					struct expr* current = e->right;
					int count = 0;
					if (offset >= 0) {
						while (current && offset < sizeof(buffer)) {
							int augmented_offset; 
							if (current->right) {
								augmented_offset = snprintf(buffer + offset, sizeof(buffer), " %d,", current->integer_value);
							} else {
								augmented_offset = snprintf(buffer + offset, sizeof(buffer), " %d", current->integer_value);
							}

							offset += augmented_offset;
							current = current->right;
							count++;
						}

						int diff = array_size - count;
						while (diff > 0 && offset < sizeof(buffer)) {
								int augmented_offset = snprintf(buffer + offset, sizeof(buffer), ", %d", 0);
								offset += augmented_offset;
								diff--;
						}							
						
						asm_to_write_section(writer, buffer, DATA_DIRECTIVE);

					}
				}
			} else if (e->symbol && e->symbol->kind == SYMBOL_LOCAL) {
				printf("HELLLOOOOOO\n");
				if (e->right) {
					struct expr* current = e->right;
					int index = 0;

					snprintf(buffer, sizeof(buffer), "");
					asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
					printf("I am here\n");
					while (current) {
						size_t offset = compute_offset(e->symbol, &index);
						snprintf(buffer, sizeof(buffer), "\tmov dword [rbp - %zu], %d",
							offset,
							current->integer_value
						);

						asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
						current = current->right;
						index++;
					}

					int diff = array_size - index;
					while (diff > 0) {						
						size_t offset = compute_offset(e->symbol, &index);
						snprintf(buffer, sizeof(buffer), "\tmov dword [rbp - %zu], %d",
							offset,
							0
						);
						asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
						index++;
						diff--;
					}

					snprintf(buffer, sizeof(buffer), "");
					asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

					printf("Down here\n");
				}
			}
			printf("Towards the end\n");
			if (e->left->reg != -1) {
				scratch_free(sregs, e->left->reg); 
			}
			printf("I am here\n");
			free((void*)array_label);
			break;
	}
}

struct LabelStack* create_label_stack() {
	struct LabelStack* label_stack = malloc(sizeof(struct LabelStack));
	if (!label_stack) {
		fprintf(stderr, "Error: Unable to allocate space for label stack\n");
		return NULL;
	}

	for (int i = 0; i < MAX_LABELS; i++) {
		label_stack->labels[i] = -1;
	}
	label_stack->top = -1;
	return label_stack;
}

void push_label(struct LabelStack* label_stack, int label) {
	if (!label_stack) return;

	if (label_stack->top < MAX_LABELS - 1) {
		label_stack->labels[++label_stack->top] = label;
	} else {
		fprintf(stderr, "Error: Label stack overflow\n");
	}
}

int pop_label(struct LabelStack* label_stack) {
	if (label_stack->top >= 0) {
		return label_stack->labels[label_stack->top--];
	}

	return -1;

}

int peek_label(struct LabelStack* label_stack, int offset) {
	if (label_stack->top - offset >= 0) {
		return label_stack->labels[label_stack->top - offset];
	}
	return -1;
}

struct CodegenContext create_codegen_context(int loop_condition,  bool in_loop) {
	struct CodegenContext context = {
		.loop_condition = loop_condition,
		.in_loop = in_loop
	};

	return context;
}


void stmt_codegen(struct RegisterTable* sregs, struct AsmWriter* writer, struct stmt* s, struct CodegenContext* context) {
    if (!sregs || !s) return;

    
    char buffer[256];
    switch (s->kind) {
        case STMT_DECL: {
            decl_codegen(sregs, writer, s->decl, true); 
            break;
        }

        case STMT_EXPR: {
        	expr_codegen(sregs, writer, s->expr);
        	if (s->expr->reg != -1) {
        		scratch_free(sregs, s->expr->reg);
        	}
        	break;       

        }

        // case STMT_BREAK: {
        // 	if (context->loop_condition != -1 && context->in_loop) {
        // 		if (context->loop_end) {
        // 			snprintf(buffer, sizeof(buffer), "\tjmp %s", label_name(context->loop_end));
        // 			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
        // 		}
        // 	} else {
        // 		fprintf(stderr, "Warning: You have 'break' statement outside of a loop\n");
        // 	}

        // 	break;

        // }

        case STMT_CONTINUE: {
        	if (context->loop_condition != -1 && context->in_loop) {
        		snprintf(buffer, sizeof(buffer), "\tjmp %s", label_name(context->loop_condition));
        		asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
        	} else {
        		fprintf(stderr, "Warning: You have 'continue' statement outside of a loop\n");
        	}

        	break;
        }	

        case STMT_IF: {
        	int condition_true_label = label_create();
        	int end_label = label_create();

        	// push_label(stack, end_label);
        	// push_label(stack, condition_true_label);

        	if (s->expr) {
        		expr_codegen(sregs, writer, s->expr);
        		
        		switch (s->expr->kind) {
        			case EXPR_LESS_EQUAL:
        			case EXPR_LESS: {
        				snprintf(buffer, sizeof(buffer), "\tjg %s", label_name(end_label));
						break;        		
        			}

        			case EXPR_GREATER_EQUAL:
        			case EXPR_GREATER: {
        				snprintf(buffer, sizeof(buffer), "\tjle %s", label_name(end_label));
        				break;
        			}

        			case EXPR_NOT_EQUAL: {
        				snprintf(buffer, sizeof(buffer), "\tje %s", label_name(end_label));
        				break;
        			}

        			case EXPR_EQUAL: {
        				snprintf(buffer, sizeof(buffer), "\tjne %s", label_name(end_label));
        				break;
        			}
  
        		}

        		asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
        		scratch_free(sregs, s->expr->reg);
        	}

        	snprintf(buffer, sizeof(buffer), "%s:", label_name(condition_true_label));
        	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

        	if (s->body) {
        		stmt_codegen(sregs, writer, s->body, context);
        	}

        	if (context->loop_condition != -1 && context->in_loop) {
        		snprintf(buffer, sizeof(buffer), "\tjmp %s", label_name(context->loop_condition));
        		asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
        	}

        	snprintf(buffer, sizeof(buffer), "%s:", label_name(end_label));
        	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
        	
        	if (s->else_body) {
        		stmt_codegen(sregs, writer, s->else_body, context);
        	}
        		
        	// pop_label(stack);
        	// pop_label(stack);
        	break;
        }

    	case STMT_ELSE_IF: {
    		int condition_true_label = label_create();
    		int end_label = label_create();

    		// push_label(stack, end_label);
    		// push_label(stack, condition_true_label);

    		if (s->expr) {
    			expr_codegen(sregs, writer, s->expr);

    			switch (s->expr->kind) {
        			case EXPR_LESS_EQUAL:
        			case EXPR_LESS: {
        				snprintf(buffer, sizeof(buffer), "\tjg %s", label_name(end_label));
						break;        		
        			}

        			case EXPR_GREATER_EQUAL:
        			case EXPR_GREATER: {
        				snprintf(buffer, sizeof(buffer), "\tjle %s", label_name(end_label));
        				break;
        			}

        			case EXPR_NOT_EQUAL: {
        				snprintf(buffer, sizeof(buffer), "\tje %s", label_name(end_label));
        				break;
        			}

        			case EXPR_EQUAL: {
        				snprintf(buffer, sizeof(buffer), "\tjne %s", label_name(end_label));
        				break;
        			}
  
        		}
    			
    			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
    			scratch_free(sregs, s->expr->reg);
    		}

    		snprintf(buffer, sizeof(buffer), "%s:", label_name(condition_true_label));
    		asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

    		if (s->body) {
    			stmt_codegen(sregs, writer, s->body, context);
    		}

    		if (context->loop_condition != -1 && context->in_loop) {
    			printf("Label Name: %d\n", label_name(context->loop_condition));
        		snprintf(buffer, sizeof(buffer), "\tjmp %s", label_name(context->loop_condition));
        		asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
        	}

    		// if (s->else_body) {
    		// 	snprintf(buffer, sizeof(buffer), "\tjmp %s", label_name(end_label));
    		// 	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
    		// }

    		snprintf(buffer, sizeof(buffer), "%s:", label_name(end_label));
    		asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

    		if (s->else_body) {
    			stmt_codegen(sregs, writer, s->else_body, context);
    		}

    		// pop_label(stack);
    		// pop_label(stack);
    		break;
    	}

    	case STMT_ELSE: {

    		if (s->body) {
    			stmt_codegen(sregs, writer, s->body, context);
    		}

    		// if (context->loop_condition != -1 && context->in_loop) {
        	// 	snprintf(buffer, sizeof(buffer), "\tjmp %s", label_name(context->loop_condition));
        	// 	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
        	// }

    		break;
    	}
        case STMT_FOR: {
        	int loop_start = label_create();
        	int loop_condition = label_create();
        	int loop_end = label_create();

        	printf("For loop label start: %d\n", loop_start);
        	printf("For loop label end: %d\n", loop_end);
        	
        	// push_label(stack, loop_end);
        	// push_label(stack, loop_start);

        	context->loop_condition = loop_condition;
        	// context->loop_end = loop_end;
        	context->in_loop = true;

        	if (s->decl) {
    	    	size_t offset = compute_offset(s->decl->value->symbol, NULL);
        		if (s->decl->value && s->decl->value->kind == EXPR_INTEGER) {
	        		snprintf(buffer, sizeof(buffer), "\tmov [rbp - %zu], %d",
	        			offset,
	        			s->decl->value->integer_value);
	        		asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

        		} else if (s->decl->value && s->decl->value->kind == EXPR_NAME) {
        			expr_codegen(sregs, writer, s->decl->value);

        			snprintf(buffer, sizeof(buffer), "\tmov [rbp - %zu], %s",
        				offset,
        				scratch_name(sregs, s->decl->value->reg));
        			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
        		}
        	} 

        	snprintf(buffer, sizeof(buffer), "%s: ", label_name(loop_start));
        	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

        	if (s->expr) {
        		if (s->expr->left->symbol != s->decl->value->symbol) {
        			s->expr->left->symbol = s->decl->value->symbol;
        		}
        		      
        		expr_codegen(sregs, writer, s->expr);

        		switch (s->expr->kind) {
        			case EXPR_LESS_EQUAL:
        			case EXPR_LESS: {
        				snprintf(buffer, sizeof(buffer), "\tjg %s", label_name(loop_end));
        				break;
        			}

        			case EXPR_GREATER_EQUAL:
        			case EXPR_GREATER: {
        				snprintf(buffer, sizeof(buffer), "\tjle %s", label_name(loop_end));
        				break;
        			}

        			case EXPR_EQUAL: {
        				snprintf(buffer, sizeof(buffer), "\tjne %s", label_name(loop_end));
        				break;
        			}

        			case EXPR_NOT_EQUAL: {
        				snprintf(buffer, sizeof(buffer), "\tje %s", label_name(loop_end));
        				break;
        			}
        		}

        		asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
        		scratch_free(sregs, s->expr->reg);
        	}

        	if (s->body) {
        		stmt_codegen(sregs, writer, s->body, context);
        	}

        	if (s->next_expr) {
        		if (s->next_expr->left->symbol != s->decl->value->symbol) {
        			s->next_expr->left->symbol = s->decl->value->symbol;
        		}

        		snprintf(buffer, sizeof(buffer), "%s:", label_name(loop_condition));
        		asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

        		expr_codegen(sregs, writer, s->next_expr);
        		if (s->next_expr->reg != -1) {
        			scratch_free(sregs, s->next_expr->reg);
        		}
        	}

        	snprintf(buffer, sizeof(buffer), "\tjmp %s", label_name(loop_start));
        	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

        	snprintf(buffer, sizeof(buffer), "%s:", label_name(loop_end));
        	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
        	
        	// pop_label(stack);
        	// pop_label(stack);

        	context->loop_condition = -1;
        	// context->loop_end = -1;
        	context->in_loop = false;

        	break;
        }

    	case STMT_WHILE: {
    		int loop_start = label_create();
    		int loop_end = label_create();

    		snprintf(buffer, sizeof(buffer), "%s:", label_name(loop_start));
    		asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

    		if (s->expr) {
    			expr_codegen(sregs, writer, s->expr);

    			snprintf(buffer, sizeof(buffer), "\tcmp %s, 0",
    				scratch_name(sregs, s->expr->reg));
    			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

    			snprintf(buffer, sizeof(buffer), "\tje %s",
    				label_name(loop_end));
    			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
    			
    			scratch_free(sregs, s->expr->reg);
    		}

    		if (s->body) {
    			stmt_codegen(sregs, writer, s->body, context);
    		}

    		snprintf(buffer, sizeof(buffer), "\tjmp %s",
    			label_name(loop_start));
    		asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

    		snprintf(buffer, sizeof(buffer), "%s:", label_name(loop_end));
    		asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
    	}
    	break;

        case STMT_RETURN:
			if (s->expr) {
				// expr_codegen(sregs, writer, s->expr);
				if (s->expr->symbol) {
					snprintf(buffer, sizeof(buffer), "\tmov rax, [%s]", 
						symbol_codegen(s->expr->symbol));
				} else {
					snprintf(buffer, sizeof(buffer), "\tmov rax, %d",
						s->expr->integer_value);
				}
				
				asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
				// scratch_free(sregs, s->expr->reg);
			}
			snprintf(buffer, sizeof(buffer), "\tleave\n\tret");
			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

        	break;
    }
    
    stmt_codegen(sregs, writer, s->next, context);
}

// struct FormatQueue* create_format_queue(int capacity) {
// 	struct FormatQueue* q = malloc(sizeof(struct FormatQueue));
// 	if (!q) {
// 		fprintf(stderr, "Error: Failed to allocate space for format queue\n");
// 		return NULL;
// 	}

// 	q->capacity = capacity;
// 	q->size = 0;
// 	q->front = -1;
// 	q->rear = -1;
// 	q->items = malloc(sizeof(char*) * capacity);
// 	if (!q->items) {
// 		fprintf(stderr, "Error: Failed to allocate space for format queue items\n");
// 		free(q);
// 		return NULL;
// 	}

// 	for (int i = 0; i < q->capacity; i++) {
// 		q->items[i] = NULL;
// 	}

// 	return q;
// }

// void enqueue_format(struct FormatQueue* q, const char* format) {
// 	if (!q || !format) return;

// 	if (q->size >= q->capacity) {
// 		printf("Format queue is full\n");
// 		return;
// 	} else {
// 		if (q->front == -1) {
// 			q->front = 0;
// 		}

// 		q->rear++;
// 		q->items[q->rear] = format;
// 		q->size++;
// 	}
// }

// char* dequeue_format(struct FormatQueue* q) {
// 	char* item;
// 	if (is_format_queue_empty(q)) {
// 		printf("Queue is empty\n");
// 		item = NULL;
// 	} else {
// 		item = q->items[q->front];
// 		q->front++;
// 		q->size--;
// 		if (q->front > q->rear) {
// 			q->front = q->rear = -1;
// 		}
// 	}

// 	return item;
// }

// int is_format_queue_empty(struct FormatQueue* q) {
// 	if (q->rear == -1) return 1;

// 	return 0;
// }

// struct ArgumentQueue* create_argument_queue(int capacity) {
// 	struct ArgumentQueue* q = malloc(sizeof(struct ArgumentQueue));
// 	if (!q) {
// 		fprintf(stderr, "Error: Failed to allocate space for argument space\n");
// 		return NULL;
// 	}

// 	q->capacity = capacity;
// 	q->size = 0;
// 	q->front = -1;
// 	q->rear = -1;
// 	q->items = malloc(sizeof(struct expr*) * capacity);
// 	if (!q->items) {
// 		fprintf(stderr, "Error: Failed to allocate space for argument queue items\n");
// 		free(q);
// 		return NULL;
// 	}

// 	return q;
// }

// void enqueue_arg(struct ArgumentQueue* q, struct expr* e) {
// 	if (!q || !e) return;

// 	if (q->size >= q->capacity) {
// 		printf("Argument queue is full, come back later\n");
// 		return;
// 	} else {
// 		if (q->front == -1) {
// 			q->front = 0;
// 		}

// 		q->rear++;
// 		q->items[q->rear] = e;
// 		q->size++;
// 	}
// }

// struct expr* dequeue_arg(struct ArgumentQueue* q) {
// 	struct expr* e;
// 	if (is_arg_queue_empty(q)) {
// 		printf("Arg queue is empty\n");
// 		e = NULL;
// 	} else {
// 		e = q->items[q->front];
// 		q->front++;
// 		q->size--;
// 		if (q->front > q->rear) {
// 			q->front = q->rear = -1;
// 		}
// 	}	
// 	return e;
// }

// int is_arg_queue_empty(struct ArgumentQueue* q) {
// 	if (q->rear == -1) return 1;

// 	return 0;
// }

// void free_format_queue(struct FormatQueue* queue) {
// 	if (!queue) return;

// 	for (int i = 0; i < queue->capacity; i++) {
// 		if (queue->items[i]) {
// 			free(queue->items[i]);
// 		}
// 	}
// 	free(queue->items);
// 	free(queue);
// }

// void free_argument_queue(struct ArgumentQueue* queue) {
// 	if (!queue) return;

// 	if (queue->items) free(queue->items);
// 	free(queue);
// }

void decl_codegen(struct RegisterTable* sregs, struct AsmWriter* writer, struct decl* d, bool is_local) {
    if (!sregs || !d) return;
    char buffer[256];
    static bool have_section_data = false;
    static bool have_section_text = false;
    static bool have_declared_start = false;

    struct LabelStack* stack = create_label_stack();
    struct CodegenContext context = create_codegen_context(-1, false);
    
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
            	snprintf(buffer, sizeof(buffer), "\n%s:", func_bodies->name);
            	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
            	snprintf(buffer, sizeof(buffer), "\tpush rbp\n\tmov rbp, rsp\n\tsub rsp, %ld\n",
            		func_bodies->symbol->s.total_local_bytes);
            	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

            	if (func_bodies->code) {
            		stmt_codegen(sregs, writer, func_bodies->code, &context);
            	}
            	// if (strcmp(func_bodies->name, "main") == 0) {
            	// 	snprintf(buffer, sizeof(buffer), "\n%s:", func_bodies->name);
            	// 	snprintf(buffer, sizeof(buffer), "\tpush rbp\n\tmov rbp, rsp\n\tsub rsp, %ld\n", 
	            //             func_bodies->symbol->s.total_local_bytes);
            	// 	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
            	// } else {
	            //     snprintf(buffer, sizeof(buffer), "\n%s:", func_bodies->name);
	            //     asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
	            //     snprintf(buffer, sizeof(buffer), "\tpush rbp\n\tmov rbp, rsp\n\tsub rsp, %ld\n", 
	            //             func_bodies->symbol->s.total_local_bytes);
	            //     asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
	               
	            //     if (func_bodies->code) {
	            //         stmt_codegen(sregs, writer, func_bodies->code);
	            //     }
            	// }
            }
            func_bodies = func_bodies->next;
        }
    } else if (d->type->kind == TYPE_INTEGER) {
        if (d->symbol->kind == SYMBOL_LOCAL) {
            if (d->value) {
            	if (d->value->symbol != d->symbol) {
            		d->value->symbol = d->symbol;
            	}
            	expr_codegen(sregs, writer, d->value);
            	snprintf(buffer, sizeof(buffer), "\tmov [%s], %s",
            		symbol_codegen(d->symbol),
            		scratch_name(sregs, d->value->reg));
            	asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
            	scratch_free(sregs, d->value->reg);

            }
        }
    } else if (d->type->kind == TYPE_ARRAY) {
    	if (d->symbol->kind == SYMBOL_GLOBAL) {
    		if (d->value) {
    			expr_codegen(sregs, writer, d->value);
    		} 
    	} else if(d->symbol->kind == SYMBOL_LOCAL) {
    		if (d->value) {
    			printf("Generating code for local array %s\n", d->name);
    			expr_codegen(sregs, writer, d->value);
    		}
    	}
    }
}

// void codegen_dag_node(struct RegisterTable* sregs, struct AsmWriter* writer, struct dag_node* node) {
// 	if (!node || node->freed || node->reg != - 1) return;

// 	node->reg = -1;
// 	if (node->left) codegen_dag_node(sregs, writer, node->left);
// 	if (node->right) codegen_dag_node(sregs, writer, node->right);

// 	char buffer[256];

// 	switch (node->kind) {
// 		case DAG_INTEGER_VALUE: {
// 			node->reg = scratch_alloc(sregs);

// 			if (node->reg == -1) return;

// 			snprintf(buffer, sizeof(buffer), "\tmov %s, %d",
// 				scratch_name(sregs, node->reg), node->u.integer_value);
			
// 			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
// 			break;
// 		} 

// 		case DAG_NAME: {
// 			if (!node->symbol) {
// 				fprintf(stderr, "Error: No symbol for DAG_NAME node\n");
// 				return;
// 			}
// 			node->reg = scratch_alloc(sregs);
// 			if (node->reg == -1) return;
// 			snprintf(buffer, sizeof(buffer), "\tmov %s, [%s]",
// 				scratch_name(sregs, node->reg), symbol_codegen(node->symbol));
// 			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
// 			break;
// 		}

// 		case DAG_ASSIGN: {
// 			if (!node->left || !node->right || node->left->kind != DAG_NAME || !node->left->symbol) {
// 				fprintf(stderr, "Error: Invalid assignment node\n");
// 				return;
// 			}

// 			if (node->right->reg == -1) {
// 				fprintf(stderr, "Error: Right operand not evaluated\n");
// 				return;
// 			}
// 			snprintf(buffer, sizeof(buffer), "\tmov [%s], %s",
// 				symbol_codegen(node->left->symbol), scratch_name(sregs, node->right->reg));
// 			asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
// 			node->reg = node->right->reg;
// 			break;
// 		}

// 		case DAG_ADD:
//         case DAG_SUB:
//         case DAG_MUL: {
//             if (!node->left || !node->right || node->left->reg == -1 || node->right->reg == -1) {
//                 fprintf(stderr, "Error: Invalid arithmetic node\n");
//                 return;
//             }
//             if (node->kind == DAG_MUL) {
//                 int save_reg = -1;
//                 if (strcmp(scratch_name(sregs, node->left->reg), "rax") != 0) {
//                     save_reg = scratch_alloc(sregs);
//                     if (save_reg == -1) {
//                         fprintf(stderr, "Error: No free registers for MUL\n");
//                         return;
//                     }
//                     snprintf(buffer, sizeof(buffer), "\tmov %s, rax",
//                              scratch_name(sregs, save_reg));
//                     asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
//                 }

//                 if (strcmp(scratch_name(sregs, node->left->reg), "rax") != 0) {
//                     snprintf(buffer, sizeof(buffer), "\tmov rax, %s",
//                              scratch_name(sregs, node->left->reg));
//                     asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
//                 }

//                 snprintf(buffer, sizeof(buffer), "\tmul %s",
//                          scratch_name(sregs, node->right->reg));
//                 asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

//                 node->reg = scratch_alloc(sregs);
//                 if (node->reg == -1) {
//                     fprintf(stderr, "Error: No free registers for MUL result\n");
//                     return;
//                 }
//                 if (strcmp(scratch_name(sregs, node->reg), "rax") != 0) {
//                     snprintf(buffer, sizeof(buffer), "\tmov %s, rax",
//                              scratch_name(sregs, node->reg));
//                     asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
//                 }

//                 if (save_reg != -1) {
//                     snprintf(buffer, sizeof(buffer), "\tmov rax, %s",
//                              scratch_name(sregs, save_reg));
//                     asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
//                     scratch_free(sregs, save_reg);
//                 }
//             } else {
             
//                 snprintf(buffer, sizeof(buffer), "\t%s %s, %s",
//                          (node->kind == DAG_ADD) ? "add" : "sub",
//                          scratch_name(sregs, node->left->reg),
//                          scratch_name(sregs, node->right->reg));
//                 asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
//                 node->reg = node->left->reg;
//                 scratch_free(sregs, node->right->reg);
//             }
//             scratch_free(sregs, node->left->reg);
//             break;
//         }

// 		default:
// 			break;

// 	}
// }

// void codegen_dag(struct RegisterTable* sregs, struct AsmWriter* writer, struct DAG* dag) {
// 	if (!sregs || !writer || !dag) return;

// 	char buffer[256];

// 	for (int i = 0; i < dag->node_count; i++) {
// 		struct dag_node* node = dag->nodes[i];
// 		if (!node || node->freed) continue;
// 		codegen_dag_node(sregs, writer, node);
// 	}
// }

// void codegen_block(struct RegisterTable* sregs, struct AsmWriter* writer, struct basic_block* block) {
//     if (!sregs || !writer || !block || block->emitted_x86_assembly) return;

//     char buffer[256];

//     // Generate a unique label for the block
//     int label = label_create();
//     snprintf(buffer, sizeof(buffer), "%s:", label_name(label));
//     asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

//     // Generate code for the block's DAG
//     if (block->dag) {
//         codegen_dag(sregs, writer, block->dag);
//     }

//     // Handle control flow to successors
//     if (block->successor_count > 0) {
//         if (block->successor_count == 1) {
//             snprintf(buffer, sizeof(buffer), "\tjmp %s",
//                      label_name(block->successors[0]->label));
//             asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);
//         } else if (block->successor_count == 2) {
//             if (block->dag && block->dag->node_count > 0) {
//                 struct dag_node* cond_node = block->dag->nodes[0];
//                 if (cond_node && cond_node->reg != -1) {
//                     snprintf(buffer, sizeof(buffer), "\tcmp %s, 0",
//                              scratch_name(sregs, cond_node->reg));
//                     asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

//                     const char* jump_inst = NULL;
//                     switch (cond_node->kind) {
//                         case DAG_LESS: jump_inst = "jl"; break;
//                         case DAG_GREATER: jump_inst = "jg"; break;
//                         case DAG_LESS_EQUAL: jump_inst = "jle"; break;
//                         case DAG_GREATER_EQUAL: jump_inst = "jge"; break;
//                         case DAG_EQUAL: jump_inst = "je"; break;
//                         case DAG_NOT_EQUAL: jump_inst = "jne"; break;
//                         default: jump_inst = "jnz"; break;
//                     }
//                     snprintf(buffer, sizeof(buffer), "\t%s %s",
//                              jump_inst, label_name(block->successors[0]->label));
//                     asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

//                     snprintf(buffer, sizeof(buffer), "\tjmp %s",
//                              label_name(block->successors[1]->label));
//                     asm_to_write_section(writer, buffer, TEXT_DIRECTIVE);

//                     scratch_free(sregs, cond_node->reg);
//                 }
//             }
//         }
//     }

//     block->emitted_x86_assembly = true;
//     block->label = label;
// }

// void codegen_CFG(struct RegisterTable* sregs, struct AsmWriter* writer, struct CFG* cfg) {
// 	if (cfg->blocks) {
// 		for (int i = 0; i < cfg->block_count; i++) {
// 			if (cfg->blocks[i]->emitted_x86_assembly) continue;

// 			cfg->blocks[i]->emitted_x86_assembly = true;
// 			codegen_block(sregs, writer, cfg->blocks[i]);
// 		}
// 	}
// }

// void process_CFG(struct RegisterTable* sregs, struct AsmWriter* writer, struct CFG* cfg) {
// 	if (!sregs || !writer || !cfg) return;

// 	while (cfg) {
// 		struct cfg* next = cfg->next;
// 		codegen_CFG(sregs, writer, cfg);
// 		cfg = next;
// 	}
// }

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