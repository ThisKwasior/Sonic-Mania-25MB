#ifndef AUDIO_H
#define AUDIO_H

namespace RSDK
{

#define SFX_COUNT     (0x100)
#define CHANNEL_COUNT (0x10)

#define MIX_BUFFER_SIZE (0x800)
#define SAMPLE_FORMAT   float

//#define AUDIO_FREQUENCY (44100)
#define AUDIO_FREQUENCY (48000) /* Because of OPUS */
#define AUDIO_CHANNELS  (2)

struct SFXInfo {
    RETRO_HASH_MD5(hash);
    float *buffer;
    size_t length;
    int32 playCount;
    uint8 maxConcurrentPlays;
    uint8 scope;
};

struct ChannelInfo {
    float *samplePtr;
    float pan;
    float volume;
    int32 speed;
    size_t sampleLength;
    int32 bufferPos;
    int32 playIndex;
    uint32 loop;
    int16 soundID;
    uint8 priority;
    uint8 state;
};

enum ChannelStates { CHANNEL_IDLE, CHANNEL_SFX, CHANNEL_STREAM, CHANNEL_LOADING_STREAM, CHANNEL_PAUSED = 0x40 };

extern SFXInfo sfxList[SFX_COUNT];
extern ChannelInfo channels[CHANNEL_COUNT];

class AudioDeviceBase
{
public:
    static bool32 Init();
    static void Release();

    static void ProcessAudioMixing(void *stream, int32 length);

    static void FrameInit();

    static void HandleStreamLoad(ChannelInfo *channel, bool32 async);

    static uint8 initializedAudioChannels;
    static uint8 audioState;
    static uint8 audioFocus;

protected:
    static void InitAudioChannels();
};

void UpdateStreamBuffer(ChannelInfo *channel);
void LoadStream(ChannelInfo *channel);
int32 PlayStream(const char *filename, uint32 slot, uint32 startPos, uint32 loopPoint, bool32 loadASync);

void LoadSfxToSlot(char *filename, uint8 slot, uint8 plays, uint8 scope);
void LoadSfx(char *filePath, uint8 plays, uint8 scope);

} // namespace RSDK

#if RETRO_AUDIODEVICE_XAUDIO
#include "XAudio/XAudioDevice.hpp"
#elif RETRO_AUDIODEVICE_PORT
#include "PortAudio/PortAudioDevice.hpp"
#elif RETRO_AUDIODEVICE_SDL2
#include "SDL2/SDL2AudioDevice.hpp"
#elif RETRO_AUDIODEVICE_OBOE
#include "Oboe/OboeAudioDevice.hpp"
#endif

namespace RSDK
{

inline uint16 GetSfx(const char *sfxName)
{
    RETRO_HASH_MD5(hash);
    GEN_HASH_MD5(sfxName, hash);

    for (int32 s = 0; s < SFX_COUNT; ++s) {
        if (HASH_MATCH_MD5(sfxList[s].hash, hash))
            return s;
    }

    return -1;
}
int32 PlaySfx(uint16 sfx, uint32 loopPoint, uint32 priority);
inline void StopSfx(uint16 sfx)
{
#if !RETRO_USE_ORIGINAL_CODE
    LockAudioDevice();
#endif

    for (int32 i = 0; i < CHANNEL_COUNT; ++i) {
        if (channels[i].soundID == sfx) {
            MEM_ZERO(channels[i]);
            channels[i].soundID = -1;
            channels[i].state   = CHANNEL_IDLE;
        }
    }

#if !RETRO_USE_ORIGINAL_CODE
    UnlockAudioDevice();
#endif
}

#if RETRO_REV0U
inline void StopAllSfx()
{
#if !RETRO_USE_ORIGINAL_CODE
    LockAudioDevice();
#endif

    for (int32 i = 0; i < CHANNEL_COUNT; ++i) {
        if (channels[i].state == CHANNEL_SFX) {
            MEM_ZERO(channels[i]);
            channels[i].soundID = -1;
            channels[i].state   = CHANNEL_IDLE;
        }
    }

#if !RETRO_USE_ORIGINAL_CODE
    UnlockAudioDevice();
#endif
}
#endif

void SetChannelAttributes(uint8 channel, float volume, float panning, float speed);

inline void StopChannel(uint32 channel)
{
    if (channel < CHANNEL_COUNT) {
        if (channels[channel].state != CHANNEL_LOADING_STREAM)
            channels[channel].state = CHANNEL_IDLE;
    }
}

inline void PauseChannel(uint32 channel)
{
    if (channel < CHANNEL_COUNT) {
        if (channels[channel].state != CHANNEL_LOADING_STREAM)
            channels[channel].state |= CHANNEL_PAUSED;
    }
}

inline void ResumeChannel(uint32 channel)
{
    if (channel < CHANNEL_COUNT) {
        if (channels[channel].state != CHANNEL_LOADING_STREAM)
            channels[channel].state &= ~CHANNEL_PAUSED;
    }
}

inline void PauseSound()
{
    for (int32 c = 0; c < CHANNEL_COUNT; ++c) PauseChannel(c);
}

inline void ResumeSound()
{
    for (int32 c = 0; c < CHANNEL_COUNT; ++c) ResumeChannel(c);
}

inline bool32 SfxPlaying(uint16 sfx)
{
    for (int32 c = 0; c < CHANNEL_COUNT; ++c) {
        if (channels[c].state == CHANNEL_SFX && channels[c].soundID == sfx)
            return true;
    }
    return false;
}

inline bool32 ChannelActive(uint32 channel)
{
    if (channel >= CHANNEL_COUNT)
        return false;
    else
        return (channels[channel].state & 0x3F) != CHANNEL_IDLE;
}

uint32 GetChannelPos(uint32 channel);
double GetVideoStreamPos();

void ClearStageSfx();
#if RETRO_USE_MOD_LOADER
void ClearGlobalSfx();
#endif

#if RETRO_REV0U
#include "Legacy/AudioLegacy.hpp"
#endif

} // namespace RSDK

#endif
