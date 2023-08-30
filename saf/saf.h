#pragma once

#include <stdint.h>

#define MAGIC_NUMBER 0x766863726c706d73
#define FLAG_ISFOLDER (1 << 0)

// used to store offsets from start of archive
typedef uint64_t saf_offset_t;

typedef struct {
    uint64_t magic; // must equal MAGIC_NUMBER
    uint64_t len; // length of structure in bytes
    char name[256]; // name of node
    uint64_t flags;
} __attribute__((packed)) saf_node_hdr_t;

typedef struct {
    saf_node_hdr_t hdr;
    uint64_t size; // size of file
    saf_offset_t addr; // offset of file
} __attribute__((packed)) saf_node_file_t;

typedef struct {
    saf_node_hdr_t hdr;
    uint64_t num_children; // no of children
    saf_offset_t children[]; // array containing offsets of children
} __attribute__((packed)) saf_node_folder_t;