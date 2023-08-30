#include "saf.h"
#include <dirent.h>
#include <memory.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define INITIAL_ALLOC_SIZE (4ULL * 1024 * 1024 * 1024)
#define MAX_PATH_SIZE 1024

static bool is_quiet = false;

typedef struct {
    uint8_t* ptr;
    uint64_t curr_offset;
    uint64_t alloc_size;
} arch_data_t;

typedef struct {
    uint64_t len;
    saf_offset_t offsets[];
} nodelist_t;

static arch_data_t adata;

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
    if (is_quiet)
        return;

    printf("\033[32minfo: \033[0m");
    va_list args;
    va_start(args, msg);
    vprintf(msg, args);
    va_end(args);
}

static void inc_offset(uint64_t d)
{
    adata.curr_offset += d;
    if (adata.curr_offset > adata.alloc_size)
        panic("archive size exceeds 4 GB\n");
}

// count number of children of a directory
static uint64_t childcnt(const char* fldr)
{
    DIR* dir = opendir(fldr);
    if (!dir)
        panic("could not open directory '%s'\n", fldr);

    uint64_t cnt = 0;
    struct dirent* ent;
    while ((ent = readdir(dir))) {
        if (!memcmp(ent->d_name, ".", 2) || !memcmp(ent->d_name, "..", 3))
            continue;
        cnt++;
    }
    closedir(dir);
    return cnt;
}

static nodelist_t* make_node(char* path)
{
    uint64_t nc = childcnt(path);
    nodelist_t* ret = malloc(sizeof(nodelist_t) + (sizeof(saf_offset_t) * nc));
    ret->len = 0;

    DIR* dir = opendir(path);

    if (!dir)
        panic("could not open directory '%s'\n", path);

    struct dirent* ent;
    while ((ent = readdir(dir))) {
        if (!memcmp(ent->d_name, ".", 2) || !memcmp(ent->d_name, "..", 3))
            continue;

        char* fullpath = (char*)malloc(MAX_PATH_SIZE);
        sprintf(fullpath, "%s/%s", path, ent->d_name);
        info("adding node %s\n", fullpath);

        // ignore things that are not files or folders
        if (ent->d_type != DT_DIR && ent->d_type != DT_REG) {
            info("ignoring node %s with unknown type\n", fullpath);
            continue;
        }

        ret->offsets[ret->len++] = adata.curr_offset;
        saf_node_hdr_t* hdr = (saf_node_hdr_t*)(adata.ptr + adata.curr_offset);
        hdr->magic = MAGIC_NUMBER;
        memcpy(&(hdr->name), &(ent->d_name), sizeof(hdr->name));

        // calculate size of node structure
        size_t nodesize;
        if (ent->d_type == DT_DIR)
            nodesize = sizeof(saf_node_folder_t) + (childcnt(fullpath) * sizeof(saf_offset_t));
        else
            nodesize = sizeof(saf_node_file_t);
        inc_offset(nodesize);
        hdr->len = nodesize;

        // the node is a folder
        if (ent->d_type == DT_DIR) {
            saf_node_folder_t* fldr = (saf_node_folder_t*)(hdr);
            fldr->hdr.flags = FLAG_ISFOLDER;
            nodelist_t* children = make_node(fullpath);
            fldr->num_children = children->len;
            for (size_t i = 0; i < children->len; i++)
                fldr->children[i] = children->offsets[i];
        }
        // the node is a file
        else if (ent->d_type == DT_REG) {
            saf_node_file_t* file = (saf_node_file_t*)(hdr);
            file->hdr.flags = 0;
            file->addr = adata.curr_offset;
            FILE* f = fopen(fullpath, "rb");
            if (!f)
                panic("could not open file %s\n", fullpath);
            fseek(f, 0, SEEK_END);
            file->size = ftell(f);
            fseek(f, 0, SEEK_SET);
            inc_offset(file->size);
            fread((uint8_t*)file + sizeof(saf_node_file_t), 1, file->size, f);
            fclose(f);
        }
    }
    closedir(dir);
    return ret;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        printf("Usage: saf-make <folder-name> <archive-name> [-q]\n");
        exit(EXIT_FAILURE);
    }
    is_quiet = (argc == 4);

    char* input = argv[1];
    char* output = "archive.saf";
    if (argc == 2)
        info("using default output filename '%s'\n", output);
    else
        output = argv[2];

    adata = (arch_data_t) {
        .ptr = malloc(INITIAL_ALLOC_SIZE),
        .curr_offset = 0,
        .alloc_size = INITIAL_ALLOC_SIZE
    };

    // create the root node
    saf_node_folder_t* root = (saf_node_folder_t*)(adata.ptr + adata.curr_offset);
    size_t nodesize = sizeof(saf_node_folder_t) + (childcnt(input) * sizeof(saf_offset_t));
    inc_offset(nodesize);
    root->hdr.magic = MAGIC_NUMBER;
    root->hdr.len = nodesize;
    root->hdr.flags = FLAG_ISFOLDER;
    nodelist_t* children = make_node(input);
    root->num_children = children->len;
    for (size_t i = 0; i < children->len; i++)
        root->children[i] = children->offsets[i];

    info("final archive size: %d bytes\n", adata.curr_offset);
    FILE* of = fopen(output, "wb");
    fwrite(adata.ptr, 1, adata.curr_offset, of);
    fclose(of);
    info("done\n");
}
