echo OFF

set DATA_OG=./Data-rsdk/Data-Original
set DATA_SML=./Data-rsdk/Data-Small

rem Windows bad bottom text
set DATA_OG_W=.\Data-rsdk\Data-Original
set DATA_SML_W=.\Data-rsdk\Data-Small

set FF_CMN_FLAGS= -loglevel error -hide_banner -y

echo ##### Unpacking the Data.rsdk
    25MB-Tools\RSDKv5U_Toolkit\RSDKv5Extract.exe ./Data-rsdk/Data.rsdk ^
                                                 ./25MB-Tools/RSDKv5U_Toolkit/rsdk_files_list.txt ^
                                                 %DATA_OG%
                                             
echo ##### Creating the Data working directory
    xcopy %DATA_OG_W%\ %DATA_SML_W%\ /E /Y /Q
    rmdir /S /Q %DATA_SML_W%\Data\Images
    mkdir %DATA_SML_W%\Data\Images
    rmdir /S /Q %DATA_SML_W%\Data\SoundFX
    mkdir %DATA_SML_W%\Data\SoundFX
    rmdir /S /Q %DATA_SML_W%\Data\Music
    mkdir %DATA_SML_W%\Data\Music
    rmdir /S /Q %DATA_SML_W%\Data\Video
    mkdir %DATA_SML_W%\Data\Video

echo ##### Compressing Images
ffmpeg %FF_CMN_FLAGS% -i %DATA_OG%/Data/Images/TrueEnd.png ^
       -filter_complex "palettegen=max_colors=48[pl],[0:v][pl]paletteuse=dither=sierra3:new=1" -pix_fmt rgb24 ^
       %DATA_SML%/Data/Images/TrueEnd.png
       
ffmpeg %FF_CMN_FLAGS% -i %DATA_OG%/Data/Images/CESA.png ^
       -filter_complex "palettegen=reserve_transparent=false:max_colors=3[pl],[0:v][pl]paletteuse=dither=sierra3:new=1" -pix_fmt rgb24 ^
       %DATA_SML%/Data/Images/CESA.png

