all:
	clang -o elf_generator elf_generator.c -Ilibft -Llibft -lft