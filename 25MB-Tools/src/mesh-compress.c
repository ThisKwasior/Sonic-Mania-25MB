#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "miniz.h"

int main(int argc, char** argv)
{
    if(argc != 3) return 0; // No input and output
    
    uint32_t in_size = 0;
    mz_ulong out_size = 0;
    uint8_t* in_data = NULL;
    uint8_t* out_data = NULL;
    
    FILE* f_in = fopen(argv[1], "rb");
    fseek(f_in, 0, SEEK_END);
    in_size = ftell(f_in);
    fseek(f_in, 0, SEEK_SET);
    in_data = (uint8_t*)calloc(in_size, 1);
    int fread_stat = fread(in_data, in_size, 1, f_in);
    fclose(f_in);
    
    printf("File size: %u %d\n", in_size, fread_stat);
    
    out_size = in_size;
    out_data = (uint8_t*)calloc(in_size, 1);
    
    int comp_stat = mz_compress2(out_data, &out_size,
                                 in_data, in_size, MZ_BEST_COMPRESSION);
    
    if(comp_stat == MZ_OK)
    {
        printf("Compressed size: %u\n", out_size);
    }
    
    FILE* f_out = fopen(argv[2], "wb");
    fwrite("MDLZ", 4, 1, f_out);
    fwrite(&in_size, sizeof(uint32_t), 1, f_out);
    fwrite(&out_size, sizeof(mz_ulong), 1, f_out);
    fwrite(out_data, out_size, 1, f_out);
    fclose(f_out);
    
    free(in_data);
    free(out_data);
    
    return 0;
}