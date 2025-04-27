#include <elf.h>
#include "libft.h"
#include <fcntl.h>

#define MACHINE EM_X86_64
#define VMA_BASE 0x400000

#define INTERPRETER "/lib64/ld-linux-x86-64.so.2"
#define NB_HEADER 3

typedef struct s_phdr
{
	Elf64_Phdr 		info;
	void			*data;
	int				offset_is_header_relative;

	struct s_phdr	*next;
} t_phdr;

typedef struct s_elf_info
{
	Elf64_Ehdr	ehdr;
	t_phdr		*phdrs;

	t_phdr		*current;
	int	nb_headers;
	int	data_offset;
	int	header_offset;
} t_elf_info;

void initialize_elf(Elf64_Ehdr *elf_hdr)
{
	ft_memmove(elf_hdr->e_ident, ELFMAG, 4);
	elf_hdr->e_ident[EI_CLASS] = ELFCLASS64;
	elf_hdr->e_ident[EI_DATA] = ELFDATA2LSB;
	elf_hdr->e_ident[EI_VERSION] = EV_CURRENT;
	elf_hdr->e_ident[EI_OSABI] = ELFOSABI_SYSV;
	elf_hdr->e_type = ET_EXEC;
	elf_hdr->e_machine = MACHINE;
	elf_hdr->e_version = EV_CURRENT;
	elf_hdr->e_entry= VMA_BASE; //to change
	elf_hdr->e_phoff = sizeof(Elf64_Ehdr);
	elf_hdr->e_shoff = 0;
	elf_hdr->e_flags = 0x0;
	elf_hdr->e_ehsize = sizeof(Elf64_Ehdr);
	elf_hdr->e_phentsize = sizeof(Elf64_Phdr);; // change, sum(header_size)
	elf_hdr->e_phnum = 0;//0xf; // default c program headers
	elf_hdr->e_shentsize = 0; // change
	elf_hdr->e_shnum = 0; // change
	elf_hdr->e_shstrndx = SHN_UNDEF; //change
}



int create_header(t_elf_info *elf_info, int type, void *data, int data_size, int flag)
{
	t_phdr	*new_phdr = calloc(1, sizeof(t_phdr));
	if (!new_phdr)
	return (1);
	
	new_phdr->next = NULL;
	new_phdr->data = data;
	new_phdr->offset_is_header_relative = 0;
	
	if (!elf_info->current) {
		elf_info->phdrs = new_phdr;
		elf_info->phdrs->offset_is_header_relative = 1;
	} else
		elf_info->current->next = new_phdr;

		size_t	phdr_size = sizeof(new_phdr->info);

	new_phdr->info.p_type = type;
	new_phdr->info.p_flags = flag;
	new_phdr->info.p_filesz = data_size;
	new_phdr->info.p_memsz = data_size;
	new_phdr->info.p_align = 1;

	if (type == PT_PHDR || type == PT_INTERP)
		new_phdr->info.p_offset = elf_info->header_offset;
	if (type == PT_LOAD) 
		new_phdr->info.p_offset = elf_info->data_offset;

	new_phdr->info.p_vaddr = VMA_BASE + new_phdr->info.p_offset;
	new_phdr->info.p_paddr = new_phdr->info.p_vaddr;
	// update PHDR
	elf_info->phdrs->info.p_filesz += phdr_size;
	elf_info->phdrs->info.p_memsz += phdr_size;
	t_phdr *node = elf_info->phdrs;
	while (node != NULL)
	{
		if (node->info.p_type == PT_INTERP)
		{
			node->info.p_offset += phdr_size;
			node->info.p_vaddr = VMA_BASE + node->info.p_offset;
			node->info.p_paddr = node->info.p_vaddr;
		}
		node = node->next;
	}
	if (type == PT_LOAD)
		elf_info->data_offset += data_size;

	elf_info->nb_headers++;
	elf_info->ehdr.e_phnum = elf_info->nb_headers;
	elf_info->current = new_phdr;
	elf_info->header_offset += phdr_size;
	return (0);
}

int initialize_elf_info(t_elf_info *elf_info)
{
	initialize_elf(&elf_info->ehdr);
	elf_info->header_offset += sizeof(elf_info->ehdr);
	if (elf_info->phdrs)
		return 1;
	
	elf_info->nb_headers = 0;
	// elf_info->current_offset = sizeof(Elf64_Ehdr);
	// elf_info->header_load_initialized = 0;
	elf_info->data_offset = 0;
	
	create_header(elf_info, PT_PHDR, NULL, 0, PF_R);
	return (0);
	// initialize_phdr(&elf_info->phdrs);
}

void free_phdrs(t_phdr *phdrs)
{
	while (phdrs)
	{
		t_phdr *node = phdrs;
		phdrs = phdrs->next;
		free(node);
	}
}

// void initialize_phdr(Elf64_Phdr *p_hdr) {
	// 	p_hdr->p_type = PT_PHDR;
	// 	p_hdr->p_flags = PF_R;
	// 	p_hdr->p_offset = sizeof(Elf64_Ehdr);
	// 	p_hdr->p_filesz = NB_HEADER * sizeof(Elf64_Phdr);
