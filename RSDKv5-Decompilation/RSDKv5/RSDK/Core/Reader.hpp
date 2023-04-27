#ifndef READER_H
#define READER_H

#if RETRO_RENDERDEVICE_SDL2 || RETRO_AUDIODEVICE_SDL2 || RETRO_INPUTDEVICE_SDL2
#define FileIO                                          SDL_RWops
#define fOpen(path, mode)                               SDL_RWFromFile(path, mode)
#define fRead(buffer, elementSize, elementCount, file)  SDL_RWread(file, buffer, elementSize, elementCount)
#define fSeek(file, offset, whence)                     SDL_RWseek(file, offset, whence)
#define fTell(file)                                     SDL_RWtell(file)
#define fClose(file)                                    SDL_RWclose(file)
#define fWrite(buffer, elementSize, elementCount, file) SDL_RWwrite(file, buffer, elementSize, elementCount)
#else
#define FileIO                                          FILE
#define fOpen(path, mode)                               fopen(path, mode)
#define fRead(buffer, elementSize, elementCount, file)  fread(buffer, elementSize, elementCount, file)
#define fSeek(file, offset, whence)                     fseek(file, offset, whence)
#define fTell(file)                                     ftell(file)
#define fClose(file)                                    fclose(file)
#define fWrite(buffer, elementSize, elementCount, file) fwrite(buffer, elementSize, elementCount, file)
#endif

#if RETRO_PLATFORM == RETRO_ANDROID
#undef fOpen
FileIO *fOpen(const char *path, const char *mode);
#endif

#include <miniz/miniz.h>

