#include <string.h>

#include "wav.h"

void write_riff_header(FILE* f, RIFF_HEADER* header, const uint8_t pass)
{
	if(pass == 0) // Before anything is written to the output file
	{
		const uint32_t fmt_size = 16; // "fmt " chunk size
		
		fprintf(f, "RIFF0000WAVEfmt ");
		fwrite(&fmt_size, sizeof(uint32_t), 1, f);
		fwrite(&header->audio_format, sizeof(uint16_t), 1, f);
		fwrite(&header->channels, sizeof(uint16_t), 1, f);
		fwrite(&header->sample_rate, sizeof(uint32_t), 1, f);
		fwrite(&header->byte_rate, sizeof(uint32_t), 1, f);
		fwrite(&header->block_align, sizeof(uint16_t), 1, f);
		fwrite(&header->bits_per_sample, sizeof(uint16_t), 1, f);
		fprintf(f, "data0000");
	}
	else if(pass == 1) // Writing sizes for chunks in the header (after data has been written)
	{
		fseek(f, 0, SEEK_END);
		const uint32_t size = ftell(f);
		const uint32_t size_file = size-4;
		const uint32_t size_data = size-44;
		
		fseek(f, 4, SEEK_SET);
		fwrite(&size_file, sizeof(uint32_t), 1, f);
		
		fseek(f, 40, SEEK_SET);
		fwrite(&size_data, sizeof(uint32_t), 1, f);
	}
}

void read_riff_header(FILE* f, RIFF_HEADER* header)
{	
	fseek(f, 4, SEEK_CUR); // 'RIFF'
	fseek(f, 4, SEEK_CUR); // Size of the WAV file
	fseek(f, 4, SEEK_CUR); // 'WAVE'
	
	while(1)
	{
		char chunk_name[5];
		uint32_t chunk_size = 0;
		uint16_t extra_param_size = 0;
		
		fread(chunk_name, sizeof(uint8_t), 4, f);
		chunk_name[4] = 0;
		fread(&chunk_size, sizeof(uint32_t), 1, f);
		
		if(strcmp(chunk_name, "fmt ") == 0)
		{
			fread(&header->audio_format, sizeof(uint16_t), 1, f);
			fread(&header->channels, sizeof(uint16_t), 1, f);
			fread(&header->sample_rate, sizeof(uint32_t), 1, f);
			fread(&header->byte_rate, sizeof(uint32_t), 1, f);
			fread(&header->block_align, sizeof(uint16_t), 1, f);
			fread(&header->bits_per_sample, sizeof(uint16_t), 1, f);

			// Reading extra params for ADPCM
			// We only want how many samples per block there is.
			if(header->audio_format == 0x11)
			{
				fread(&extra_param_size, sizeof(uint16_t), 1, f);
				fread(&header->samples_block, sizeof(uint16_t), 1, f);
			}
		}
		else if(strcmp(chunk_name, "data") == 0)
		{
			header->data_size = chunk_size;
			// Data chunk is on the end, so we can safely exit the loop.
			break;
		}
		else
		{
			printf("[!] Unknown chunk \"%s\" at 0x%x. Skipping %u bytes.\n", chunk_name, ftell(f), chunk_size);
			fseek(f, chunk_size, SEEK_CUR);
		}
	}
	
	// Calculating the number of samples
	
	uint32_t full_blocks = 0;
	uint32_t last_block = 0;
	uint32_t nibbles_block = header->block_align * 2;
	
	full_blocks = header->data_size / nibbles_block;
	header->num_samples = full_blocks * header->samples_block;
	last_block = (header->data_size - (full_blocks * nibbles_block)) - (nibbles_block - header->samples_block);
	header->num_samples += last_block;
	header->num_samples *= 2;
}
