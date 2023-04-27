#include "RSDK/Core/RetroEngine.hpp"

using namespace RSDK;

RSDKFileInfo RSDK::dataFileList[DATAFILE_COUNT];
RSDKContainer RSDK::dataPacks[DATAPACK_COUNT];

uint8 RSDK::dataPackCount      = 0;
uint16 RSDK::dataFileListCount = 0;

char RSDK::gameLogicName[0x200];

bool32 RSDK::useDataPack = false;

#if RETRO_REV0U
void RSDK::DetectEngineVersion()
{
    bool32 readDataPack = useDataPack;
#if RETRO_USE_MOD_LOADER
    // mods can manually set their target engine versions if needed
    if (modSettings.versionOverride) {
        engine.version = modSettings.versionOverride;
        return;
    }

    // check if we have any mods with gameconfigs
    int32 m              = 0;
    int32 activeModCount = (int32)ActiveMods().size();
    for (m = 0; m < activeModCount; ++m) {
        SetActiveMod(m);

        FileInfo checkInfo;
        InitFileInfo(&checkInfo);
        if (LoadFile(&checkInfo, "Data/Game/GameConfig.bin", FMODE_RB)) {
            readDataPack = false;
            CloseFile(&checkInfo);
            m = 0; // found one, just sets this to 0 again
            break;
        }
    }

    if (m) // didn't find a gameconfig
        SetActiveMod(-1);
#endif

    FileInfo info;
    InitFileInfo(&info);
    if (!readDataPack) {
        if (LoadFile(&info, "Data/Game/GameConfig.bin", FMODE_RB)) {
#if RETRO_USE_MOD_LOADER
            SetActiveMod(-1);
#endif
            uint32 sig = ReadInt32(&info, false);

            // GameConfig has "CFG" signature, its RSDKv5 formatted
            if (sig == RSDK_SIGNATURE_CFG) {
                engine.version = 5;
            }
            else {
                // else, assume its RSDKv4 for now
                engine.version = 4;

                // Go back to the start of the file to check v3's "Data" string, that way we can tell if its v3 or v4
                Seek_Set(&info, 0);

                uint8 length = ReadInt8(&info);
                char buffer[0x40];
                ReadBytes(&info, buffer, length);

                // the "Data" thing is actually a string, but lets treat it as a "signature" for simplicity's sake shall we?
                length     = ReadInt8(&info);
                uint32 sig = ReadInt32(&info, false);
                if (sig == RSDK_SIGNATURE_DATA && length == 4)
                    engine.version = 3;
            }

            CloseFile(&info);
        }
    }
    else {
        info.externalFile = true;
        if (LoadFile(&info, dataPacks[dataPackCount - 1].name, FMODE_RB)) {
            uint32 sig = ReadInt32(&info, false);
            if (sig == RSDK_SIGNATURE_RSDK) {
                ReadInt8(&info); // 'v'
                uint8 version = ReadInt8(&info);

                switch (version) {
                    default: break;
                    case '3': engine.version = 3; break;
                    case '4': engine.version = 4; break;
                    case '5': engine.version = 5; break;
                }
            }
            else {
                // v3 has no 'RSDK' signature
                engine.version = 3;
            }

            CloseFile(&info);
        }
    }
}
#endif