echo ##### Converting Sound Effects to Opus
    for /D %%d in (%DATA_OG%/Data/SoundFX/*) do (
        mkdir %DATA_SML_W%\Data\SoundFX\%%d
        for %%f in (%DATA_OG%/Data/SoundFX/%%d/*.wav) do (
            rem Some WAV files have wrong type of IEEE Float samples, but data is s16le
            ECHO F|xcopy %DATA_OG_W%\Data\SoundFX\%%d\%%f %DATA_SML_W%\Data\SoundFX\%%d\%%~nf_temp.wav
            25MB-Tools\fix_wav_header %DATA_SML%/Data/SoundFX/%%d/%%~nf_temp.wav
            25MB-Tools\opusenc %DATA_SML%/Data/SoundFX/%%d/%%~nf_temp.wav %DATA_SML%/Data/SoundFX/%%d/%%f ^
                    --discard-comments --discard-pictures --framesize 60 --downmix-mono --music --bitrate 17 --padding 0
            del %DATA_SML_W%\Data\SoundFX\%%d\%%~nf_temp.wav
        )
    )
    
echo ##### Converting Music to Opus
    for %%f in (%DATA_OG%/Data/Music/*.ogg) do (
        ffmpeg %FF_CMN_FLAGS% -i %DATA_OG%/Data/Music/%%f %DATA_SML%/Data/Music/%%~nf.wav
        25MB-Tools\stereo2monophase %DATA_SML%/Data/Music/%%~nf.wav 0.5 0.6
        25MB-Tools\opusenc %DATA_SML%/Data/Music/%%~nf_mono.wav %DATA_SML%/Data/Music/%%~nf.ogg ^
                           --discard-comments --discard-pictures --framesize 60 --music --bitrate 12
        del %DATA_SML_W%\Data\Music\%%~nf.wav
        del %DATA_SML_W%\Data\Music\%%~nf_mono.wav
    )

echo ##### Converting Video to VP9
echo ##### This might take some time
set VID_RES=-vf scale=-2:120
set BITRATE_MANIA=-minrate 24K -b:v 48K -maxrate 72K
set BITRATE_END=-minrate 16K -b:v 32K -maxrate 48K
set BITRATE_BAD=-minrate 12K -b:v 24K -maxrate 36K
set CUR_BITRATE=-minrate 12K -b:v 24K -maxrate 36K
set VID_CMN_FLAGS= -c:v libvpx-vp9 -deadline best -speed 0 -g 300 -row-mt 1 -vsync 0 -pix_fmt yuv420p -threads 4

for %%f in (%DATA_OG%/Data/Video/*.ogv) do (
    echo Converting %%f
    
    set CUR_BITRATE=%BITRATE_BAD%
    
    if "%%f"=="GoodEnd.ogv" (
        set CUR_BITRATE=%BITRATE_END%
    )
    if "%%f"=="MREnd.ogv" (
        set CUR_BITRATE=%BITRATE_END%
    )
    if "%%f"=="BadEnd.ogv" (
        set CUR_BITRATE=%BITRATE_END%
    )
    
    if "%%f"=="Mania.ogv" (
        set CUR_BITRATE=%BITRATE_MANIA%
        rem Mania.ogv is a special case because of weird timestamps at the beginning of the file
        mkdir %DATA_SML_W%\Data\Video\Mania_frames
        ffmpeg %FF_CMN_FLAGS% -stats -i %DATA_OG%/Data/Video/%%f %VID_RES% %DATA_SML%/Data/Video/Mania_frames/%%04d.png
        ffmpeg %FF_CMN_FLAGS% -stats -start_number 174 -r 24 -i %DATA_SML%/Data/Video/Mania_frames/%%04d.png ^
               %VID_RES% %VID_CMN_FLAGS% %CUR_BITRATE% -pass 1 ^
               -f ivf %DATA_SML%/Data/Video/%%~nf.ogv
        ffmpeg %FF_CMN_FLAGS% -stats -start_number 174 -r 24 -i %DATA_SML%/Data/Video/Mania_frames/%%04d.png ^
               %VID_RES% %VID_CMN_FLAGS% %CUR_BITRATE% -pass 2 ^
               -f ivf %DATA_SML%/Data/Video/%%~nf.ogv  
        rmdir /S /Q %DATA_SML_W%\Data\Video\Mania_frames
    ) else (
        ffmpeg %FF_CMN_FLAGS% -stats -i %DATA_OG%/Data/Video/%%f %VID_RES% %VID_CMN_FLAGS% %CUR_BITRATE% -pass 1 -f ivf %DATA_SML%/Data/Video/%%~nf.ogv
        ffmpeg %FF_CMN_FLAGS% -stats -i %DATA_OG%/Data/Video/%%f %VID_RES% %VID_CMN_FLAGS% %CUR_BITRATE% -pass 2 -f ivf %DATA_SML%/Data/Video/%%~nf.ogv
    )
)

echo ##### Compressing Models
    for /D %%d in (%DATA_OG%/Data/Meshes/*) do (
        for %%f in (%DATA_OG%/Data/Meshes/%%d/*.bin) do (
            25MB-Tools\mesh-compress %DATA_OG%/Data/Meshes/%%d/%%f %DATA_SML%/Data/Meshes/%%d/%%f
        )
    )

echo ##### Removing unused files
    del %DATA_SML_W%\Data\Stages\MMZ\Temp.gif
    del %DATA_SML_W%\Data\Sprites\TMZ1\Objects-Temp.gif
    del %DATA_SML_W%\Data\Sprites\PSZ2\Objects-Temp.gif
    del %DATA_SML_W%\Data\Sprites\PSZ2\Enemies-Temp.gif
    del %DATA_SML_W%\Data\Sprites\LRZ2\ObjectsTemp.gif
    del %DATA_SML_W%\Data\Sprites\LRZ1\Objects-Temp.gif
    del %DATA_SML_W%\Data\Sprites\UI\TextEN_old.bin
    del %DATA_SML_W%\Data\Sprites\UI\TextEN_old.gif
    del %DATA_SML_W%\Data\Sprites\UI\SaveSelect_old.gif
    del %DATA_SML_W%\Data\Sprites\UI\PauseEN_old.gif
    del %DATA_SML_W%\Data\Sprites\SSZ2\Kabasira_old.bin
    del %DATA_SML_W%\Data\Sprites\SSZ1\Kabasira_old.bin
    del "%DATA_SML_W%\Data\Sprites\PSZ2\Enemies - Old.gif"
    del %DATA_SML_W%\Data\Sprites\Global\ShieldsOld.bin
    del %DATA_SML_W%\Data\Sprites\Global\ShieldsOld.gif
    del %DATA_SML_W%\Data\Sprites\Title\Image1.png
    del %DATA_SML_W%\Data\Sprites\UI\Diorama_TMP.gif
    
echo ##### Making Mania English-Only
    del %DATA_SML_W%\Data\Sprites\UI\*FR.*
    del %DATA_SML_W%\Data\Sprites\UI\*GE.*
    del %DATA_SML_W%\Data\Sprites\UI\*IT.*
    del %DATA_SML_W%\Data\Sprites\UI\*JP.*
    del %DATA_SML_W%\Data\Sprites\UI\*KO.*
    del %DATA_SML_W%\Data\Sprites\UI\*SC.*
    del %DATA_SML_W%\Data\Sprites\UI\*SP.*
    del %DATA_SML_W%\Data\Sprites\UI\*TC.*
    copy %DATA_SML_W%\Data\Sprites\UI\HeadingsEN.bin %DATA_SML_W%\Data\Sprites\UI\HeadingsFR.bin
    copy %DATA_SML_W%\Data\Sprites\UI\HeadingsEN.bin %DATA_SML_W%\Data\Sprites\UI\HeadingsGE.bin
    copy %DATA_SML_W%\Data\Sprites\UI\HeadingsEN.bin %DATA_SML_W%\Data\Sprites\UI\HeadingsIT.bin
    copy %DATA_SML_W%\Data\Sprites\UI\HeadingsEN.bin %DATA_SML_W%\Data\Sprites\UI\HeadingsJP.bin
    copy %DATA_SML_W%\Data\Sprites\UI\HeadingsEN.bin %DATA_SML_W%\Data\Sprites\UI\HeadingsKO.bin
    copy %DATA_SML_W%\Data\Sprites\UI\HeadingsEN.bin %DATA_SML_W%\Data\Sprites\UI\HeadingsSC.bin
    copy %DATA_SML_W%\Data\Sprites\UI\HeadingsEN.bin %DATA_SML_W%\Data\Sprites\UI\HeadingsSP.bin
    copy %DATA_SML_W%\Data\Sprites\UI\HeadingsEN.bin %DATA_SML_W%\Data\Sprites\UI\HeadingsTC.bin
    copy %DATA_SML_W%\Data\Sprites\UI\TextEN.bin %DATA_SML_W%\Data\Sprites\UI\TextFR.bin
    copy %DATA_SML_W%\Data\Sprites\UI\TextEN.bin %DATA_SML_W%\Data\Sprites\UI\TextGE.bin
    copy %DATA_SML_W%\Data\Sprites\UI\TextEN.bin %DATA_SML_W%\Data\Sprites\UI\TextIT.bin
    copy %DATA_SML_W%\Data\Sprites\UI\TextEN.bin %DATA_SML_W%\Data\Sprites\UI\TextJP.bin
    copy %DATA_SML_W%\Data\Sprites\UI\TextEN.bin %DATA_SML_W%\Data\Sprites\UI\TextKO.bin
    copy %DATA_SML_W%\Data\Sprites\UI\TextEN.bin %DATA_SML_W%\Data\Sprites\UI\TextSC.bin
    copy %DATA_SML_W%\Data\Sprites\UI\TextEN.bin %DATA_SML_W%\Data\Sprites\UI\TextSP.bin
    copy %DATA_SML_W%\Data\Sprites\UI\TextEN.bin %DATA_SML_W%\Data\Sprites\UI\TextTC.bin

echo ##### Packing the Data.rsdk
    25MB-Tools\RSDKv5U_Toolkit\RSDKv5Pack.exe ./Data-rsdk/Data-Small ^
                                              ./Data-rsdk/Data-Small.rsdk