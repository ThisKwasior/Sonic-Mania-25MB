#include "RSDK/Core/RetroEngine.hpp"

using namespace RSDK;

FileInfo VideoManager::file;

/*ogg_sync_state VideoManager::oy;
ogg_page VideoManager::og;
ogg_packet VideoManager::op;
ogg_stream_state VideoManager::vo;
ogg_stream_state VideoManager::to;
th_info VideoManager::ti;
th_comment VideoManager::tc;
th_dec_ctx *VideoManager::td    = NULL;
th_setup_info *VideoManager::ts = NULL;

th_pixel_fmt VideoManager::pixelFormat;*/
ogg_int64_t VideoManager::granulePos = 0;
bool32 VideoManager::initializing    = false;

/* VP9 */
#include "vpx/vpx_decoder.h"
#include "tools_common.h"
#include "video_reader.h"

vpx_codec_ctx_t codec;
VpxVideoReader* reader = NULL;
const VpxInterface* decoder = NULL;
const VpxVideoInfo* info = NULL;
int cur_frame = 0;

bool32 RSDK::LoadVideo(const char *filename, double startDelay, bool32 (*skipCallback)())
{
    if (ENGINE_VERSION == 5 && sceneInfo.state == ENGINESTATE_VIDEOPLAYBACK)
        return false;
#if RETRO_REV0U
    if (ENGINE_VERSION == 3 && RSDK::Legacy::gameMode == RSDK::Legacy::v3::ENGINE_VIDEOWAIT)
        return false;
#endif

    char fullFilePath[0x80];
    sprintf_s(fullFilePath, sizeof(fullFilePath), "Data/Video/%s", filename);

    InitFileInfo(&VideoManager::file);
    if (LoadFile(&VideoManager::file, fullFilePath, FMODE_RB)) {
        // Init
        /*ogg_sync_init(&VideoManager::oy);

        th_comment_init(&VideoManager::tc);
        th_info_init(&VideoManager::ti);

        int32 theora_p = 0;
        char *buffer   = NULL;*/
        
        reader = vpx_video_reader_open_retro(&VideoManager::file);
        info = vpx_video_reader_get_info(reader);
        decoder = get_vpx_decoder_by_fourcc(info->codec_fourcc);
        vpx_codec_dec_init(&codec, decoder->codec_interface(), NULL, 0);
        cur_frame = 0;

            if (!reader) {
                vpx_video_reader_close(reader);
            }
            else {
                engine.storedShaderID     = videoSettings.shaderID;
                videoSettings.screenCount = 0;

                if (ENGINE_VERSION == 5)
                    engine.storedState = sceneInfo.state;
#if RETRO_REV0U
                else if (ENGINE_VERSION == 3)
                    engine.storedState = RSDK::Legacy::gameMode;
#endif
                engine.displayTime         = 0.0;
                VideoManager::initializing = true;
                VideoManager::granulePos   = 0;

                engine.displayTime     = 0.0;
                engine.videoStartDelay = 0.0;
                if (AudioDevice::audioState == 1)
                    engine.videoStartDelay = startDelay;

                videoSettings.shaderID = SHADER_YUV_420;
                
                engine.skipCallback = NULL;
                ProcessVideo();
                engine.skipCallback = skipCallback;

                changedVideoSettings = false;
                if (ENGINE_VERSION == 5)
                    sceneInfo.state = ENGINESTATE_VIDEOPLAYBACK;
#if RETRO_REV0U
                else if (ENGINE_VERSION == 3)
                    RSDK::Legacy::gameMode = RSDK::Legacy::v3::ENGINE_VIDEOWAIT;
#endif

                return true;
            }
        }

        CloseFile(&VideoManager::file);

    return false;
}

void RSDK::ProcessVideo()
{
    bool32 finished = false;
    double curTime = 0;
    if (!VideoManager::initializing) {
        double streamPos = GetVideoStreamPos();

        if (streamPos <= -1.0)
            engine.displayTime += (1.0 / 60.0); // deltaTime frame-step
        else
            engine.displayTime = streamPos;

        //curTime = th_granule_time(VideoManager::td, VideoManager::granulePos);
        curTime = cur_frame/(info->time_base.numerator/(double)info->time_base.denominator);

        //PrintLog(PRINT_ERROR, "Stream pos: %lf %lf", streamPos, curTime);

#if RETRO_USE_MOD_LOADER
        RunModCallbacks(MODCB_ONVIDEOSKIPCB, (void *)engine.skipCallback);
#endif
        if (engine.skipCallback && engine.skipCallback()) {
            finished = true;
        }
    }

    if (!finished && (VideoManager::initializing || engine.displayTime >= engine.videoStartDelay + curTime)) {
         // No more frames
        if(!vpx_video_reader_read_frame_retro(reader))
        {
            finished = true; 
        }
        
        vpx_codec_iter_t iter = NULL;
        vpx_image_t* img = NULL;
        size_t frame_size = 0;
        const unsigned char* frame = vpx_video_reader_get_frame(reader, &frame_size);

        if (!finished && !vpx_codec_decode(&codec, frame, (unsigned int)frame_size, NULL, 0)) {
            img = vpx_codec_get_frame(&codec, &iter);
            cur_frame+=1;
            //int32 dataPos = (VideoManager::ti.pic_x & 0xFFFFFFFE) + (VideoManager::ti.pic_y & 0xFFFFFFFE) * yuv[0].stride;
                        
            RenderDevice::SetupVideoTexture_YUV420(
                        vpx_img_plane_width(img, 0), vpx_img_plane_height(img, 0),
                        img->planes[0], img->planes[1], img->planes[2],
                        img->stride[0], img->stride[1], img->stride[2]);
        }

        VideoManager::initializing = false;
    }

    if (finished) {
        CloseFile(&VideoManager::file);

        vpx_video_reader_close_retro(reader);

        videoSettings.shaderID    = engine.storedShaderID;
        videoSettings.screenCount = 1;
        if (ENGINE_VERSION == 5)
            sceneInfo.state = engine.storedState;
#if RETRO_REV0U
        else if (ENGINE_VERSION == 3)
            RSDK::Legacy::gameMode = engine.storedState;
#endif
    }
}
