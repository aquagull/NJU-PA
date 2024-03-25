#include <elf.h>
#include <stdio.h>
#include <string.h>

#include "elfload.h"
static FUNC_INFO elf_funcs[1024];

static void read_file(FILE *elf, size_t offset, size_t size, void *dest)
{
    fseek(elf, offset, SEEK_SET);         // 移动文件指示器到offset
    int flag = fread(dest, size, 1, elf); // 读1个size大小的数据到dest中
    assert(flag == 1);
}

static void get_str_from_file(FILE *elf, size_t offset, size_t n, void *dest)
{
    fseek(elf, offset, SEEK_SET);
    char *flag = fgets(dest, n, elf);
    assert(flag != NULL);
}

static int end;
static void append(char *func_name, paddr_t start, size_t size)
{
    
    strncpy(elf_funcs[end].func_name, func_name, sizeof(elf_funcs[0].func_name));
    elf_funcs[end].start = start;
    elf_funcs[end].size = size;
    end++;
}

void init_elf(const char *elf_file, size_t global_offset)
{
    FILE *elf = fopen(elf_file, "rb");
    assert(elf != NULL);

    Elf32_Ehdr elf_header;
    read_file(elf, global_offset, sizeof(elf_header), &elf_header);

    Elf32_Off section_header_offset = elf_header.e_shoff; // 偏移
    size_t headers_entry_size = elf_header.e_shentsize;   // 读取节头部表大小
    int headers_entry_num = elf_header.e_shnum;           // 项数

    assert(sizeof(Elf32_Shdr) == headers_entry_size);

    Elf32_Off symbol_table_offset = 0, string_table_offset = 0;
    size_t symbol_tabel_total_size = 0;
    size_t symbol_table_entry_size = 0;

    for (int i = 0; i < headers_entry_num; i++)
    {
        // 遍历每一个节头部表
        Elf32_Shdr section_entry;
        read_file(elf, global_offset + i * headers_entry_size + section_header_offset,
                  headers_entry_size, &section_entry); // 读取一个节头部表
        switch (section_entry.sh_type)
        {
        case SHT_SYMTAB:
            symbol_table_offset = section_entry.sh_offset;
            symbol_tabel_total_size = section_entry.sh_size;
            symbol_table_entry_size = section_entry.sh_entsize;
            break;

        case SHT_STRTAB:
            if (i == elf_header.e_shstrndx)
            {
            } // 这个存储的是每个节的名称的字符串表
            else
                string_table_offset = section_entry.sh_offset;
            break;
        }
    }
    char func_name[64];
    assert(symbol_table_entry_size == sizeof(Elf32_Sym));

    for (int i = 0; i < symbol_tabel_total_size / symbol_table_entry_size; i++)
    {
        Elf32_Sym symbol_section_entry;
        read_file(elf, global_offset + i * symbol_table_entry_size + symbol_table_offset,
                  symbol_table_entry_size, &symbol_section_entry);
        switch (ELF32_ST_TYPE(symbol_section_entry.st_info))
        {
        // 获取符号类型
        case STT_FUNC:
            get_str_from_file(elf, global_offset + string_table_offset + symbol_section_entry.st_name,
                              sizeof(func_name), func_name);

            append(func_name, symbol_section_entry.st_value, symbol_section_entry.st_size);
            break;
        }
    }
}

FUNC_INFO *check_func(paddr_t addr)
{
    for (int i = 0; i < end; i++)
    {
        FUNC_INFO *info = &elf_funcs[i];
        if ((addr >= info->start) && (addr < info->start + info->size))
            Log("return info");
        return info;
    }
    return NULL;
}