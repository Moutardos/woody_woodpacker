#include <elf.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "elf_generator.h"
#include "elf_parser.h"
#include "elf_utils.h"

int	setup_file_buffers(t_file_buffers *fb, int fd, Elf64_Ehdr elf_header){

	fb->file_size = elf_header.e_shoff + elf_header.e_shnum * elf_header.e_shentsize;
	fb->file = ft_calloc(fb->file_size, sizeof(uint8_t));

	ft_memmove(fb->file, &elf_header, EHDR_SIZE);
	read(fd, fb->file + EHDR_SIZE, fb->file_size - EHDR_SIZE);
	fb->data_off = elf_header.e_phoff + elf_header.e_phnum * elf_header.e_phentsize;
	fb->data = fb->file + fb->data_off;
	fb->data_size = elf_header.e_shoff - fb->data_off;
	return (0);
}


int main(int argc, char* argv[]){

	if (argc < 2 || argc > 3) {
		printf("Usage: %s <elf_file> [debug]\n", argv[0]);
		return 1;
	}

	int debug = argc == 3;

	/// PARSING ///

	int fd = open(argv[1], O_RDONLY);

	t_elf_info	   elf_info = {0};
	
	read(fd, &elf_info.ehdr, EHDR_SIZE);
	
	if (debug)
		print_ehdr(elf_info.ehdr);
	
	Elf64_Ehdr ehdr = elf_info.ehdr;
	t_file_buffers file_buffers;
	setup_file_buffers(&file_buffers, fd, ehdr);
	
	close(fd);

	for (int i = 0, cursor = EHDR_SIZE; i < elf_info.ehdr.e_phnum;
		 i++, cursor += sizeof(Elf64_Phdr)) {
		t_phdr* new_phdr = allocate_pheader(&elf_info, NULL);
		ft_memmove(&new_phdr->info, file_buffers.file + cursor,
				   sizeof(Elf64_Phdr));
		new_phdr->data = file_buffers.file + new_phdr->info.p_offset;

		if (debug) {
			Elf64_Word type = new_phdr->info.p_type;
			printf("New header, type: ");
			if (type < 16)
				printf("%d\n", type);
			else
				printf("0x%x\n", type);
			if (new_phdr->info.p_type == PT_INTERP)
				printf("Interpreter: %.*s\n", (int)new_phdr->info.p_filesz,
					   (char*)elf_info.current_phdr->data);
		}
	}

	if (debug)
		printf("\n");

	for (int i = 0, cursor = ehdr.e_shoff; i < elf_info.ehdr.e_shnum;
		 i++, cursor += sizeof(Elf64_Shdr)) {
		t_shdr* new_shdr = allocate_sheader(&elf_info, NULL);
		ft_memmove(&new_shdr->info, file_buffers.file + cursor,
				   sizeof(Elf64_Shdr));
		new_shdr->data = file_buffers.file + new_shdr->info.sh_offset;

		if (debug) {
			Elf64_Word type = new_shdr->info.sh_type;
			printf("New section, type: ");
			if (type < 16)
				printf("%d\n", type);
			else
				printf("0x%x\n", type);
			if (i == ehdr.e_shstrndx) {
				printf("shstrtab found, data: ");
				fwrite((char*)new_shdr->data, 1,
					   (int)elf_info.current_shdr->info.sh_size, stdout);
				printf("\n");
			}
		}
	}

	/// INJECT CODE ///

	Elf64_Addr old_entry = ehdr.e_entry;
	elf_info.data_offset =
		(file_buffers.data - file_buffers.file) + file_buffers.data_size;
	size_t bytes = load_code(&elf_info, "../" DFLT_PATH);


	if (old_entry - VMA_BASE >= file_buffers.data_off)
		old_entry += sizeof(Elf64_Phdr);
	if (old_entry - VMA_BASE >= file_buffers.data_off + file_buffers.data_size)
		old_entry += bytes;
	// elf_info.ehdr.e_entry = 0x40c1040;
	elf_info.ehdr.e_entry = old_entry;

	elf_info.ehdr.e_shoff += sizeof(Elf64_Phdr) + bytes;

	int first_load_incremented = 0;
	int nb_load = 0;
	for (t_phdr* cur = elf_info.phdrs; cur; cur = cur->next) {
		if (!first_load_incremented && cur->info.p_type == PT_LOAD) {
			cur->info.p_filesz += sizeof(Elf64_Phdr);
			cur->info.p_memsz += sizeof(Elf64_Phdr);
			first_load_incremented = 1;
		}
		if (cur->info.p_type == PT_LOAD) {
			nb_load++;
			if (nb_load == 4) {
				cur->info.p_filesz += bytes;
				cur->info.p_memsz += bytes;
				continue;
			}
			// if (nb_load == 5)
			// {
			// 	cur->info.p_align = 0x1000;
			// 	cur->info.p_vaddr = 0x503000;
			// 	cur->info.p_paddr = 0x503000;
			// 	cur->info.p_offset = 0x03000;
				

			// }
		}
		if (cur->info.p_type == PT_DYNAMIC) 
		{
			cur->info.p_filesz = 0x190;
			cur->info.p_memsz = 0x190;
			continue;
		}
			if (cur->info.p_vaddr > 0 &&  cur->info.p_vaddr - VMA_BASE >= file_buffers.data_off + file_buffers.data_size) {
			cur->info.p_offset += bytes;
			cur->info.p_vaddr += bytes;
			cur->info.p_paddr += bytes;
		}
	}

	for (t_shdr* cur = elf_info.shdrs; cur; cur = cur->next) {
		if (cur->info.sh_type == SHT_DYNAMIC) {
			Elf64_Dyn entry = {0};
			Elf64_Off cursor = cur->info.sh_offset;
			do {
				ft_memmove(&entry, file_buffers.file + cursor,
						   sizeof(Elf64_Dyn));

				if (is_entry_a_ptr(entry)) {
					Elf64_Addr old_ptr = entry.d_un.d_ptr;
					Elf64_Off  ptr_off = entry.d_un.d_ptr - VMA_BASE;

					// if (ptr_off > 0 && ptr_off < file_buffers.file_size) {
						if (ptr_off >= file_buffers.data_off)
							entry.d_un.d_ptr += sizeof(Elf64_Phdr);
						if (ptr_off >= file_buffers.data_off + file_buffers.data_size)
							entry.d_un.d_ptr += bytes;

						if (entry.d_un.d_ptr != old_ptr)
							ft_memmove(file_buffers.file + cursor, &entry,
									   sizeof(Elf64_Dyn));
					// }
				}

				cursor += sizeof(Elf64_Dyn);
			} while (entry.d_tag != DT_NULL);
		} else if (cur->info.sh_type == SHT_SYMTAB) {
			Elf64_Sym entry = {0};
			int		  nb_entries = cur->info.sh_size / cur->info.sh_entsize;

			for (int i = 0, cursor = cur->info.sh_offset; i < nb_entries;
				 i++, cursor += sizeof(Elf64_Sym)) {
				ft_memmove(&entry, file_buffers.file + cursor,
						   sizeof(Elf64_Sym));

				if (entry.st_shndx != SHN_UNDEF &&
					entry.st_shndx < SHN_LORESERVE) {
					Elf64_Addr old_ptr = entry.st_value;
					Elf64_Off  ptr_off = entry.st_value - VMA_BASE;

					// if (ptr_off > 0 && ptr_off < file_buffers.file_size) {
						if (ptr_off >= file_buffers.data_off)
							entry.st_value += sizeof(Elf64_Phdr);
						if (entry.st_shndx != 22)
							if (ptr_off >= file_buffers.data_off + file_buffers.data_size)
								entry.st_value += bytes;

						if (entry.st_value != old_ptr)
							ft_memmove(file_buffers.file + cursor, &entry,
									   sizeof(Elf64_Sym));
					// }
				}
			}
		} else if (cur->info.sh_type == SHT_RELA)
		{
			Elf64_Rela entry = {0};
			int		  nb_entries = cur->info.sh_size / cur->info.sh_entsize;

			for (int i = 0, cursor = cur->info.sh_offset; i < nb_entries;
				 i++, cursor += sizeof(Elf64_Rela)) {
					ft_memmove(&entry, file_buffers.file + cursor,
						sizeof(Elf64_Rela));
					Elf64_Addr	old_ptr = entry.r_offset;
					Elf64_Off	ptr_off = entry.r_offset - VMA_BASE;

					// if (ptr_off > 0 && ptr_off < file_buffers.file_size) {
						if (ptr_off >= file_buffers.data_off)
							entry.r_offset += sizeof(Elf64_Phdr);
						if (ptr_off >= file_buffers.data_off + file_buffers.data_size)
							entry.r_offset += bytes;

						if (entry.r_offset != old_ptr)
							ft_memmove(file_buffers.file + cursor, &entry,
										sizeof(Elf64_Rela));			  
					// }
			}
		} 
		// else if (cur->info.sh_type == SHT_GNU_verneed) {
		// 	Elf64_Verneed entry = {0};
		// 	Elf64_Vernaux entry_aux = {0};
		// 	int		  nb_entries = 1;// cur->info.sh_size / cur->info.sh_entsize;

		// 	for (int i = 0, cursor = cur->info.sh_offset; i < nb_entries;
		// 		i++, cursor += sizeof(Elf64_Verneed)) {
		// 			ft_memmove(&entry, file_buffers.file + cursor,
		// 				sizeof(Elf64_Verneed));
		// 			// Elf64_Addr	old_ptr = entry.vn_aux;
		// 			// Elf64_Off	ptr_off = entry.vn_aux;

		// 			ft_memmove(&entry_aux, file_buffers.file + cursor + entry.vn_aux,
		// 				sizeof(Elf64_Vernaux));

		// 			while (entry_aux.vna_next) {
		// 				ft_memmove(&entry_aux, file_buffers.file + cursor + entry.vn_aux + entry_aux.vna_next,
		// 					sizeof(Elf64_Vernaux));
		// 			}

		// 			// if (ptr_off > 0 && ptr_off < file_buffers.file_size) {
		// 				// if (ptr_off >= file_buffers.data_off)
		// 				// 	entry.vn_aux += sizeof(Elf64_Phdr);
		// 				// if (ptr_off >= file_buffers.data_off + file_buffers.data_size)
		// 				// 	entry.vn_aux += bytes;

		// 				// if (entry.vn_aux != old_ptr)
		// 				// 	ft_memmove(file_buffers.file + cursor, &entry,
		// 				// 				sizeof(Elf64_Verneed));			  
		// 			// }
		// 	}
		// }

		Elf64_Off addr_off = cur->info.sh_addr > VMA_BASE ? cur->info.sh_addr - VMA_BASE : 0;
		Elf64_Off off = cur->info.sh_offset; 
		if (cur->info.sh_offset > 0 /**&& cur->info.sh_offset < file_buffers.file_size
			&& addr_off > 0 && addr_off < file_buffers.file_size**/) {
			if (off >= file_buffers.data_off || addr_off >= file_buffers.data_off) {
				cur->info.sh_offset += sizeof(Elf64_Phdr);

				if (cur->info.sh_addr > VMA_BASE)
					cur->info.sh_addr += sizeof(Elf64_Phdr);
			}
			if (cur->info.sh_type == SHT_DYNAMIC) continue;
			if (off >= file_buffers.data_off + file_buffers.data_size
				|| addr_off >= file_buffers.data_off + file_buffers.data_size) {
				cur->info.sh_offset += bytes;

				if (cur->info.sh_addr > VMA_BASE)
					cur->info.sh_addr += bytes;
			}
		}
	}

	/// OUTPUT ELF ///

	fd = open("parser_result.elf", O_WRONLY | O_TRUNC | O_CREAT, 0777);
	if (fd < 0)
	{
		printf("Error creating file\n");
		exit(1);
	}
	write(fd, &elf_info.ehdr, sizeof(elf_info.ehdr));

	for (t_phdr* cur = elf_info.phdrs; cur; cur = cur->next)
		write(fd, &cur->info, sizeof(cur->info));

	write(fd, file_buffers.data, file_buffers.data_size);

	write(fd, elf_info.current_phdr->data,
		  elf_info.current_phdr->info.p_filesz);

	for (t_shdr* cur = elf_info.shdrs; cur; cur = cur->next)
		write(fd, &cur->info, sizeof(cur->info));

	close(fd);

	return 0;
}