#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vpx/vpx_decoder.h"

#include "tools_common.h"
#include "video_reader.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "plane_converters.h"

int main(int argc, char** argv)
{
    if(argc != 2) return 0;
    
    int frame_cnt = 0;
    vpx_codec_ctx_t codec;
    VpxVideoReader* reader = NULL;
    const VpxInterface* decoder = NULL;
    const VpxVideoInfo* info = NULL;
    
    reader = vpx_video_reader_open(argv[1]);
    if(!reader)
    {
        printf("Reader died\n");
        return 0;
    }
    
    info = vpx_video_reader_get_info(reader);
    decoder = get_vpx_decoder_by_fourcc(info->codec_fourcc);
    if(!decoder)
    {
        printf("decoder died\n");
        return 0;
    }
    
    printf("Using %s\n", vpx_codec_iface_name(decoder->codec_interface()));
    
    if(vpx_codec_dec_init(&codec, decoder->codec_interface(), NULL, 0))
    {
        printf("Failed to initialize decoder.\n");
        return 0;
    }
    
    while (vpx_video_reader_read_frame(reader))
    {
        vpx_codec_iter_t iter = NULL;
        vpx_image_t* img = NULL;
        size_t frame_size = 0;
        const unsigned char* frame = vpx_video_reader_get_frame(reader, &frame_size);
        if(vpx_codec_decode(&codec, frame, (unsigned int)frame_size, NULL, 0))
        {
            printf("Failed to decode frame\n");
            return 0;
        }

        while((img = vpx_codec_get_frame(&codec, &iter)) != NULL)
        {
            //vpx_img_write(img, outfile);
            frame_cnt+=1;
            
            /*printf("%u %u %u %u %u %u\n",
                   img->w, img->h, img->d_w, img->d_h,
                   img->r_w, img->r_h);*/

            const unsigned char* ybuf = img->planes[0];
            const unsigned char* ubuf = img->planes[1];
            const unsigned char* vbuf = img->planes[2];
            int w = vpx_img_plane_width(img, 0);
            int h = vpx_img_plane_height(img, 0);
            int stride = img->stride[0];
            int strideuv = img->stride[1];
            unsigned char* rgbbuf = (unsigned char*)calloc(w*h*3, 1);

            //printf("%u %u %u\n", w, h, stride);

            uint32_t rgb_pos = 0;
            for(uint32_t img_y = 0; img_y != h; ++img_y)
            {
                for(uint32_t img_x = 0; img_x != w; ++img_x)
                {
                    const uint32_t ypos = (img_y*stride)+img_x;
                    const uint32_t uvpos = ((img_y/2)*strideuv)+(img_x/2);
                    /*rgbbuf[rgb_pos] = ybuf[ypos];
                    rgbbuf[rgb_pos+1] = ubuf[uvpos];
                    rgbbuf[rgb_pos+2] = vbuf[uvpos];*/
                    
                    conv_YUV_to_RGB(ybuf[ypos], vbuf[uvpos], ubuf[uvpos],
                                    &rgbbuf[rgb_pos++], &rgbbuf[rgb_pos++], &rgbbuf[rgb_pos++]);
                }
            }
            
            char path[0x100] = {0};
            sprintf(path, "./frames/%04d.png", frame_cnt);
            stbi_write_png(path, w, h, 3, rgbbuf, w*3);
            free(rgbbuf);
            
        }
    }
    
    if(vpx_codec_destroy(&codec))
    {
        printf("Failed to destroy codec");
    }

    vpx_video_reader_close(reader);

    
}