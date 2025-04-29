#include <stdio.h>
#include <elf.h>
#include <fcntl.h>
#include <unistd.h>

#include "elf_generator.h"

void print_ehdr(Elf64_Ehdr ehdr) {
	printf("e_ident: %s\n", ehdr.e_ident);
	printf("e_ident hex:\n");
	for (int i = 0; i < EI_NIDENT; i++)
		printf("%02x ", ehdr.e_ident[i]);
	printf("\n");
	printf("phnum: %d\n", ehdr.e_phnum);
	printf("shnum: %d\n", ehdr.e_shnum);
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage: %s <elf_file>\n", argv[0]);
		return 1;
	}

	int fd = open(argv[1], O_RDONLY);

	t_elf_info elf_info = {0};

	read(fd, &elf_info.ehdr, sizeof(elf_info.ehdr));

	for (int i = 0; i < elf_info.ehdr.e_phnum; i++) {
		Elf64_Phdr new_phdr = (allocate_pheader(&elf_info, NULL))->info;
		read(fd, &new_phdr, sizeof(new_phdr));
	
		printf("%x %lx\n", new_phdr.p_type, new_phdr.p_filesz);
	}

	return 0;
}