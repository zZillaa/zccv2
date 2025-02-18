#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include "preprocessor.h"
// #include "lexer.h"
// #include "ast.h"
// #include "codegen.h"

long get_file_size(FILE* file) {
    fpos_t posIndicator;
    if (fgetpos(file, &posIndicator) != 0) {
        fprintf(stderr, "fgetpos() failed\n");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    
    if (fsetpos(file, &posIndicator) != 0) {
        fprintf(stderr, "fsetpos() failed\n");
        exit(EXIT_FAILURE);
    }

    return file_size;
}

char* get_file_contents(const char* file_path) {
    FILE* file = fopen(file_path, "rb");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    long file_size = get_file_size(file);
    char* contents = malloc(file_size + 1);
    char* writeIt = contents;

    size_t bytes_read = 0;
    while (bytes_read < file_size) {
        size_t curr_bytes_read = fread(writeIt, 1, file_size - bytes_read, file);
        bytes_read += curr_bytes_read;
        writeIt += curr_bytes_read;

        if (feof(file)) { break; }
    }

    if (bytes_read < file_size) {
        printf("Bytes read: %zu\nFile size: %li\n", bytes_read, file_size);
        free(contents);
        exit(EXIT_FAILURE);
    }
    contents[bytes_read] ='\0';
    fclose(file);

    return contents;

}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Error: expected two arguments\n");
        return EXIT_FAILURE;
    }

    const char* file_path = argv[1];
    char* contents = get_file_contents(file_path);
    if (contents != NULL) {
        printf("Contents of %s\n---\n\"%s\"\n---\n", file_path, contents);
        Preprocessor* preprocessor = preprocess(contents);
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
        free_preprocessor(preprocessor);
        free(contents);
    }

    return EXIT_SUCCESS;
}