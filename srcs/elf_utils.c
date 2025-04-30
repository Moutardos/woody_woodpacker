#include "elf_utils.h"

int element_in_array(Elf64_Sxword element, const Elf64_Sxword *array) {

	for (const Elf64_Sxword *tag = array; tag != DT_NULL; tag++)
	{
		if (element == *tag)
			return (1);
	}
	return (0);
}

int is_entries_a_ptr(Elf64_Dyn entry)
{
	const static t_ptr_tags ptr_tags = {DT_PLTGOT, DT_HASH, DT_STRTAB, DT_SYMTAB,
		 DT_RELA, DT_INIT, DT_FINI,
		  DT_REL, DT_DEBUG, DT_JMPREL, DT_INIT_ARRAY, DT_FINI_ARRAY,
		   DT_PREINIT_ARRAY};
	
	return (element_in_array(entry.d_tag, ptr_tags));
}