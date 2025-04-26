#include <elf.h>
#include "libft.h"

#define MACHINE EM_X86_64
#define VMA_BASE 0x400000

#define INTERPRETER "/lib64/ld-linux-x86-64.so.2"

void initialize_elf(Elf64_Ehdr *elf_hdr)
{
	ft_memmove(elf_hdr->e_ident, ELFMAG, 4);
	elf_hdr->e_ident[EI_CLASS] = ELFCLASS64;
	elf_hdr->e_ident[EI_DATA] = ELFDATA2LSB; // set endian
	elf_hdr->e_ident[EI_VERSION] = EV_CURRENT;
	elf_hdr->e_ident[EI_OSABI] = ELFOSABI_SYSV;
	elf_hdr->e_type = ET_EXEC;
	elf_hdr->e_machine = MACHINE;
	elf_hdr->e_version = EV_CURRENT;
	elf_hdr->e_entry= 0; //to change
	elf_hdr->e_phoff = sizeof(Elf64_Ehdr);
	elf_hdr->e_shoff = 0; //to change
	elf_hdr->e_flags = 0x0; // change?
	elf_hdr->e_ehsize = sizeof(Elf64_Ehdr);
	elf_hdr->e_phentsize = sizeof(Elf64_Phdr);; // change, sum(header_size)
	elf_hdr->e_phnum = 3;//0xf; // default c program headers
	elf_hdr->e_shentsize = 0; // change
	elf_hdr->e_shnum = 0; // change
	elf_hdr->e_shstrndx = SHN_UNDEF; //change
}

void initialize_phdr(Elf64_Phdr *p_hdr) {
	p_hdr->p_type = PT_PHDR;
	p_hdr->p_flags = PF_R;
	p_hdr->p_offset = sizeof(Elf64_Ehdr);
	p_hdr->p_filesz = 3 * sizeof(Elf64_Phdr);
	p_hdr->p_memsz = p_hdr->p_filesz;
	p_hdr->p_align = 1;
	p_hdr->p_vaddr = VMA_BASE + p_hdr->p_offset;
	p_hdr->p_paddr = p_hdr->p_vaddr;
}

void initialize_interp_hdr(Elf64_Phdr *interp_hdr) {
	interp_hdr->p_type = PT_INTERP;
	interp_hdr->p_flags = PF_R;
	interp_hdr->p_offset = sizeof(Elf64_Ehdr) + 3 * sizeof(Elf64_Phdr);
	interp_hdr->p_filesz = sizeof(INTERPRETER);
	interp_hdr->p_memsz = interp_hdr->p_filesz;
	interp_hdr->p_align = 1;
	interp_hdr->p_vaddr = VMA_BASE + interp_hdr->p_offset;
	interp_hdr->p_paddr = interp_hdr->p_vaddr;
}

void initialize_load_hdr(Elf64_Phdr *load_hdr) {
	load_hdr->p_type = PT_LOAD;
	load_hdr->p_flags = PF_R;
	load_hdr->p_offset = 0;
	load_hdr->p_filesz = sizeof(Elf64_Ehdr) + 3 * sizeof(Elf64_Phdr) + sizeof(INTERPRETER);
	load_hdr->p_memsz = load_hdr->p_filesz;
	load_hdr->p_align = 1;
	load_hdr->p_vaddr = VMA_BASE + load_hdr->p_offset;
	load_hdr->p_paddr = load_hdr->p_vaddr;
}


int main(int argc, char const *argv[])
{
	Elf64_Ehdr elf_hdr = {0};
	Elf64_Phdr p_hdr = {0};
	Elf64_Phdr interp_hdr = {0};
	Elf64_Phdr load_hdr = {0};

	initialize_elf(&elf_hdr);
	initialize_phdr(&p_hdr);
	initialize_interp_hdr(&interp_hdr);
	initialize_load_hdr(&load_hdr);
	write(1, &elf_hdr, sizeof(elf_hdr));
	write(1, &p_hdr, sizeof(p_hdr));
	write(1, &interp_hdr, sizeof(interp_hdr));
	write(1, &load_hdr, sizeof(load_hdr));
	write(1, INTERPRETER, sizeof(INTERPRETER));
	return 0;
}

