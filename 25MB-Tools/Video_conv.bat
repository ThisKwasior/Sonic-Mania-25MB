set COMMON_FLAGS= -vf scale=-2:120 -minrate 24K -b:v 48K -maxrate 96K -deadline best -speed 0 -g 300 -row-mt 1 -vsync 0

for %%f in (Data/Video/*.ogv) do (
    ffmpeg -y -hide_banner -i Data/Video/%%f -c:v libvpx-vp9 %COMMON_FLAGS% -pass 1 -f ivf ../Data-Mod/Data/Video/%%~nf.ogv
    ffmpeg -y -hide_banner -i Data/Video/%%f -c:v libvpx-vp9 %COMMON_FLAGS% -pass 2 -f ivf ../Data-Mod/Data/Video/%%~nf.ogv
)