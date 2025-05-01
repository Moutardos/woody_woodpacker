#ifndef ELF_PARSER_H
#define ELF_PARSER_H

#include <stdint.h>

typedef struct s_buffers {
	uint8_t* file;
	uint8_t* data;

	unsigned long file_size;
	unsigned long data_size;
} t_file_buffers;

#endif	// ELF_PARSER_H
