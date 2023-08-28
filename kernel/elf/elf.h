#pragma once
#include <stdint.h>
#include <stddef.h>
#include <libm/env/env.h>


namespace Elf
{
    // ELF-64 Object File Format 1.5d2 p. 2
    typedef uint64_t Elf64_Addr;
    typedef uint64_t Elf64_Off;
    typedef uint16_t Elf64_Half;
    typedef uint32_t Elf64_Word;
    typedef int32_t  Elf64_Sword;
    typedef uint64_t Elf64_Xword;
    typedef int64_t  Elf64_Sxword;

    typedef struct proc process_t;

	struct elf_info_t
    {
		uint64_t entry;
		uint64_t phdr_segment;
		uint64_t phdr_entry_size;
		uint64_t phdr_num;

		const char* linker_path;
	};

    constexpr uint32_t MAGIC        = 0x7f454c46; // ELF Magic as one unit

    // File Types - ELF-64 Object File Format 1.5d2 p. 5
    constexpr Elf64_Half ET_NONE    = 0x0000;
    constexpr Elf64_Half ET_REL     = 0x0001;
    constexpr Elf64_Half ET_EXEC    = 0x0002;
    constexpr Elf64_Half ET_DYN     = 0x0003;
    constexpr Elf64_Half ET_CORE    = 0x0004;
    constexpr Elf64_Half ET_LOOS    = 0xFE00;
    constexpr Elf64_Half ET_HIOS    = 0xFEFF;
    constexpr Elf64_Half ET_LOPROC  = 0xFF00;
    constexpr Elf64_Half ET_HIPROC  = 0xFFFF;

    // ELF Identification - ELF-64 Object File Format 1.5d2 p. 3
    constexpr uint8_t EI_MAG0       = 0;
    constexpr uint8_t EI_MAG1       = 1;
    constexpr uint8_t EI_MAG2       = 2;
    constexpr uint8_t EI_MAG3       = 3;
    constexpr uint8_t EI_CLASS      = 4;
    constexpr uint8_t EI_DATA       = 5;
    constexpr uint8_t EI_VERSION    = 6;
    constexpr uint8_t EI_OSABI      = 7;
    constexpr uint8_t EI_ABIVERSION = 8;
    constexpr uint8_t EI_PAD        = 9;
    constexpr uint8_t EI_NIDENT     = 0x10;

    // Sgement Types - ELF-64 Object File Format 1.5d2 p. 12
    constexpr uint32_t PT_NULL      = 0;
    constexpr uint32_t PT_LOAD      = 1;
    constexpr uint32_t PT_DYNAMIC   = 2;
    constexpr uint32_t PT_INTERP    = 3;
    constexpr uint32_t PT_NOTE      = 4;
    constexpr uint32_t PT_SHLIB     = 5;
    constexpr uint32_t PT_PHDR      = 6;
    constexpr uint32_t PT_LOOS      = 0x60000000;
    constexpr uint32_t PT_HIOS      = 0x6fffffff;
    constexpr uint32_t PT_LOPROC    = 0x70000000;
    constexpr uint32_t PT_HIPROC    = 0x7fffffff;

    // Segment Attributes - ELF-64 Object File Format 1.5d2 p. 13
    constexpr uint32_t PF_X         = 0x1;
    constexpr uint32_t PF_W         = 0x2;
    constexpr uint32_t PF_R         = 0x4;
    constexpr uint32_t PF_MASKOS    = 0x00ff0000;
    constexpr uint32_t PF_MASKPROC  = 0xff000000;

    // Object File Classes - ELF-64 Object File Format 1.5d2 p. 5
    constexpr uint8_t ELFCLASS32    = 1;
    constexpr uint8_t ELFCLASS64    = 2;

	constexpr uint64_t DT_NULL      = 0x00000000;
	constexpr uint64_t DT_NEEDED    = 0x00000001;
	constexpr uint64_t DT_RELA      = 0x00000007;
	constexpr uint64_t DT_RELASZ    = 0x00000008;
	constexpr uint64_t DT_RELAENT   = 0x00000009;


	// ELF File Header - ELF-64 Object File Format 1.5d2 p. 3
	struct __attribute__((packed)) Elf64_Ehdr 
    {
		union {
			unsigned char c[EI_NIDENT];
			uint32_t i;
		} e_ident;

		Elf64_Half e_type;
		Elf64_Half e_machine;
		Elf64_Word e_version;
		Elf64_Addr e_entry;
		Elf64_Off e_phoff;
		Elf64_Off e_shoff;
		Elf64_Word e_flags;
		Elf64_Half e_ehsize;
		Elf64_Half e_phentsize;
		Elf64_Half e_phnum;
		Elf64_Half e_shentsize;
		Elf64_Half e_shnum;
		Elf64_Half e_shstrndx;
	};

	// Program Header Table Entry - ELF-64 Object File Format 1.5d2 p. 12
	typedef struct {
		Elf64_Word p_type;
		Elf64_Word p_flags;
		Elf64_Off p_offset;
		Elf64_Addr p_vaddr;
		Elf64_Addr p_paddr;
		Elf64_Xword p_filesz;
		Elf64_Xword p_memsz;
		Elf64_Xword p_align;    
	} __attribute__((packed)) Elf64_Phdr;


	#define ELF64_R_SYM(i)    ((i) >> 32)
	#define ELF64_R_TYPE(i)   ((i) & 0xFFFFFFFFL)
	#define ELF64_R_INFO(s,t) (((s) << 32) + ((t) & 0xFFFFFFFFL))

	#define R_X86_64_64 1
	#define R_X86_64_32 10
	#define R_X86_64_32S 11
	#define R_X86_64_PC32 2
	#define R_X86_64_PLT32 4
	#define R_X86_64_RELATIVE 8

	typedef struct {
		Elf64_Addr r_offset;
		Elf64_Xword r_info; 
		Elf64_Sxword r_addend;
	} __attribute__((packed)) Elf64_Rela;

	typedef struct {
		Elf64_Word sh_name;
		Elf64_Word sh_type;
		Elf64_Xword sh_flags;
		Elf64_Addr sh_addr;
		Elf64_Off sh_offset;
		Elf64_Xword sh_size;
		Elf64_Word sh_link;
		Elf64_Word sh_info;
		Elf64_Xword sh_addralign;
		Elf64_Xword sh_entsize;
	} __attribute__((packed)) Elf64_Shdr;


	typedef struct {
		Elf64_Word st_name;
		uint8_t st_info;
		uint8_t st_other;
		Elf64_Half st_shndx;
		Elf64_Addr st_value;
		Elf64_Xword st_size;
	} __attribute__((packed)) Elf64_Sym;

	typedef struct {
		uint64_t d_tag;
		uint64_t d_un;
	} __attribute__((packed)) Elf64_Dyn;

	struct symbol_table_information {
		Elf64_Shdr* shdr;
		size_t shdr_count;
		Elf64_Sym* sym_entries;
		size_t sym_count;
		char* sym_str_table;
		size_t sym_str_table_size;
		char* sect_str_table;
		size_t sect_str_table_size;
	};



    struct LoadedElfFile
    {
        uint8_t* data;
        uint64_t size;
        void* entryPoint;
        void* offset;
        bool works;
    };

    extern LoadedElfFile borkedElfFile;

    LoadedElfFile LoadElf(uint8_t* data);

    void RunElfHere(LoadedElfFile file, int argc, char** argv, ENV_DATA* envData);

}