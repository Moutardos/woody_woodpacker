#include "elf_generator.h"

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




int initialize_elf_info(t_elf_info *elf_info)
{
	elf_info->current_phdr = NULL;
	elf_info->current_shdr = NULL;

	initialize_elf(&elf_info->ehdr);
	elf_info->header_offset = 0;
	elf_info->header_offset += sizeof(elf_info->ehdr);
	
	elf_info->data_offset = 0;

	add_pheader(elf_info, PT_PHDR, NULL, 0, PF_R);
	add_pheader(elf_info, PT_LOAD, NULL, 0x1000, PF_R);
	return (0);
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



t_phdr *allocate_pheader(t_elf_info *elf_info, void *data)
{
	t_phdr	*new_phdr = calloc(1, sizeof(t_phdr));
	if (!new_phdr)
	return (NULL);
	
	new_phdr->next = NULL;
	new_phdr->data = data;
	new_phdr->offset_is_header_relative = 0;
	
	if (!elf_info->current_phdr) {
		elf_info->phdrs = new_phdr;
		elf_info->phdrs->offset_is_header_relative = 1;
	} 
	else
		elf_info->current_phdr->next = new_phdr;

	elf_info->current_phdr = new_phdr;
	
	return (new_phdr);
}

t_shdr *allocate_sheader(t_elf_info *elf_info, void *data)
{
	t_shdr	*new_shdr = calloc(1, sizeof(t_shdr));
	if (!new_shdr)
	return (NULL);
	
	new_shdr->next = NULL;
	new_shdr->data = data;
	
	if (!elf_info->current_shdr)
		elf_info->shdrs = new_shdr; 
	else
		elf_info->current_shdr->next = new_shdr;

	elf_info->current_shdr = new_shdr;
	
	return (new_shdr);
}

void update_phinfo(t_elf_info *elf_info, t_phdr *new_phdr, int data_size)
{
	size_t	phdr_size = sizeof(new_phdr->info);
	
	// update PHDR
	elf_info->phdrs->info.p_filesz += phdr_size;
	elf_info->phdrs->info.p_memsz += phdr_size;
	
	// update interpreter offset
	t_phdr *node = elf_info->phdrs;
	while (node != NULL)
	{
		// if (node->info.p_type == PT_INTERP)
		if (node->info.p_type != PT_PHDR && node->info.p_offset != 0)
		{
			node->info.p_offset += phdr_size;
			node->info.p_vaddr += phdr_size;
			node->info.p_paddr += phdr_size;
		}
		node = node->next;
	}
	
	// update elf info
	if (new_phdr->info.p_type == PT_LOAD)
		elf_info->data_offset += data_size;
	
	elf_info->ehdr.e_phnum++;
	elf_info->header_offset += phdr_size;
}

int add_pheader(t_elf_info *elf_info, int type, void *data, int data_size, int flag)
{
	
	t_phdr *new_phdr = allocate_pheader(elf_info, data);
	if (!new_phdr)
	return (1);
	
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
	
	update_phinfo(elf_info, new_phdr, data_size);
	return (0);
}

size_t load_code(t_elf_info *elf_info, char *file_name)
{
	
	int fd = open(file_name, O_RDONLY);
	char buff[0x1000];
	size_t bytes;
	bytes = read(fd, buff, sizeof(buff));
	add_pheader(elf_info, PT_LOAD, buff, bytes, PF_R | PF_X);
	elf_info->ehdr.e_entry = elf_info->current_phdr->info.p_vaddr;

	return bytes;
}

