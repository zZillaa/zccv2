#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include "preprocessor.h"
#include "lexer.h"
#include "ast.h"
#include "IR.h"
#include "codegen.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Error: expected two arguments\n");
        return EXIT_FAILURE;
    }

    char* file_path = argv[1];
    char* contents = get_file_contents(file_path);

    if (contents) {
        printf("Contents of %s\n---\n\"%s\"\n---\n", file_path, contents);
        Preprocessor* preprocessor = preprocess(contents);
        if (preprocessor->output) {
            printf("Preprocessed output:\n---\n\"%s\"\n---\n", preprocessor->output);
        }
        printf("I am here\n");
        Token* tokens = lexical_analysis(preprocessor->output);
        print_tokens(tokens);
        
        struct program* ast = build_ast(tokens);
        print_ast(ast);
    
        // Name resolution and type checking
        struct stack* stack = create_stack();
        scope_enter(stack, NULL);
        program_resolve(ast, stack);    
        program_typecheck(ast, stack);

        // struct CFG* cfg = build_CFG(ast->declaration);

        struct RegisterTable* sregs = create_register_table();
        struct AsmWriter* writer = create_asm_writer("output.asm");
        decl_codegen(sregs, writer, ast->declaration, false);
        // process_CFG(sregs, writer, cfg);
        
        free_asm_writer(writer);
        free_register_table(sregs);
        // free_all_CFG(cfg);
        free_stack(stack);
        free_ast(ast);
        free_tokens(tokens);
        free_preprocessor(preprocessor);
        free(contents);
    }

    return EXIT_SUCCESS;
}