#include "saf.h"
#include "../kernelStuff/memory/memory.h"
#include "../devices/serial/serial.h"
#include <libm/cstr.h>
#include <libm/cstrTools.h>
#include "../paging/PageTableManager.h"
#include "../paging/PageFrameAllocator.h"

namespace SAF
{
    saf_node_hdr_t* initrd_find(char* path, void* base, saf_node_hdr_t* current) 
    {
        //Serial::Writelnf("initrd_find(%s, %X, %X, %s)", path, base, current, current->name);

        while (*path == '/') 
            path++;
        
        if (*path == 0) 
            return current;

        char buffer[128] = { 0 };
        int index = StrIndexOf(path, '/');
        if (index == -1) 
            index = StrLen(path);
        char* next = path + index;//copy_until('/', path, buffer);
        _memcpy(path, buffer, index);
        
        //assert(current->flags == FLAG_ISFOLDER);
        saf_node_folder_t* folder_node = (saf_node_folder_t*) current;

        for (int i = 0; i < folder_node->num_children; i++) {
            saf_node_hdr_t* child = (saf_node_hdr_t*) ((uint32_t) base + (uint32_t) folder_node->children[i]);
            if (StrEquals(buffer, child->name)) 
            {
                return initrd_find(next, base, child);
            }
        }

        return NULL;
    }


    file_t* initrd_open(initrdMount* mount, char* path, int flags) 
    {
        //Serial::Writelnf("open: %s", path);

        saf_node_hdr_t* file = initrd_find(path, mount->driver_specific_data, (saf_node_hdr_t*) mount->driver_specific_data);
        if (file == NULL) {
            Serial::Writelnf("file %s not found", path);
            return NULL;
        }

        if (file->flags == FLAG_ISFOLDER) {
            Serial::Writelnf("file %s is a folder", path);
            return NULL;
        }

        saf_node_file_t* file_node = (saf_node_file_t*) file;

        file_t* f = (file_t*)GlobalAllocator->RequestPages(1);
        f->mount = mount;
        f->size = file_node->size;
        f->driver_specific_data = (void*) ((uint32_t) mount->driver_specific_data + (uint32_t) file_node->addr);

        return f;
    }

    file_t* LoadFileFromNode(initrdMount* mount, saf_node_file_t* file)
    {
        saf_node_file_t* file_node = file;

        file_t* f = (file_t*)GlobalAllocator->RequestPages(1);
        f->mount = mount;
        f->size = file_node->size;
        f->driver_specific_data = (void*) ((uint32_t) mount->driver_specific_data + (uint32_t) file_node->addr);

        return f;
    }

    void initrd_close(initrdMount* mount, file_t* f) 
    {
        GlobalAllocator->FreePages((void*)f, 1);
    }

    void initrd_read(initrdMount* mount, file_t* f, void* buffer, size_t size, size_t offset) 
    {
        //assert(f->size >= offset + size);

        _memcpy((void*) ((uint32_t) f->driver_specific_data + offset), buffer, size);
    }

    dir_t initrd_dir_at(initrdMount* mount, int idx, char* path)
    {
        // debugf("dir_at: %s (%d)", path, idx);
        char path_cpy[StrLen(path) + 1];
        _memcpy(path, path_cpy, StrLen(path) + 1);
        path_cpy[StrLen(path)] = 0;;

        saf_node_hdr_t* folder = initrd_find(path, mount->driver_specific_data, (saf_node_hdr_t*) mount->driver_specific_data);

        if (folder == NULL) {
            dir_t dir = {
                .is_none = true,
            };
            return dir;
        }

        if (folder->flags != FLAG_ISFOLDER) {
            dir_t dir = {
                .is_none = true,
            };
            return dir;
        }

        saf_node_folder_t* folder_node = (saf_node_folder_t*) folder;

        if (idx > folder_node->num_children - 1) {
            dir_t dir = {
                .is_none = true,
            };
            return dir;
        } else {
            saf_node_hdr_t* child = (saf_node_hdr_t*) ((uint32_t) mount->driver_specific_data + (uint32_t) folder_node->children[idx]);

            dir_t dir;
            _memset(&dir, 0, sizeof(dir));

            _memcpy(child->name, dir.name, StrLen(child->name) + 1);
            dir.idx = idx;
            dir.is_none = false;

            if (child->flags == FLAG_ISFOLDER) {
                dir.type = ENTRY_DIR;
            } else {
                dir.type = ENTRY_FILE;
            }

            return dir;
        }
    }

    initrdMount* initrd_mount(void* saf_image) 
    {
        initrdMount* mount = (initrdMount*) GlobalAllocator->RequestPages(1);
        _memset(mount, 0, sizeof(initrdMount));

        mount->driver_specific_data = saf_image;

        return mount;
    }
}