# Sonic-Mania-25MB
RSDKv5 and Sonic Mania decompilation with added VP9 video and OPUS audio capabilities to make the game 25MB.

# How to build
You need to download some stuff first.
  - [CMake](https://cmake.org/download/)
  - [This specific version of MinGW-w64](https://github.com/brechtsanders/winlibs_mingw/releases/download/12.2.0-16.0.0-10.0.0-msvcrt-r5/winlibs-i686-posix-dwarf-gcc-12.2.0-mingw-w64msvcrt-10.0.0-r5.7z)
  - [FFmpeg](https://ffmpeg.org/download.html)

After that you need to:
  - install CMake (add it to PATH in the installer)
  - unpack both the compiler and FFmpeg
  - add both `mingw32/bin` and `ffmpeg/bin` folders to PATH, the directory should have no spaces!!!

When it's done, copy `Data.rsdk` from your copy of Sonic Mania Plus to the `./Data-rsdk` directory and run `compress-data-rsdk.bat` - it will take some time because video compression.
Then, you will have a `./Data-rsdk/Data-Small.rsdk` file, hopefully it'll be 24.7MB.
At long last, run `compile-the-thing.bat` - after another 10 minutes you will get `Sonic25Mania.exe` in the `./bin` directory.

That's all, have fun.