namespace RSDK
{

#define RSDK_SIGNATURE_RSDK (0x4B445352) // "RSDK"
#if RETRO_REV0U
#define RSDK_SIGNATURE_DATA (0x61746144) // "Data"
#endif

#define DATAFILE_COUNT (0x1000)
#define DATAPACK_COUNT (4)

enum Scopes {
    SCOPE_NONE,
    SCOPE_GLOBAL,
    SCOPE_STAGE,
};

struct FileInfo {
    int32 fileSize;
    int32 externalFile;
    FileIO *file;
    uint8 *fileBuffer;
    int32 readPos;
    int32 fileOffset;
    uint8 usingFileBuffer;
    uint8 encrypted;
    uint8 eNybbleSwap;
    uint8 encryptionKeyA[0x10];
    uint8 encryptionKeyB[0x10];
    uint8 eKeyPosA;
    uint8 eKeyPosB;
    uint8 eKeyNo;
};

struct RSDKFileInfo {
    RETRO_HASH_MD5(hash);
    int32 size;
    int32 offset;
    uint8 encrypted;
    uint8 useFileBuffer;
    int32 packID;
};

struct RSDKContainer {
    char name[0x100];
    uint8 *fileBuffer;
    int32 fileCount;
};

extern RSDKFileInfo dataFileList[DATAFILE_COUNT];
extern RSDKContainer dataPacks[DATAPACK_COUNT];

extern uint8 dataPackCount;
extern uint16 dataFileListCount;

extern char gameLogicName[0x200];

extern bool32 useDataPack;

#if RETRO_REV0U
void DetectEngineVersion();
#endif
bool32 LoadDataPack(const char *filename, size_t fileOffset, bool32 useBuffer);
bool32 OpenDataFile(FileInfo *info, const char *filename);

enum FileModes { FMODE_NONE, FMODE_RB, FMODE_WB, FMODE_RB_PLUS };

static const char *openModes[3] = { "rb", "wb", "rb+" };

inline bool32 CheckBigEndian()
{
    uint32 x = 1;
    uint8 *c = (uint8 *)&x;
    return ((int32)*c) == 0;
}

inline void InitFileInfo(FileInfo *info)
{
    info->file            = NULL;
    info->fileSize        = 0;
    info->externalFile    = false;
    info->usingFileBuffer = false;
    info->encrypted       = false;
    info->readPos         = 0;
    info->fileOffset      = 0;
}

bool32 LoadFile(FileInfo *info, const char *filename, uint8 fileMode);

inline void CloseFile(FileInfo *info)
{
    if (!info->usingFileBuffer && info->file)
        fClose(info->file);

    info->file = NULL;
}

void GenerateELoadKeys(FileInfo *info, const char *key1, int32 key2);
void DecryptBytes(FileInfo *info, void *buffer, size_t size);
void SkipBytes(FileInfo *info, int32 size);

inline void Seek_Set(FileInfo *info, int32 count)
{
    if (info->readPos != count) {
        if (info->encrypted) {
            info->eKeyNo      = (info->fileSize / 4) & 0x7F;
            info->eKeyPosA    = 0;
            info->eKeyPosB    = 8;
            info->eNybbleSwap = false;
            SkipBytes(info, count);
        }

        info->readPos = count;
        if (info->usingFileBuffer) {
            uint8 *fileBuffer = (uint8 *)info->file;
            info->fileBuffer  = &fileBuffer[info->readPos];
        }
        else {
            fSeek(info->file, info->fileOffset + info->readPos, SEEK_SET);
        }
    }
}

inline void Seek_Cur(FileInfo *info, int32 count)
{
    info->readPos += count;

    if (info->encrypted)
        SkipBytes(info, count);

    if (info->usingFileBuffer) {
        info->fileBuffer += count;
    }
    else {
        fSeek(info->file, count, SEEK_CUR);
    }
}

inline size_t ReadBytes(FileInfo *info, void *data, int32 count)
{
    size_t bytesRead = 0;

    if (info->usingFileBuffer) {
        bytesRead = MIN(count, info->fileSize - info->readPos);
        memcpy(data, info->fileBuffer, bytesRead);
        info->fileBuffer += bytesRead;
    }
    else {
        bytesRead = fRead(data, 1, count, info->file);
    }

    if (info->encrypted)
        DecryptBytes(info, data, bytesRead);

    info->readPos += (int32)bytesRead;
    return bytesRead;
}

inline uint8 ReadInt8(FileInfo *info)
{
    int8 result      = 0;
    size_t bytesRead = 0;

    if (info->usingFileBuffer) {
        bytesRead = MIN(sizeof(int8), info->fileSize - info->readPos);
        if (bytesRead) {
            result = info->fileBuffer[0];
            info->fileBuffer += sizeof(int8);
        }
    }
    else {
        bytesRead = fRead(&result, 1, sizeof(int8), info->file);
    }

    if (info->encrypted)
        DecryptBytes(info, &result, bytesRead);

    info->readPos += (int32)bytesRead;
    return result;
}

inline int16 ReadInt16(FileInfo *info)
{
    union {
        uint16 result;
        uint8 b[sizeof(result)];
    } buffer;
    memset(&buffer, 0, sizeof(buffer));

    size_t bytesRead = 0;

    if (info->usingFileBuffer) {
        bytesRead = MIN(sizeof(buffer), info->fileSize - info->readPos);
        if (bytesRead >= sizeof(buffer)) {
            memcpy(buffer.b, info->fileBuffer, sizeof(buffer));

            info->fileBuffer += sizeof(buffer);
        }
    }
    else {
        bytesRead = fRead(buffer.b, 1, sizeof(int16), info->file);
    }

    if (info->encrypted)
        DecryptBytes(info, buffer.b, bytesRead);

    // if we're on a big endian machine, swap the byte order
    // this is done AFTER reading & decrypting since they expect little endian order on all systems
    if (CheckBigEndian()) {
        uint8 bytes[sizeof(buffer)];
        memcpy(bytes, &buffer, sizeof(buffer));

        int32 max = sizeof(buffer) - 1;
        for (int32 i = 0; i < sizeof(buffer) / 2; ++i) {
            uint8 store    = bytes[i];
            bytes[i]       = bytes[max - i];
            bytes[max - i] = store;
        }
        memcpy(&buffer, bytes, sizeof(buffer));
    }

    info->readPos += (int32)bytesRead;
    return buffer.result;
}

inline int32 ReadInt32(FileInfo *info, bool32 swapEndian)
{
    union {
        uint32 result;
        uint8 b[sizeof(result)];
    } buffer;
    memset(&buffer, 0, sizeof(buffer));

    size_t bytesRead = 0;

    if (info->usingFileBuffer) {
        bytesRead = MIN(sizeof(buffer), info->fileSize - info->readPos);
        if (bytesRead >= sizeof(buffer)) {
            memcpy(buffer.b, info->fileBuffer, sizeof(buffer));

            info->fileBuffer += sizeof(buffer);
        }
    }
    else {
        bytesRead = fRead(buffer.b, 1, sizeof(int32), info->file);
    }

    if (info->encrypted)
        DecryptBytes(info, buffer.b, bytesRead);

    if (swapEndian) {
        uint8 bytes[sizeof(buffer)];
        memcpy(bytes, &buffer, sizeof(buffer));

        int32 max = sizeof(buffer) - 1;
        for (int32 i = 0; i < sizeof(buffer) / 2; ++i) {
            uint8 store    = bytes[i];
            bytes[i]       = bytes[max - i];
            bytes[max - i] = store;
        }
        memcpy(&buffer, bytes, sizeof(buffer));
    }

    // if we're on a big endian machine, swap the byte order
    // this is done AFTER reading & decrypting since they expect little endian order on all systems
    if (CheckBigEndian()) {
        uint8 bytes[sizeof(buffer)];
        memcpy(bytes, &buffer, sizeof(buffer));

        int32 max = sizeof(buffer) - 1;
        for (int32 i = 0; i < sizeof(buffer) / 2; ++i) {
            uint8 store    = bytes[i];
            bytes[i]       = bytes[max - i];
            bytes[max - i] = store;
        }
        memcpy(&buffer, bytes, sizeof(buffer));
    }

    info->readPos += (int32)bytesRead;
    return buffer.result;
}
inline int64 ReadInt64(FileInfo *info)
{
    union {
        uint64 result;
        uint8 b[sizeof(result)];
    } buffer;
    memset(&buffer, 0, sizeof(buffer));

    size_t bytesRead = 0;

    if (info->usingFileBuffer) {
        bytesRead = MIN(sizeof(buffer), info->fileSize - info->readPos);
        if (bytesRead >= sizeof(buffer)) {
            memcpy(buffer.b, info->fileBuffer, sizeof(buffer));

            info->fileBuffer += sizeof(buffer);
        }
    }
    else {
        bytesRead = fRead(buffer.b, 1, sizeof(int64), info->file);
    }

    if (info->encrypted)
        DecryptBytes(info, buffer.b, bytesRead);

    // if we're on a big endian machine, swap the byte order
    // this is done AFTER reading & decrypting since they expect little endian order on all systems
    if (CheckBigEndian()) {
        uint8 bytes[sizeof(buffer)];
        memcpy(bytes, &buffer, sizeof(buffer));

        int32 max = sizeof(buffer) - 1;
        for (int32 i = 0; i < sizeof(buffer) / 2; ++i) {
            uint8 store    = bytes[i];
            bytes[i]       = bytes[max - i];
            bytes[max - i] = store;
        }
        memcpy(&buffer, bytes, sizeof(buffer));
    }

    info->readPos += (int32)bytesRead;
    return buffer.result;
}

inline float ReadSingle(FileInfo *info)
{
    union {
        float result;
        uint8 b[sizeof(result)];
    } buffer;
    memset(&buffer, 0, sizeof(buffer));

    size_t bytesRead = 0;

    if (info->usingFileBuffer) {
        bytesRead = MIN(sizeof(buffer), info->fileSize - info->readPos);
        if (bytesRead >= sizeof(buffer)) {
            memcpy(buffer.b, info->fileBuffer, sizeof(buffer));

            info->fileBuffer += sizeof(buffer);
        }
    }
    else {
        bytesRead = fRead(buffer.b, 1, sizeof(float), info->file);
    }

    if (info->encrypted)
        DecryptBytes(info, buffer.b, bytesRead);

    // if we're on a big endian machine, swap the byte order
    // this is done AFTER reading & decrypting since they expect little endian order on all systems
    if (CheckBigEndian()) {
        uint8 bytes[sizeof(buffer)];
        memcpy(bytes, &buffer, sizeof(buffer));

        int32 max = sizeof(buffer) - 1;
        for (int32 i = 0; i < sizeof(buffer) / 2; ++i) {
            uint8 store    = bytes[i];
            bytes[i]       = bytes[max - i];
            bytes[max - i] = store;
        }
        memcpy(&buffer, bytes, sizeof(buffer));
    }

    info->readPos += (int32)bytesRead;
    return buffer.result;
}

inline void ReadString(FileInfo *info, char *buffer)
{
    uint8 size = ReadInt8(info);
    ReadBytes(info, buffer, size);
    buffer[size] = 0;
}

inline int32 Uncompress(uint8 **cBuffer, int32 cSize, uint8 **buffer, int32 size)
{
    if (!buffer || !cBuffer)
        return 0;

    uLongf cLen    = cSize;
    uLongf destLen = size;

    int32 result = uncompress(*buffer, &destLen, *cBuffer, cLen);
    (void)result;

    return (int32)destLen;
}

// The buffer passed in parameter is allocated here, so it's up to the caller to free it once it goes unused
inline int32 ReadCompressed(FileInfo *info, uint8 **buffer)
{
    if (!buffer)
        return 0;

    uint32 cSize  = ReadInt32(info, false) - 4;
    uint32 sizeBE = ReadInt32(info, false);

    uint32 sizeLE = (uint32)((sizeBE << 24) | ((sizeBE << 8) & 0x00FF0000) | ((sizeBE >> 8) & 0x0000FF00) | (sizeBE >> 24));
    AllocateStorage((void **)buffer, sizeLE, DATASET_TMP, false);

    uint8 *cBuffer = NULL;
    AllocateStorage((void **)&cBuffer, cSize, DATASET_TMP, false);
    ReadBytes(info, cBuffer, cSize);

    uint32 newSize = Uncompress(&cBuffer, cSize, buffer, sizeLE);
    RemoveStorageEntry((void **)&cBuffer);

    return newSize;
}

inline void ClearDataFiles()
{
    // Unload file list
    for (int32 f = 0; f < DATAFILE_COUNT; ++f) {
        HASH_CLEAR_MD5(dataFileList[f].hash);
    }
}

} // namespace RSDK

#endif
