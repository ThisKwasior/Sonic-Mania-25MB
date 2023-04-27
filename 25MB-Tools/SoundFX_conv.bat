echo OFF

for /D %%d in (Data/SoundFX/*) do (
    for %%f in (Data/SoundFX/%%d/*.wav) do (
        echo %%d/%%f
        ffprobe -loglevel warning -hide_banner Data/SoundFX/%%d/%%f
        opusenc Data/SoundFX/%%d/%%f ../Data-Mod/Data/SoundFX/%%d/%%f --discard-comments --discard-pictures --framesize 60 --downmix-mono --music --bitrate 17 --padding 0
    )
)