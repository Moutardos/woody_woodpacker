#ifndef UTILS_H
#define UTILS_H

#include <elf.h>

#define PAGE_SIZE 0X200000
#define NB_PTR_ENTRIES 12
#define EHDR_SIZE sizeof(Elf64_Ehdr)

#define MAX(a,b) a > b ? a : b
 
typedef struct s_phdr
{
	Elf64_Phdr 		info;
	void			*data;
	int				offset_is_header_relative;

	struct s_phdr	*next;
} t_phdr;

typedef struct s_shdr
{
	Elf64_Shdr 		info;
	void			*data;

	struct s_shdr	*next;
} t_shdr;

typedef struct s_elf_info
{
	Elf64_Ehdr	ehdr;
	t_phdr		*phdrs;
	t_shdr		*shdrs;

	t_phdr		*current_phdr;
	t_shdr		*current_shdr;

	int	data_offset;
	int	header_offset;
} t_elf_info;


typedef Elf64_Sxword	t_tag;
typedef t_tag			t_ptr_tags[NB_PTR_ENTRIES + 1];
typedef Elf64_Word		t_phdr_type;
typedef	Elf64_Word		t_phdr_flag;
typedef Elf64_Word		t_shdr_type;

int is_entry_a_ptr(Elf64_Dyn entry);
int element_in_array(Elf64_Sxword element, const Elf64_Sxword* array);
void print_ehdr(Elf64_Ehdr ehdr);
t_phdr	*find_phdr(t_phdr *phdrs, t_phdr_type type, t_phdr_flag flags_mask);
t_shdr	*find_section(t_elf_info elf_info, uint8_t* file, unsigned char *name);

#endif