// 	p_hdr->p_memsz = p_hdr->p_filesz;
// 	p_hdr->p_align = 1;
// 	p_hdr->p_vaddr = VMA_BASE + p_hdr->p_offset;
// 	p_hdr->p_paddr = p_hdr->p_vaddr;
// }

// void initialize_interp_hdr(Elf64_Phdr *interp_hdr) {
// 	interp_hdr->p_type = PT_INTERP;
// 	interp_hdr->p_flags = PF_R;
// 	interp_hdr->p_offset = sizeof(Elf64_Ehdr) + NB_HEADER * sizeof(Elf64_Phdr);
// 	interp_hdr->p_filesz = sizeof(INTERPRETER);
// 	interp_hdr->p_memsz = interp_hdr->p_filesz;
// 	interp_hdr->p_align = 1;
// 	interp_hdr->p_vaddr = VMA_BASE + interp_hdr->p_offset;
// 	interp_hdr->p_paddr = interp_hdr->p_vaddr;
// }

// void initialize_load_hdr(Elf64_Phdr *load_hdr) {
// 	load_hdr->p_type = PT_LOAD;
// 	load_hdr->p_flags = PF_R;
// 	load_hdr->p_offset = 0;
// 	load_hdr->p_filesz = sizeof(Elf64_Ehdr) + NB_HEADER * sizeof(Elf64_Phdr) + sizeof(INTERPRETER);
// 	load_hdr->p_memsz = load_hdr->p_filesz;
// 	load_hdr->p_align = 1;
// 	load_hdr->p_vaddr = VMA_BASE + load_hdr->p_offset;
// 	load_hdr->p_paddr = load_hdr->p_vaddr;
// }

// void initialize_bin_load_hdr(Elf64_Phdr *load_hdr) {
// 	load_hdr->p_type = PT_LOAD;
// 	load_hdr->p_flags = PF_R | PF_X;
// 	load_hdr->p_offset = sizeof(Elf64_Ehdr) + NB_HEADER * sizeof(Elf64_Phdr) /**+ sizeof(INTERPRETER)**/;
// 	load_hdr->p_filesz = 48;
// 	load_hdr->p_memsz = load_hdr->p_filesz;
// 	load_hdr->p_align = 1;
// 	load_hdr->p_vaddr = VMA_BASE + load_hdr->p_offset;
// 	load_hdr->p_paddr = load_hdr->p_vaddr;
// }


void load_code(t_elf_info *elf_info, char *file_name)
{
	
	int fd = open(file_name, O_RDONLY);
	char buff[0x1000];
	size_t bytes;
	bytes = read(fd, buff, sizeof(buff));
	create_header(elf_info, PT_LOAD, buff, bytes, PF_R | PF_X);
	elf_info->ehdr.e_entry = elf_info->current->info.p_vaddr;
}


// void add_phdr()
#include <stdio.h>
int main(int argc, char const *argv[])
{
	// Elf64_Ehdr elf_hdr = {0};
	// Elf64_Phdr p_hdr = {0};
	// Elf64_Phdr interp_hdr = {0};
	// Elf64_Phdr load_hdr = {0};
	// Elf64_Phdr bin_load_hdr = {0};

	t_elf_info elf_info;
	char buffer[0x5000] = {0};
	initialize_elf_info(&elf_info);
	// create_header(&elf_info, PT_INTERP, INTERPRETER, sizeof(INTERPRETER), PF_R);
	create_header(&elf_info, PT_LOAD, NULL, 0x1000, PF_R);

	load_code(&elf_info, "code.bin");

	size_t bytes;
	bytes = sizeof(elf_info.ehdr);
	ft_memcpy(buffer, &elf_info.ehdr, sizeof(elf_info.ehdr));
	// sprintf(buffer, "%&elf_info.ehdr, sizeof(elf_info.ehdr));
	t_phdr *node = elf_info.phdrs;
	while (node != NULL)
	{
		ft_memcpy(buffer + bytes, &node->info, sizeof(node->info) );
		bytes += sizeof(node->info);
		node = node->next;
	}
	// ft_memcpy(buffer + bytes, INTERPRETER, sizeof(INTERPRETER));
	ft_memcpy(buffer + 0x1000, elf_info.current->data, elf_info.current->info.p_filesz);
	ft_memcpy(buffer + 0x1000 * 2, elf_info.current->data, elf_info.current->info.p_filesz);

	// initialize_phdr(&p_hdr);
	// initialize_interp_hdr(&interp_hdr);
	// initialize_load_hdr(&load_hdr);
	// initialize_bin_load_hdr(&bin_load_hdr);
	// elf_hdr.e_entry += bin_load_hdr.p_offset;
	// write(1, &elf_hdr, sizeof(elf_hdr));
	// write(1, &p_hdr, sizeof(p_hdr));
	// // write(1, &interp_hdr, sizeof(interp_hdr));
	// write(1, &load_hdr, sizeof(load_hdr));
	// write(1, &bin_load_hdr, sizeof(bin_load_hdr));
	// // write(1, INTERPRETER, sizeof(INTERPRETER));
	// manage_binary("code.bin");
	int fd = open("result.elf", O_RDWR | O_CREAT);
	write(fd, buffer, 0x1000 + elf_info.current->info.p_filesz);
	close(fd);
	return 0;
}

