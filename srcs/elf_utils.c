#include "elf_utils.h"
#include "libft.h"

#include <stdio.h>
#include <fcntl.h>

int element_in_array(Elf64_Sxword element, const Elf64_Sxword* array) {

	for (const Elf64_Sxword* tag = array; *tag != DT_NULL; tag++)
		if (element == *tag)
			return (1);

	return (0);
}

int is_entry_a_ptr(Elf64_Dyn entry) {
	const static t_ptr_tags ptr_tags = {
		DT_PLTGOT,	   DT_HASH,		  DT_STRTAB, DT_SYMTAB, DT_RELA,
		DT_INIT,	   DT_FINI,		  DT_REL,	 DT_DEBUG,	DT_JMPREL,
		DT_INIT_ARRAY, DT_FINI_ARRAY, DT_NULL};

	if (entry.d_tag >= DT_ADDRRNGLO && entry.d_tag <= DT_ADDRRNGHI)
		return 1;

	if (entry.d_tag >= DT_ENCODING)
		return entry.d_tag % 2 == 0;

	return (element_in_array(entry.d_tag, ptr_tags));
}

void print_ehdr(Elf64_Ehdr ehdr) {
	printf("e_ident: %s\n", ehdr.e_ident);
	printf("e_ident hex:\n");
	for (int i = 0; i < EI_NIDENT; i++)
		printf("%02x ", ehdr.e_ident[i]);
	printf("\n");
	printf("phnum: %d\n", ehdr.e_phnum);
	printf("shnum: %d\n\n", ehdr.e_shnum);
}

t_phdr	*find_phdr(t_phdr *phdrs, t_phdr_type type, t_phdr_flag flags_mask)
{
	while (phdrs != NULL)
	{
		if (phdrs->info.p_type == type)
		{
			if ((phdrs->info.p_flags & flags_mask) == flags_mask)
				return (phdrs);
		}
		phdrs = phdrs->next;
	}
	return (NULL);
}

t_shdr	*find_section(t_elf_info elf_info, uint8_t* file, char *name)
{
	t_shdr* shstrtab = elf_info.shdrs;
	for (int i = 0; i != elf_info.ehdr.e_shstrndx && shstrtab; i++, shstrtab = shstrtab->next);

	if (shstrtab == NULL) return NULL;

	char *names = (char*)file + shstrtab->info.sh_offset;
	char *shdr_name = NULL;

	for (t_shdr* cur = elf_info.shdrs; cur; cur = cur->next) {
		shdr_name = names + cur->info.sh_name;
		if (ft_strcmp(name, shdr_name) == 0)
			return cur;
	}
	return (NULL);

}

size_t load_code_buffer(char *file_name, char* buffer, int size) {
	int fd = open(file_name, O_RDONLY);
	if (fd == -1) {
		printf("Can't open code file\n");
		exit(1);
	}
	size_t bytes = read(fd, buffer, size);

	close(fd);

	return bytes;
}