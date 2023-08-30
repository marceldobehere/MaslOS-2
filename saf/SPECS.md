# SAF specification
Every file and folder in an SAF archive is represented by a *node*. Each node has a *header*, followed by other data depending on the type of the node, i.e. *file* or *folder*. Every archive is guaranteed to start with a node of type *folder* and an empty name, with all other nodes being children of this root node directly or indirectly. The node header structure is as follows:
```C
typedef struct {
    uint64_t magic; // must be equal to 0x766863726c706d73
    uint64_t len; // length of node in bytes
    char name[256]; // null-terminated string containing name of the node
    uint64_t flags; // described below
} __attribute__((packed)) saf_node_hdr_t;
```
The 0th bit of `flags` will be set in case of a folder, and unset in case of a file. All other bits are reserved.
## Folder node
The structure of a folder node is as follows:
```C
typedef struct {
    saf_node_hdr_t hdr; // the node header, described above
    uint64_t num_children; // no of children nodes of this folder
    saf_offset_t children[]; // array containing offsets of children
} __attribute__((packed)) saf_node_folder_t;
```
The `saf_offset_t` type stores offsets of arbitrary data from the starting of the archive. In this case, `children[]` stores the offsets of the nodes which are direct children of this node.
## File node
```C
typedef struct {
    saf_node_hdr_t hdr; // described above
    uint64_t size; // size of file in bytes
    saf_offset_t addr; // offset of file data
} __attribute__((packed)) saf_node_file_t;
```
Here, `addr` contains the offset of the file contents from the start of the archive. Please note that the `len` field in the header does **not** include the length of the file, and only the length of the node itself.
