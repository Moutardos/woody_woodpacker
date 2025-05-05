#ifndef UTILS_H
#define UTILS_H

#include <elf.h>

#define NB_PTR_ENTRIES 12
#define MAX(a,b) a > b ? a : b
 
typedef Elf64_Sxword t_tag;
typedef t_tag		 t_ptr_tags[NB_PTR_ENTRIES + 1];

int is_entry_a_ptr(Elf64_Dyn entry);
int element_in_array(Elf64_Sxword element, const Elf64_Sxword* array);
void print_ehdr(Elf64_Ehdr ehdr);
#endif