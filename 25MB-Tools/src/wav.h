#pragma once

#include <stdio.h>
#include <stdint.h>

typedef struct RIFF
{
	uint16_t audio_format;
	uint16_t channels;
	uint32_t sample_rate;
	uint32_t byte_rate;
	uint16_t block_align;
	uint16_t bits_per_sample;
	uint16_t samples_block;
	uint32_t data_size;
	uint32_t num_samples;
	
} RIFF_HEADER;


void read_riff_header(FILE* f, RIFF_HEADER* header);
void write_riff_header(FILE* f, RIFF_HEADER* header, uint8_t pass);