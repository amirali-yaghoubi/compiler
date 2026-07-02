#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"


char* read_file(const char* path);

int main()
{
    const char* valid_source_path = "src/valid.txt";


    char* source = read_file(valid_source_path);
    if (!source)
    {
        fprintf(stderr, "Failed to read %s\n", valid_source_path);
        return 1;
    }


    Lexer lexer = {0};
    lexer_init(&lexer, source);

    printf("source :\n%s\n", source);

    while (1)
    {
    Token token = lex_next_token(&lexer);

    if (token.type == TOK_EOF) break;
    
    printf("%-18s  length: %-7d  value: %-9ld  line: %-5d  start: %p\n",
       TokenTypeStr((TokenType)token.type),
       token.length,
       token.value.int_value,
       token.line,
       token.start);

    }

    free(source);

    return 0;
}


char* read_file(const char* path)
{
    FILE* file = fopen(path, "rb");
    if (!file)
    {
        perror("fopen");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size_temp = ftell(file);
    if (size_temp < 0)
    {
        fclose(file);
        fprintf(stderr, "Error in reading %s\n", path);
        return NULL;
    }
    size_t size = (size_t)size_temp;
    rewind(file);

    char* buffer = malloc(size + 1);
    if (!buffer)
    {
        fclose(file);
        fprintf(stderr, "Error in allocating memory\n");
        return NULL;
    }

    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);
    return buffer;
}
