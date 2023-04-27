rmdir /S /Q .\bin

mkdir build-25mania
cd build-25mania

cmake ../RSDKv5-Decompilation -DGAME_STATIC=True -DCMAKE_BUILD_TYPE=minsizerel -G"MinGW Makefiles" ^
    -DCMAKE_CXX_FLAGS=" -fpermissive -static -static-libgcc -static-libstdc++ -ffunction-sections  -Wl,--gc-sections -D_WIN32_WINNT=0x0501 -DWINVER=0x0501" ^
    -DCMAKE_C_FLAGS=" -static -static-libgcc -ffunction-sections  -Wl,--gc-sections -D_WIN32_WINNT=0x0501 -DWINVER=0x0501" ^
    -DCMAKE_CXX_FLAGS_MINSIZEREL="-Os -DNDEBUG -s -pie" ^
    -DCMAKE_C_FLAGS_MINSIZEREL="-Os -DNDEBUG -s -pie" ^
    -DRETRO_EMBEDDED_DATA=True ^
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="../bin/"
    
mingw32-make -j 4

cd ..

25MB-Tools\upx-4.0.2-win64\upx.exe -9 -o ./bin/Sonic25Mania.exe ./bin/RSDKv5U.exe