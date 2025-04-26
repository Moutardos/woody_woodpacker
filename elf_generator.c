#include <elf.h>
#include "libft.h"

#define MACHINE EM_X86_64
#define VMA_BASE 0x400000

void initialize_elf(Elf64_Ehdr *elf_hdr)
{
	ft_memmove(elf_hdr->e_ident, ELFMAG, 4);
	elf_hdr->e_ident[EI_CLASS] = ELFCLASS64;
	elf_hdr->e_ident[EI_DATA] = ELFDATA2LSB; // set endian
	elf_hdr->e_ident[EI_VERSION] = EV_CURRENT;
	elf_hdr->e_ident[EI_OSABI] = ELFOSABI_LINUX;
	elf_hdr->e_type = ET_EXEC;
	elf_hdr->e_machine = MACHINE;
	elf_hdr->e_version = EV_CURRENT;
	elf_hdr->e_entry= 0; //to change
	elf_hdr->e_phoff = sizeof(Elf64_Ehdr);
	elf_hdr->e_shoff = 0; //to change
	elf_hdr->e_flags = 0x0; // change?
	elf_hdr->e_ehsize = sizeof(Elf64_Ehdr);
	elf_hdr->e_phentsize = sizeof(Elf64_Phdr);; // change, sum(header_size)
	elf_hdr->e_phnum = 1;//0xf; // default c program headers
	elf_hdr->e_shentsize = 0; // change
	elf_hdr->e_shnum = 0; // change
	elf_hdr->e_shstrndx = SHN_UNDEF; //change
}

void initialize_phdr(Elf64_Phdr *p_hdr) {
	p_hdr->p_type = PT_PHDR;
	p_hdr->p_flags = PF_R | PF_X;
	p_hdr->p_offset = sizeof(Elf64_Ehdr);
	p_hdr->p_filesz = sizeof(Elf64_Phdr) * 1;
	p_hdr->p_memsz = p_hdr->p_filesz;
	p_hdr->p_align = 1;
	p_hdr->p_vaddr = VMA_BASE + p_hdr->p_offset;
	p_hdr->p_paddr = p_hdr->p_vaddr;
}


int main(int argc, char const *argv[])
{
	Elf64_Ehdr elf_hdr = {0};
	Elf64_Phdr p_hdr = {0};

	initialize_elf(&elf_hdr);
	initialize_phdr(&p_hdr);
	write(1, &elf_hdr, sizeof(elf_hdr));
	write(1, &p_hdr, sizeof(p_hdr));
	return 0;
}

