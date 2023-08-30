#include "saf.h"
#include <malloc.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define MAX_PATH_SIZE 1024

static void panic(const char* msg, ...)
{
    fprintf(stderr, "\033[31merror: \033[0m");
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);
    exit(EXIT_FAILURE);
}

static void info(const char* msg, ...)
{
    printf("\033[32minfo: \033[0m");
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);
}

static void parsenode(char* path, void* base, saf_node_hdr_t* node, int level)
{
    if (node->magic != MAGIC_NUMBER)
        panic("archive is damaged or of invalid format\n");
    info("processing node '%s'\n", node->name);

    char* fullpath = malloc(MAX_PATH_SIZE);
    sprintf(fullpath, "%s/%s", path, node->name);

    struct stat st;
    if (stat(fullpath, &st) != -1)
        panic("node '%s' already exists\n", fullpath);

    if (node->flags == FLAG_ISFOLDER) {
        saf_node_folder_t* fldr = (saf_node_folder_t*)node;
        mkdir(fullpath, 0700);
        for (size_t i = 0; i < fldr->num_children; i++)
            parsenode(fullpath, base, (saf_node_hdr_t*)((uint8_t*)base + fldr->children[i]), level + 1);
    } else {
        saf_node_file_t* file = (saf_node_file_t*)node;
        FILE* f = fopen(fullpath, "wb");
        fwrite((uint8_t*)base + file->addr, 1, file->size, f);
        fclose(f);
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        printf("Usage: saf-extract <archive-name> <output-dir>\n");
        exit(EXIT_FAILURE);
    }

    char* input = argv[1];
    char* output = "extracted-files";
    if (argc == 2)
        info("using output folder '%s'\n", output);
    else
        output = argv[2];

    FILE* f = fopen(input, "rb");
    if (!f)
        panic("could not open file '%s'\n", input);

    void* buff;
    size_t bufflen;

    fseek(f, 0, SEEK_END);
    bufflen = ftell(f);
    fseek(f, 0, SEEK_SET);

    buff = malloc(bufflen);
    fread(buff, 1, bufflen, f);

    parsenode(output, buff, (saf_node_hdr_t*)buff, 0);
}
