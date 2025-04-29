#include "elf_generator.h"

// mm
#include <stdio.h>
int main(int argc, char const *argv[])
{
	char *code_path = DFLT_PATH;
	t_elf_info elf_info;
	char buffer[0x5000] = {0};

	// ft_printf("Initializing ")
	initialize_elf_info(&elf_info);
	if (argc == 2)
		code_path = (char*)argv[1];

	load_code(&elf_info, code_path);

	size_t bytes;
	bytes = sizeof(elf_info.ehdr);
	ft_memmove(buffer, &elf_info.ehdr, sizeof(elf_info.ehdr));
	t_phdr *node = elf_info.phdrs;
	while (node != NULL)
	{
		ft_memmove(buffer + bytes, &node->info, sizeof(node->info) );
		bytes += sizeof(node->info);
		node = node->next;
	}
	ft_memmove(buffer + 0x1038, elf_info.current_phdr->data, elf_info.current_phdr->info.p_filesz);
	// ft_memmove(buffer + 0x1030, elf_info.current_phdr->data, elf_info.current_phdr->info.p_filesz);

	int fd = open("result.elf", O_RDWR | O_CREAT);
	write(fd, buffer, 0x1038 + elf_info.current_phdr->info.p_filesz * 2);
	close(fd);
	return 0;
}