bool32 RSDK::LoadDataPack(const char *filePath, size_t fileOffset, bool32 useBuffer)
{
    MEM_ZERO(dataPacks[dataPackCount]);
    useDataPack = false;
    FileInfo info;

    char dataPackPath[0x100];
    sprintf_s(dataPackPath, sizeof(dataPackPath), "%s%s", SKU::userFileDir, filePath);

    InitFileInfo(&info);
    info.externalFile = true;
    if (LoadFile(&info, dataPackPath, FMODE_RB)) {
        uint32 sig = ReadInt32(&info, false);
        if (sig != RSDK_SIGNATURE_RSDK)
            return false;

        useDataPack = true;

        ReadInt8(&info); // 'v'
        ReadInt8(&info); // version

        strcpy(dataPacks[dataPackCount].name, dataPackPath);

        dataPacks[dataPackCount].fileCount = ReadInt16(&info);
        for (int32 f = 0; f < dataPacks[dataPackCount].fileCount; ++f) {
            uint8 b[4];
            for (int32 y = 0; y < 4; y++) {
                ReadBytes(&info, b, 4);
                dataFileList[f].hash[y] = (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | (b[3] << 0);
            }

            dataFileList[f].offset = ReadInt32(&info, false);
            dataFileList[f].size   = ReadInt32(&info, false);

            dataFileList[f].encrypted = (dataFileList[f].size & 0x80000000) != 0;
            dataFileList[f].size &= 0x7FFFFFFF;
            dataFileList[f].useFileBuffer = useBuffer;
            dataFileList[f].packID        = dataPackCount;
        }

        dataPacks[dataPackCount].fileBuffer = NULL;
        if (useBuffer) {
            dataPacks[dataPackCount].fileBuffer = (uint8 *)malloc(info.fileSize);
            Seek_Set(&info, 0);
            ReadBytes(&info, dataPacks[dataPackCount].fileBuffer, info.fileSize);
        }

        dataFileListCount += dataPacks[dataPackCount].fileCount;
        dataPackCount++;

        CloseFile(&info);

        return true;
    }
#ifdef RETRO_EMBEDDED_DATA
    else if(true)
    {
        PrintLog(PRINT_NORMAL, "READER Data Embedded\n");
        
        HRSRC res_rsdk = FindResource(NULL, "DATA_RSDK", RT_RCDATA);
        HGLOBAL res_rsdk_handle = LoadResource(NULL, res_rsdk);
        uint8_t* res_rsdk_data = (uint8_t*)LockResource(res_rsdk_handle);
        uint32_t res_rsdk_size = SizeofResource(NULL, res_rsdk);
        
        PrintLog(PRINT_NORMAL, "READER Resource %u\n", res_rsdk_size);
        
        CloseFile(&info);
        InitFileInfo(&info);
        
        /*const uint32_t* res_rsdk_data_32 = (const uint32_t*)res_rsdk_data;
        mz_ulong size_og = res_rsdk_data_32[1];
        unsigned int size_z = res_rsdk_data_32[2];

        PrintLog(PRINT_NORMAL, "DATA_RSDK: %u %u", size_og, size_z);*/

        info.fileSize = res_rsdk_size;
        info.readPos = 0;
        info.usingFileBuffer = true;
        info.fileBuffer = (uint8_t*)calloc(res_rsdk_size, 1);
        memcpy(info.fileBuffer, res_rsdk_data, res_rsdk_size);
        
        /*info.fileSize = size_og;
        info.readPos = 0;
        info.usingFileBuffer = true;
        info.fileBuffer = (uint8_t*)calloc(size_og, 1);
        unsigned char* data_unc = (unsigned char*)calloc(size_og, 1); // Really weird hack trust me
        PrintLog(PRINT_NORMAL, "DATA_RSDK Alloc: %p", info.fileBuffer);
        int zstat = uncompress(info.fileBuffer, &size_og, &res_rsdk_data[12], size_og);
        memcpy(data_unc, info.fileBuffer, size_og);*/

        int32 sig = ReadInt32(&info, false);
        if(sig != RSDK_SIGNATURE_RSDK) return false;
        
        PrintLog(PRINT_NORMAL, "READER Sig %x\n", sig);

        useDataPack = true;

        int8_t v = ReadInt8(&info); // 'v'
        int8_t ver = ReadInt8(&info); // version
        
        PrintLog(PRINT_NORMAL, "READER Sig %c%d\n", v, ver);

        strcpy(dataPacks[dataPackCount].name, dataPackPath);

        dataPacks[dataPackCount].fileCount = ReadInt16(&info);
        for (int32 f = 0; f < dataPacks[dataPackCount].fileCount; ++f) {
            uint8 b[4];
            for (int32 y = 0; y < 4; y++) {
                ReadBytes(&info, b, 4);
                dataFileList[f].hash[y] = (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | (b[3] << 0);
            }

            dataFileList[f].offset = ReadInt32(&info, false);
            dataFileList[f].size   = ReadInt32(&info, false);

            dataFileList[f].encrypted = (dataFileList[f].size & 0x80000000) != 0;
            dataFileList[f].size &= 0x7FFFFFFF;
            dataFileList[f].useFileBuffer = useBuffer;
            dataFileList[f].packID        = dataPackCount;
        }

        dataPacks[dataPackCount].fileBuffer = NULL;
        if (useBuffer) {
            dataPacks[dataPackCount].fileBuffer = (uint8 *)malloc(info.fileSize);
            Seek_Set(&info, 0);
            //ReadBytes(&info, dataPacks[dataPackCount].fileBuffer, info.fileSize);
            memcpy(dataPacks[dataPackCount].fileBuffer, res_rsdk_data, res_rsdk_size);
            //free(data_unc);
        }

        dataFileListCount += dataPacks[dataPackCount].fileCount;
        dataPackCount++;

        free(info.fileBuffer);
        CloseFile(&info);

        return true;

    }
#endif
    else {
        useDataPack = false;
        return false;
    }
}

#if !RETRO_USE_ORIGINAL_CODE && RETRO_REV0U
inline bool ends_with(std::string const &value, std::string const &ending)
{
    if (ending.size() > value.size())
        return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}
#endif

bool32 RSDK::OpenDataFile(FileInfo *info, const char *filename)
{
    char hashBuffer[0x400];
    StringLowerCase(hashBuffer, filename);
    RETRO_HASH_MD5(hash);
    GEN_HASH_MD5_BUFFER(hashBuffer, hash);

    for (int32 f = 0; f < dataFileListCount; ++f) {
        RSDKFileInfo *file = &dataFileList[f];

        if (!HASH_MATCH_MD5(hash, file->hash))
            continue;

        info->usingFileBuffer = file->useFileBuffer;
        if (!file->useFileBuffer) {
            info->file = fOpen(dataPacks[file->packID].name, "rb");
            if (!info->file) {
                PrintLog(PRINT_NORMAL, "File not found (Unable to open datapack): %s", filename);
                return false;
            }

            fSeek(info->file, file->offset, SEEK_SET);
        }
        else {
            // a bit of a hack, but it is how it is in the original
            info->file = (FileIO *)&dataPacks[file->packID].fileBuffer[file->offset];

            uint8 *fileBuffer = (uint8 *)info->file;
            info->fileBuffer  = fileBuffer;
        }

        info->fileSize   = file->size;
        info->readPos    = 0;
        info->fileOffset = file->offset;
        info->encrypted  = file->encrypted;
        memset(info->encryptionKeyA, 0, 0x10 * sizeof(uint8));
        memset(info->encryptionKeyB, 0, 0x10 * sizeof(uint8));
        if (info->encrypted) {
            GenerateELoadKeys(info, filename, info->fileSize);
            info->eKeyNo      = (info->fileSize / 4) & 0x7F;
            info->eKeyPosA    = 0;
            info->eKeyPosB    = 8;
            info->eNybbleSwap = false;
        }

#if !RETRO_USE_ORIGINAL_CODE
        PrintLog(PRINT_NORMAL, "Loaded File %s", filename);
#endif
        return true;
    }

#if !RETRO_USE_ORIGINAL_CODE
    PrintLog(PRINT_NORMAL, "Data File not found: %s", filename);
#else
    PrintLog(PRINT_NORMAL, "File not found: %s", filename);
#endif
    return false;
}

bool32 RSDK::LoadFile(FileInfo *info, const char *filename, uint8 fileMode)
{
    if (info->file)
        return false;

    char fullFilePath[0x100];
    strcpy(fullFilePath, filename);

#if RETRO_USE_MOD_LOADER
    char pathLower[0x100];
    memset(pathLower, 0, sizeof(pathLower));
    for (int32 c = 0; c < strlen(filename); ++c) pathLower[c] = tolower(filename[c]);

    bool32 addPath = false;
    int32 m        = modSettings.activeMod != -1 ? modSettings.activeMod : 0;
    for (; m < modList.size(); ++m) {
        if (modList[m].active) {
            std::map<std::string, std::string>::const_iterator iter = modList[m].fileMap.find(pathLower);
            if (iter != modList[m].fileMap.cend()) {
                if (std::find(modList[m].excludedFiles.begin(), modList[m].excludedFiles.end(), pathLower) == modList[m].excludedFiles.end()) {
                    strcpy(fullFilePath, iter->second.c_str());
                    info->externalFile = true;
                    break;
                }
                else {
                    PrintLog(PRINT_NORMAL, "[MOD] Excluded File: %s", pathLower);
                }
            }
        }
    }

#if RETRO_REV0U
    if (modSettings.forceScripts && !info->externalFile) {
        if (std::string(fullFilePath).rfind("Data/Scripts/", 0) == 0 && ends_with(std::string(fullFilePath), "txt")) {
            // is a script, since those dont exist normally, load them from "scripts/"
            info->externalFile = true;
            addPath            = true;
            std::string fStr   = std::string(fullFilePath);
            fStr.erase(fStr.begin(), fStr.begin() + 5); // remove "Data/"
            StrCopy(fullFilePath, fStr.c_str());
        }
    }
#endif
#endif

#if RETRO_PLATFORM == RETRO_OSX || RETRO_PLATFORM == RETRO_ANDROID
    if (addPath) {
        char pathBuf[0x100];
        sprintf_s(pathBuf, sizeof(pathBuf), "%s%s", SKU::userFileDir, fullFilePath);
        sprintf_s(fullFilePath, sizeof(fullFilePath), "%s", pathBuf);
    }
#else
    (void)addPath; // unused
#endif

    if (!info->externalFile && fileMode == FMODE_RB && useDataPack) {
        return OpenDataFile(info, filename);
    }

    if (fileMode == FMODE_RB || fileMode == FMODE_WB || fileMode == FMODE_RB_PLUS) {
        info->file = fOpen(fullFilePath, openModes[fileMode - 1]);
    }

    if (!info->file) {
#if !RETRO_USE_ORIGINAL_CODE
        PrintLog(PRINT_NORMAL, "File not found: %s", fullFilePath);
#endif
        return false;
    }

    info->readPos  = 0;
    info->fileSize = 0;

    if (fileMode != FMODE_WB) {
        fSeek(info->file, 0, SEEK_END);
        info->fileSize = (int32)fTell(info->file);
        fSeek(info->file, 0, SEEK_SET);
    }
#if !RETRO_USE_ORIGINAL_CODE
    PrintLog(PRINT_NORMAL, "Loaded file %s", fullFilePath);
#endif
    return true;
}

void RSDK::GenerateELoadKeys(FileInfo *info, const char *key1, int32 key2)
{
    // This function splits hashes into bytes by casting their integers to byte arrays,
    // which only works as intended on little-endian CPUs.
#if !RETRO_USE_ORIGINAL_CODE
    RETRO_HASH_MD5(hash);
#else
    uint8 hash[0x10];
#endif
    char hashBuffer[0x400];

    // KeyA
    StringUpperCase(hashBuffer, key1);
#if !RETRO_USE_ORIGINAL_CODE
    GEN_HASH_MD5_BUFFER(hashBuffer, hash);

    for (int32 i = 0; i < 4; ++i)
        for (int32 j = 0; j < 4; ++j) info->encryptionKeyA[i * 4 + j] = (hash[i] >> (8 * (j ^ 3))) & 0xFF;
#else
    GEN_HASH_MD5_BUFFER(hashBuffer, (uint32 *)hash);

    for (int32 y = 0; y < 0x10; y += 4) {
        info->encryptionKeyA[y + 3] = hash[y + 0];
        info->encryptionKeyA[y + 2] = hash[y + 1];
        info->encryptionKeyA[y + 1] = hash[y + 2];
        info->encryptionKeyA[y + 0] = hash[y + 3];
    }
#endif

    // KeyB
    sprintf_s(hashBuffer, sizeof(hashBuffer), "%d", key2);
#if !RETRO_USE_ORIGINAL_CODE
    GEN_HASH_MD5_BUFFER(hashBuffer, hash);

    for (int32 i = 0; i < 4; ++i)
        for (int32 j = 0; j < 4; ++j) info->encryptionKeyB[i * 4 + j] = (hash[i] >> (8 * (j ^ 3))) & 0xFF;
#else
    GEN_HASH_MD5_BUFFER(hashBuffer, (uint32 *)hash);

    for (int32 y = 0; y < 0x10; y += 4) {
        info->encryptionKeyB[y + 3] = hash[y + 0];
        info->encryptionKeyB[y + 2] = hash[y + 1];
        info->encryptionKeyB[y + 1] = hash[y + 2];
        info->encryptionKeyB[y + 0] = hash[y + 3];
    }
#endif
}

void RSDK::DecryptBytes(FileInfo *info, void *buffer, size_t size)
{
    if (size) {
        uint8 *data = (uint8 *)buffer;
        while (size > 0) {
            *data ^= info->eKeyNo ^ info->encryptionKeyB[info->eKeyPosB];
            if (info->eNybbleSwap)
                *data = ((*data << 4) + (*data >> 4)) & 0xFF;
            *data ^= info->encryptionKeyA[info->eKeyPosA];

            info->eKeyPosA++;
            info->eKeyPosB++;

            if (info->eKeyPosA <= 15) {
                if (info->eKeyPosB > 12) {
                    info->eKeyPosB = 0;
                    info->eNybbleSwap ^= 1;
                }
            }
            else if (info->eKeyPosB <= 8) {
                info->eKeyPosA = 0;
                info->eNybbleSwap ^= 1;
            }
            else {
                info->eKeyNo += 2;
                info->eKeyNo &= 0x7F;

                if (info->eNybbleSwap) {
                    info->eNybbleSwap = false;

                    info->eKeyPosA = info->eKeyNo % 7;
                    info->eKeyPosB = (info->eKeyNo % 12) + 2;
                }
                else {
                    info->eNybbleSwap = true;

                    info->eKeyPosA = (info->eKeyNo % 12) + 3;
                    info->eKeyPosB = info->eKeyNo % 7;
                }
            }

            ++data;
            --size;
        }
    }
}
void RSDK::SkipBytes(FileInfo *info, int32 size)
{
    if (size) {
        while (size > 0) {
            info->eKeyPosA++;
            info->eKeyPosB++;

            if (info->eKeyPosA <= 15) {
                if (info->eKeyPosB > 12) {
                    info->eKeyPosB = 0;
                    info->eNybbleSwap ^= 1;
                }
            }
            else if (info->eKeyPosB <= 8) {
                info->eKeyPosA = 0;
                info->eNybbleSwap ^= 1;
            }
            else {
                info->eKeyNo += 2;
                info->eKeyNo &= 0x7F;

                if (info->eNybbleSwap) {
                    info->eNybbleSwap = false;

                    info->eKeyPosA = info->eKeyNo % 7;
                    info->eKeyPosB = (info->eKeyNo % 12) + 2;
                }
                else {
                    info->eNybbleSwap = true;

                    info->eKeyPosA = (info->eKeyNo % 12) + 3;
                    info->eKeyPosB = info->eKeyNo % 7;
                }
            }

            --size;
        }
    }
}
