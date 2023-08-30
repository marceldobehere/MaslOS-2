#pragma once
#include <stdint.h>
#include <stddef.h>

namespace SAF
{
    struct file_t;
    struct dir_t;

    enum file_open_mode_t 
    {
        FILE_OPEN_MODE_READ,
        FILE_OPEN_MODE_WRITE,
        FILE_OPEN_MODE_READ_WRITE
    };

    struct initrdMount 
    {
        file_t* (*open)(struct vfs_mount* mount, char* path, int flags);
        void (*close)(struct vfs_mount* mount, struct file* file);
        void (*read)(struct vfs_mount* mount, struct file* file, void* buf, size_t size, size_t offset);
        void (*write)(struct vfs_mount* mount, struct file* file, void* buf, size_t size, size_t offset);
        void (*_delete)(struct vfs_mount* mount, struct file* file);
        void (*mkdir)(struct vfs_mount* mount, char* path);
        void (*touch)(struct vfs_mount* mount, char* path);

        dir_t (*dir_at)(struct vfs_mount* mount, int idx, char* path);
        void (*delete_dir)(struct vfs_mount* mount, char* path);

        void (*truncate)(struct vfs_mount* mount, struct file* file, size_t new_size);

        char* (*name)(struct vfs_mount* mount);

        void* driver_specific_data;

    };

    struct file_t 
    {
        initrdMount* mount;
        int mode;
        void* driver_specific_data;
        char buffer[512];
        size_t size;
    };

    enum dir_entry_type_e 
    {
        ENTRY_FILE,
        ENTRY_DIR
    };

    struct dir_t 
    {
        char name[256];
        int idx;
        bool is_none;
        int type;
    };

    // void vfs_init();

    // void vfs_mount(vfs_mount_t* mount);

    // file_t* vfs_open(char* path, int flags);
    // void vfs_close(file_t* file);
    // void vfs_read(file_t* file, void* buf, size_t size, size_t offset);
    // void vfs_write(file_t* file, void* buf, size_t size, size_t offset);
    // void vfs_delete(file_t* file);
    // void vfs_mkdir(char* path);
    // void vfs_touch(char* path);

    // void vfs_truncate(file_t* file, size_t new_size);

    // dir_t vfs_dir_at(int idx, char* path);
    // void vfs_delete_dir(char* path);

    // bool vfs_fs_at(int idx, char* out);

    // typedef vfs_mount_t* (*fs_scanner)(int disk_id);
    // void vfs_register_fs_scanner(fs_scanner scanner);
    // void vfs_scan_fs();

    // bool try_read_disk_label(char* out, vfs_mount_t* mount);



    #define MAGIC_NUMBER 0x766863726c706d73
    #define FLAG_ISFOLDER (1 << 0)

    // used to store offsets from start of archive
    typedef uint64_t saf_offset_t;

    struct saf_node_hdr_t
    {
        uint64_t magic; // must equal MAGIC_NUMBER
        uint64_t len; // length of structure in bytes
        char name[256]; // name of node
        uint64_t flags;
    } __attribute__((packed));

    struct saf_node_file_t
    {
        saf_node_hdr_t hdr;
        uint64_t size; // size of file
        saf_offset_t addr; // offset of file
    } __attribute__((packed));

    struct saf_node_folder_t
    {
        saf_node_hdr_t hdr;
        uint64_t num_children; // no of children
        saf_offset_t children[]; // array containing offsets of children
    } __attribute__((packed));

    initrdMount* initrd_mount(void* saf_image);



    saf_node_hdr_t* initrd_find(char* path, void* base, saf_node_hdr_t* current);
    file_t* initrd_open(initrdMount* mount, char* path, int flags);
    void initrd_close(initrdMount* mount, file_t* f);
    void initrd_read(initrdMount* mount, file_t* f, void* buffer, size_t size, size_t offset);
    dir_t initrd_dir_at(initrdMount* mount, int idx, char* path);
    initrdMount* initrd_mount(void* saf_image);
    file_t* LoadFileFromNode(initrdMount* mount, saf_node_file_t* file);
}