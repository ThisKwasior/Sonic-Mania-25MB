for %%f in (Data/Music/*.ogg) do (
    ffmpeg -y -hide_banner -i Data/Music/%%f ../Data-Mod/Data/Music/%%~nf.wav
    stereo2monophase ../Data-Mod/Data/Music/%%~nf.wav 0.5 0.6
    opusenc ../Data-Mod/Data/Music/%%~nf_mono.wav ../Data-Mod/Data/Music/%%~nf.ogg --discard-comments --discard-pictures --framesize 60 --music --bitrate 12
    del ..\Data-Mod\Data\Music\%%~nf.wav
    del ..\Data-Mod\Data\Music\%%~nf_mono.wav

)