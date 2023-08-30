#include "elf.h"
#include "../devices/serial/serial.h"
#include "../kernelStuff/memory/memory.h"
#include "../paging/PageFrameAllocator.h"
#include "../paging/PageTableManager.h"

namespace Elf
{
    void elf_apply_relocations(void* ptr, Elf64_Ehdr* header, uint64_t vaddr, uint64_t elf_load_offset, uint64_t size) 
    {
        Elf64_Phdr* ph = (Elf64_Phdr*) (((char*) ptr) + header->e_phoff);
        for (int i = 0; i < header->e_phnum; i++, ph++) 
        {
            if (ph->p_type != PT_DYNAMIC) 
                continue;
            

            //debugf("dynamic section found at %x\n", ph);

            uint64_t rela_offset = 0;
            uint64_t rela_size = 0;
            uint64_t rela_ent = 0;

            for (uint16_t j = 0; j < ph->p_filesz / sizeof(Elf64_Dyn); j++) 
            {
                Elf64_Dyn* dyn = (Elf64_Dyn*) (((char*) ptr) + ph->p_offset + j * sizeof(Elf64_Dyn));

                switch (dyn->d_tag) 
                {
                    case DT_RELA:
                        rela_offset = dyn->d_un;
                        break;
                    case DT_RELAENT:
                        rela_ent = dyn->d_un;
                        break;
                    case DT_RELASZ:
                        rela_size = dyn->d_un;
                        break;
                }
            }

            //debugf("rela_offset: %x\n", rela_offset);
            //debugf("rela_size: %x\n", rela_size);
            //debugf("rela_ent: %x\n", rela_ent);

            if (rela_offset != 0) {
                //assert(rela_ent == sizeof(Elf64_Rela));

                for (uint64_t off = 0; off < rela_size; off += rela_ent) 
                {
                    Elf64_Rela* rela = (Elf64_Rela*) (((char*) ptr) + rela_offset + off);

                    if (rela->r_info == R_X86_64_RELATIVE) 
                    {
                        uint64_t* target = (uint64_t*) (elf_load_offset + rela->r_offset);
                        *target = elf_load_offset + rela->r_addend;
                    }
                }
            }
        }
    }

    LoadedElfFile borkedElfFile = {NULL, 0, NULL, NULL, false};

    LoadedElfFile LoadElf(uint8_t* data)
    {
        Elf64_Ehdr* header = (Elf64_Ehdr*) data;

        if (data == NULL)
        {
            Serial::Writelnf("ELF> DATA IS NULL!");
            return borkedElfFile;
        }
        if(__builtin_bswap32(header->e_ident.i) != MAGIC) 
        {
            Serial::Writelnf("ELF> NUMBER IS %x and not %x", __builtin_bswap32(header->e_ident.i), MAGIC);
            Serial::Writelnf("ELF> (NUMBER IS %x)", header->e_ident.i);
            Serial::Writelnf("ELF> NOT AN ELF!");
            return borkedElfFile; // not an elf
        }
        if(header->e_ident.c[EI_CLASS] != ELFCLASS64) 
        {
            Serial::Writelnf("ELF> NOT 64 BIT!");
            return borkedElfFile; // not 64 bit
        }
        if(header->e_type != ET_DYN) 
        {
            Serial::Writelnf("ELF> NOT PIC!");
            return borkedElfFile; // not pic
        }
        

        Elf64_Phdr* ph = (Elf64_Phdr*) (((char*) data) + header->e_phoff);
        Serial::Writelnf("ELF> ph: %x\n", ph);

        void* last_dest;

        for (int i = 0; i < header->e_phnum; i++, ph++) 
        {
            if (ph->p_type != PT_LOAD) 
                continue;
    
            last_dest = (void*) ((uint64_t) ph->p_vaddr + ph->p_memsz);
        }

        void* offset = GlobalAllocator->RequestPages((uint64_t) last_dest / 0x1000 + 1);
        GlobalPageTableManager.MapMemories((void*)offset, (void*)offset, (uint64_t) last_dest / 0x1000 + 1, PT_Flag_Present | PT_Flag_ReadWrite | PT_Flag_UserSuper);
        Serial::Writelnf("offset: %x\n", offset);

        ph = (Elf64_Phdr*) (((char*) data) + header->e_phoff);
        for (int i = 0; i < header->e_phnum; i++, ph++)
        {
            void* dest = (void*) ((uint64_t) ph->p_vaddr + (uint64_t) offset);
            void* src = ((char*) data) + ph->p_offset;


            if (ph->p_type != PT_LOAD) 
                continue;
            
        
            _memset(dest, 0, ph->p_memsz);
            _memcpy(src, dest, ph->p_filesz);

            elf_apply_relocations(data, header, (uint64_t) ph->p_vaddr, (uint64_t) offset, ph->p_filesz);
        }
        

        LoadedElfFile file;
        file.entryPoint = (void*) (header->e_entry + (uint64_t) offset);
        file.offset = offset;
        file.data = data;
        file.size = (uint64_t) last_dest / 0x1000 + 1;
        file.works = true;

        return file;
    }

    void RunElfHere(LoadedElfFile file, int argc, char** argv, ENV_DATA* envData)
    {
        if (file.works)
        {
            void (*entry)(int, char**, ENV_DATA*) = (void (*)(int, char**, ENV_DATA*)) file.entryPoint;
            Serial::Writelnf("ELF> ENTRY POINT: %x", entry);
            entry(argc, argv, envData);
        }
    }
}
