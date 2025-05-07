#ifndef ELF_G_H
# define ELF_G_H

# include <elf.h>
# include "elf_utils.h"
# include "libft.h"
# include <fcntl.h>

# define MACHINE EM_X86_64
# define VMA_BASE 0x400000

# define INTERPRETER "/lib64/ld-linux-x86-64.so.2"
# define DFLT_PATH "misc/code.bin"

int		initialize_elf_info(t_elf_info *elf_info);
void	initialize_elf(Elf64_Ehdr *elf_hdr);

t_phdr *allocate_pheader(t_elf_info *elf_info, void *data);
t_shdr *allocate_sheader(t_elf_info *elf_info, void *data);
void 	free_phdrs(t_phdr *phdrs);

int 	add_pheader(t_elf_info *elf_info, int type, void *data, int data_size, int flag);
void 	update_phinfo(t_elf_info *elf_info, t_phdr *new_phdr, int data_size);

size_t	load_code(t_elf_info *elf_info, char *file_name);

#endif