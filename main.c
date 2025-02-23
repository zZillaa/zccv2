#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include "preprocessor.h"
// #include "lexer.h"
// #include "ast.h"
// #include "codegen.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Error: expected two arguments\n");
        return EXIT_FAILURE;
    }

    char* file_path = argv[1];
    // char* contents = get_file_contents(file_path);
    // preprocessor opens up file.
    char* contents = get_file_contents(file_path);
<<<<<<< HEAD
    if (contents != NULL) {
        printf("Contents of %s\n---\n\"%s\"\n---\n", file_path, contents);
        Preprocessor* preprocessor = preprocess(contents);
        if (preprocessor->output) {
            printf("Preprocessed output:\n---\n\"%s\"\n---\n", preprocessor->output);
        }
=======
    Preprocessor* preprocessor = preprocess(file_path, contents);
    if (preprocessor->output) {
        printf("Preprocessed output:\n---\n\"%s\"\n---\n", preprocessor->output);
    }
    // if (contents) {
        // printf("Contents of %s\n---\n\"%s\"\n---\n", file_path, contents);
        // Preprocessor* preprocessor = preprocess(contents);
        // if (preprocessor->processed_source) {
        //     printf("Preprocessed output:\n---\n\"%s\"\n---\n", preprocessor->processed_source);
        // }
>>>>>>> 8e0126cb350316c8a6962c814ab27d1140b47e05
        // Token* tokens = lexical_analysis(processed_output);
        // print_tokens(tokens);
        
        // struct program* ast = build_ast(tokens);
        // print_ast(ast);
    
        // // Name resolution and type checking
        // struct stack* stack = create_stack();
        // scope_enter(stack, NULL);
        // program_resolve(ast, stack);    
        // program_typecheck(ast, stack);

        // struct RegisterTable* sregs = create_register_table();
        // struct AsmWriter* writer = create_asm_writer("output.asm");
        // decl_codegen(sregs, writer, ast->declaration, false);
        
        // free_asm_writer(writer);
        // free_register_table(sregs);
        // free_stack(stack);
        // free_ast(ast);
        // free_tokens(tokens);
        // free_preprocessor(preprocessor);
        // free(contents);
    // }

    return EXIT_SUCCESS;
}