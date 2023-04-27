#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "wav.h"
#include "pathutils.h"

int16_t clamp_s16(int32_t value)
{
	if(value > SHRT_MAX)
		return SHRT_MAX;
	else if(value < SHRT_MIN)
		return SHRT_MIN;
	
	return value;
}

void write_info()
{
	printf("===STEREO TO MONO WITH PHASE====\n");
	printf("By Kwasior/@ThisKwasior\n");
	printf("\n====Usage\n");
	printf("\n\tstereo2monophase.exe path_to_file (double)sum_gain (double)diff_gain (whatever)write_streams\n");
	printf("\n");
}

int main(int argc, char** argv)
{
	double sum_gain = 0.5f;
	double diff_gain = 0.5f;
	char write_streams = 0;
	
	if(argc <= 1)
	{
		write_info();
		return 0;
	}
	
	if(argc > 2)
		sum_gain = atof(argv[2]);

	if(argc > 3)
		diff_gain = atof(argv[3]);

	if(argc > 4)
		write_streams = 1;
	
	FILE_PATH audio_path;
	
	split_path(argv[1], strlen(argv[1]), &audio_path);
	
	printf("[#] %s\n", audio_path.absolute_dir.ptr);
	printf("[#] %s\n", audio_path.file_name.ptr);
	printf("[#] %s\n", audio_path.ext.ptr);
	
	char file_suffix[] = "_mono";
	mut_cstring suffix;
	suffix.ptr = &file_suffix[0];
	suffix.size = sizeof(file_suffix)-1;
	add_to_string(&audio_path.file_name, &suffix);

	char* path_out = create_string(&audio_path);
	
	printf("[#] Output file: %s\n", path_out);
	
	FILE* fin = fopen(argv[1], "rb");
	FILE* fout = fopen(path_out, "wb");
	
	FILE* fsum;
	FILE* fdiff;
	
	if(write_streams)
	{
		char file_diff[] = "_diff";
		suffix.ptr = &file_diff[0];
		suffix.size = sizeof(file_diff)-1;
		add_to_string(&audio_path.file_name, &suffix);
		
		char* path_diff = create_string(&audio_path);

		char file_add[] = "_sum";
		suffix.ptr = &file_add[0];
		suffix.size = sizeof(file_add)-1;
		add_to_string(&audio_path.file_name, &suffix);
		
		char* path_sum = create_string(&audio_path);
		
		fsum = fopen(path_sum, "wb");
		fdiff = fopen(path_diff, "wb");
		printf("[#] Output file: %s\n", path_sum);
		printf("[#] Output file: %s\n", path_diff);
	}

	RIFF_HEADER header;
	read_riff_header(fin, &header);
	
	printf("\n==== PCM File\n");
	printf("Audio format: \t0x%x\n", header.audio_format);
	printf("Channels: \t%u\n", header.channels);
	printf("sample_rate: \t%u\n", header.sample_rate);
	printf("block_align: \t%u\n", header.block_align);
	printf("Bits/sample: \t%u\n", header.bits_per_sample);
	printf("Samples/block: \t%u\n", header.samples_block);
	printf("Data size: \t%u\n", header.data_size);
	printf("Samples: \t%u\n", header.num_samples);
	printf("====\n\n");
	
	if(header.channels != 2)
	{
		printf("File isn't 2 channel audio.\n");
		return 0;
	}
	
	header.channels = 1;
	header.byte_rate = (header.sample_rate * header.bits_per_sample * header.channels)/8;
	header.block_align = (header.channels * header.bits_per_sample)/8;
	
	write_riff_header(fout, &header, 0);
	
	if(write_streams)
	{
		write_riff_header(fdiff, &header, 0);
		write_riff_header(fsum, &header, 0);
	}
	
	short sample_in1 = 0;
	short sample_in2 = 0;
	short sample_out = 0;

	for(unsigned int it = 0; it != header.data_size; it+=4)
	{
		fread(&sample_in1, sizeof(short), 1, fin);
		fread(&sample_in2, sizeof(short), 1, fin);
		
		int dif = (sample_in2 - sample_in1)*diff_gain;

		int sample = sample_in1*sum_gain + sample_in2*sum_gain + dif;
		
		sample_out = clamp_s16(sample);
		fwrite(&sample_out, sizeof(short), 1, fout);
		
		if(write_streams)
		{
			sample_out = clamp_s16(sample_in1*sum_gain + sample_in2*sum_gain);
			fwrite(&sample_out, sizeof(short), 1, fsum);
			sample_out = dif;
			fwrite(&sample_out, sizeof(short), 1, fdiff);
		}
	}
	
	write_riff_header(fout, &header, 1);
	
	if(write_streams)
	{
		write_riff_header(fdiff, &header, 1);
		write_riff_header(fsum, &header, 1);
		fclose(fsum);
		fclose(fdiff);
	}
	
	fclose(fin);
	fclose(fout);
}