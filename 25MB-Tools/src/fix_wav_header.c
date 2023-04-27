#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

int main(int argc, char** argv)
{
    if(argc == 1) return 0;
    
    FILE* f = fopen(argv[1], "rb");
    fseek(f, 0, SEEK_END);
    const uint32_t wav_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t* wav_data = (uint8_t*)calloc(wav_size, 1);
    fread(wav_data, wav_size, 1, f);
    fclose(f);
    
    if(wav_data[0x14] == 3)
    {
        wav_data[0x14] = 1;
        f = fopen(argv[1], "wb");
        fwrite(wav_data, wav_size, 1, f);
        fclose(f);
    }
    
    return 0;
}