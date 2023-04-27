#ifndef LINK_H
#define LINK_H

namespace RSDK
{

#if RETRO_REV02
enum APITableIDs {
    APITable_GetUserLanguage,
    APITable_GetConfirmButtonFlip,
    APITable_ExitGame,
    APITable_LaunchManual,
#if RETRO_REV0U
    APITable_GetDefaultGamepadType,
#endif
    APITable_IsOverlayEnabled,
    APITable_CheckDLC,
#if RETRO_VER_EGS
    APITable_SetupExtensionOverlay,
    APITable_CanShowExtensionOverlay,
    APITable_ShowExtensionOverlay,
    APITable_CanShowAltExtensionOverlay,
    APITable_ShowAltExtensionOverlay,
    APITable_GetConnectingStringID,
    APITable_ShowLimitedVideoOptions,
#else
    APITable_ShowExtensionOverlay,
#endif
    APITable_UnlockAchievement,
    APITable_GetAchievementsEnabled,
    APITable_SetAchievementsEnabled,
#if RETRO_VER_EGS
    APITable_CheckAchievementsEnabled,
    APITable_SetAchievementNames,
#endif
    APITable_InitLeaderboards,
#if RETRO_VER_EGS
    APITable_CheckLeaderboardsEnabled,
#endif
    APITable_FetchLeaderboard,
    APITable_TrackScore,
    APITable_GetLeaderboardsStatus,
    APITable_LeaderboardEntryViewSize,
    APITable_LeaderboardEntryLoadSize,
    APITable_LoadLeaderboardEntries,
    APITable_ResetLeaderboardInfo,
    APITable_ReadLeaderboardEntry,
    APITable_SetPresence,
    APITable_TryTrackStat,
    APITable_GetStatsEnabled,
    APITable_SetStatsEnabled,
    APITable_ClearPrerollErrors,
    APITable_TryAuth,
    APITable_GetUserAuthStatus,
    APITable_GetUsername,
    APITable_TryInitStorage,
    APITable_GetUserStorageStatus,
    APITable_GetSaveStatus,
    APITable_ClearSaveStatus,
    APITable_SetSaveStatusContinue,
    APITable_SetSaveStatusOK,
    APITable_SetSaveStatusForbidden,
    APITable_SetSaveStatusError,
    APITable_SetUserStorageNoSave,
    APITable_GetUserStorageNoSave,
    APITable_LoadUserFile,
    APITable_SaveUserFile,
    APITable_DeleteUserFile,
    APITable_InitUserDB,
    APITable_OpenUserDB,
    APITable_SaveUserDB,
    APITable_ClearUserDB,
    APITable_ClearAllUserDBs,
    APITable_SetupUserDBRowSorting,
    APITable_GetUserDBRowsChanged,
    APITable_AddUserDBRowSortFilter,
    APITable_SortUserDBRows,
    APITable_GetSortedUserDBRowCount,
    APITable_GetSortedUserDBRowID,
    APITable_AddUserDBRow,
    APITable_SetUserDBValue,
    APITable_GetUserDBValue,
    APITable_GetRowUUID,
    APITable_GetUserDBRowByID,
    APITable_GetUserDBRowCreationTime,
    APITable_RemoveDBRow,
    APITable_RemoveAllDBRows,
    APITable_Count,
};
#else
#define APITABLE_COUNT (32)
#endif

enum FunctionTableIDs {
    FunctionTable_RegisterGlobalVariables,
    FunctionTable_RegisterObject,
#if RETRO_REV02
    FunctionTable_RegisterStaticVariables,
#endif
    FunctionTable_GetActiveEntities,
    FunctionTable_GetAllEntities,
    FunctionTable_BreakForeachLoop,
    FunctionTable_SetEditableVar,
    FunctionTable_GetEntity,
    FunctionTable_GetEntitySlot,
    FunctionTable_GetEntityCount,
    FunctionTable_GetDrawListRefSlot,
    FunctionTable_GetDrawListRef,
    FunctionTable_ResetEntity,
    FunctionTable_ResetEntitySlot,
    FunctionTable_CreateEntity,
    FunctionTable_CopyEntity,
    FunctionTable_CheckOnScreen,
    FunctionTable_CheckPosOnScreen,
    FunctionTable_AddDrawListRef,
    FunctionTable_SwapDrawListEntries,
    FunctionTable_SetDrawGroupProperties,
    FunctionTable_SetScene,
    FunctionTable_SetEngineState,
#if RETRO_REV02
    FunctionTable_ForceHardReset,
#endif
    FunctionTable_CheckValidScene,
    FunctionTable_CheckSceneFolder,
    FunctionTable_LoadScene,
    FunctionTable_FindObject,
    FunctionTable_ClearCameras,
    FunctionTable_AddCamera,
#if !RETRO_REV02
    FunctionTable_GetAPIFunction,
#endif
    FunctionTable_GetVideoSetting,
    FunctionTable_SetVideoSetting,
    FunctionTable_UpdateWindow,
    FunctionTable_Sin1024,
    FunctionTable_Cos1024,
    FunctionTable_ATan1024,
    FunctionTable_ASin1024,
    FunctionTable_ACos1024,
    FunctionTable_Sin512,
    FunctionTable_Cos512,
    FunctionTable_ATan512,
    FunctionTable_ASin512,
    FunctionTable_ACos512,
    FunctionTable_Sin256,
    FunctionTable_Cos256,
    FunctionTable_ATan256,
    FunctionTable_ASin256,
    FunctionTable_ACos256,
    FunctionTable_Rand,
    FunctionTable_RandSeeded,
    FunctionTable_SetRandSeed,
    FunctionTable_ATan2,
    FunctionTable_SetIdentityMatrix,
    FunctionTable_MatrixMultiply,
    FunctionTable_MatrixTranslateXYZ,
    FunctionTable_MatrixScaleXYZ,
    FunctionTable_MatrixRotateX,
    FunctionTable_MatrixRotateY,
    FunctionTable_MatrixRotateZ,
    FunctionTable_MatrixRotateXYZ,
    FunctionTable_MatrixInverse,
    FunctionTable_MatrixCopy,
    FunctionTable_InitString,
    FunctionTable_CopyString,
    FunctionTable_SetString,
    FunctionTable_AppendString,
    FunctionTable_AppendText,
    FunctionTable_LoadStringList,
    FunctionTable_SplitStringList,
    FunctionTable_GetCString,
    FunctionTable_CompareStrings,
    FunctionTable_GetDisplayInfo,
    FunctionTable_GetWindowSize,
    FunctionTable_SetScreenSize,
    FunctionTable_SetClipBounds,
#if RETRO_REV02
    FunctionTable_SetScreenVertices,
#endif
    FunctionTable_LoadSpriteSheet,
    FunctionTable_SetTintLookupTable,
    FunctionTable_SetPaletteMask,
    FunctionTable_SetPaletteEntry,
    FunctionTable_GetPaletteEntry,
    FunctionTable_SetActivePalette,
    FunctionTable_CopyPalette,
#if RETRO_REV02
    FunctionTable_LoadPalette,
#endif
    FunctionTable_RotatePalette,
    FunctionTable_SetLimitedFade,
#if RETRO_REV02
    FunctionTable_BlendColors,
#endif
    FunctionTable_DrawRect,
    FunctionTable_DrawLine,
    FunctionTable_DrawCircle,
    FunctionTable_DrawCircleOutline,
    FunctionTable_DrawFace,
    FunctionTable_DrawBlendedFace,
    FunctionTable_DrawSprite,
    FunctionTable_DrawDeformedSprite,
    FunctionTable_DrawString,
    FunctionTable_DrawTile,
    FunctionTable_CopyTile,
    FunctionTable_DrawAniTile,
#if RETRO_REV0U
    FunctionTable_DrawDynamicAniTile,
#endif
    FunctionTable_FillScreen,
    FunctionTable_LoadMesh,
    FunctionTable_Create3DScene,
    FunctionTable_Prepare3DScene,
    FunctionTable_SetDiffuseColor,
    FunctionTable_SetDiffuseIntensity,
    FunctionTable_SetSpecularIntensity,
    FunctionTable_AddModelToScene,
    FunctionTable_SetModelAnimation,
    FunctionTable_AddMeshFrameToScene,
    FunctionTable_Draw3DScene,
    FunctionTable_LoadSpriteAnimation,
    FunctionTable_CreateSpriteAnimation,
    FunctionTable_SetSpriteAnimation,
    FunctionTable_EditSpriteAnimation,
    FunctionTable_SetSpriteString,
    FunctionTable_FindSpriteAnimation,
    FunctionTable_GetFrame,
    FunctionTable_GetHitbox,
    FunctionTable_GetFrameID,
    FunctionTable_GetStringWidth,
    FunctionTable_ProcessAnimation,
    FunctionTable_GetTileLayerID,
    FunctionTable_GetTileLayer,
    FunctionTable_GetLayerSize,
    FunctionTable_GetTile,
    FunctionTable_SetTile,
    FunctionTable_CopyTileLayer,
    FunctionTable_ProcessParallax,
    FunctionTable_GetScanlines,
    FunctionTable_CheckObjectCollisionTouch,
    FunctionTable_CheckObjectCollisionCircle,
    FunctionTable_CheckObjectCollisionBox,
    FunctionTable_CheckObjectCollisionPlatform,
    FunctionTable_ObjectTileCollision,
    FunctionTable_ObjectTileGrip,
    FunctionTable_ProcessObjectMovement,
#if RETRO_REV0U
    FunctionTable_SetupCollisionConfig,
    FunctionTable_SetPathGripSensors,
    FunctionTable_FloorCollision,
    FunctionTable_LWallCollision,
    FunctionTable_RoofCollision,
    FunctionTable_RWallCollision,
    FunctionTable_FindFloorPosition,
    FunctionTable_FindLWallPosition,
    FunctionTable_FindRoofPosition,
    FunctionTable_FindRWallPosition,
#endif
    FunctionTable_GetTileAngle,
    FunctionTable_SetTileAngle,
    FunctionTable_GetTileFlags,
    FunctionTable_SetTileFlags,
#if RETRO_REV0U
    FunctionTable_CopyCollisionMask,
    FunctionTable_GetCollisionInfo,
#endif
    FunctionTable_GetSfx,
    FunctionTable_PlaySfx,
    FunctionTable_StopSfx,
#if RETRO_REV0U
    FunctionTable_StopAllSfx,
#endif
    FunctionTable_PlayMusic,
    FunctionTable_SetChannelAttributes,
    FunctionTable_StopChannel,
    FunctionTable_PauseChannel,
    FunctionTable_ResumeChannel,
    FunctionTable_SfxPlaying,
    FunctionTable_ChannelActive,
    FunctionTable_GetChannelPos,
    FunctionTable_LoadVideo,
    FunctionTable_LoadImage,
#if RETRO_REV02
    FunctionTable_GetInputDeviceID,
    FunctionTable_GetFilteredInputDeviceID,
    FunctionTable_GetInputDeviceType,
    FunctionTable_IsInputDeviceAssigned,
    FunctionTable_GetInputDeviceUnknown,
    FunctionTable_InputDeviceUnknown1,
    FunctionTable_InputDeviceUnknown2,
    FunctionTable_GetInputSlotUnknown,
    FunctionTable_InputSlotUnknown1,
    FunctionTable_InputSlotUnknown2,
    FunctionTable_AssignInputSlotToDevice,
    FunctionTable_IsInputSlotAssigned,
    FunctionTable_ResetInputSlotAssignments,
#endif
#if !RETRO_REV02
    FunctionTable_GetUnknownInputValue,
#endif
    FunctionTable_LoadUserFile,
    FunctionTable_SaveUserFile,
#if RETRO_REV02
    FunctionTable_PrintLog,
    FunctionTable_PrintText,
    FunctionTable_PrintString,
    FunctionTable_PrintUInt32,
    FunctionTable_PrintInt32,
    FunctionTable_PrintFloat,
    FunctionTable_PrintVector2,
    FunctionTable_PrintHitbox,
#endif
    FunctionTable_SetActiveVariable,
    FunctionTable_AddEnumVariable,
#if !RETRO_REV02
    FunctionTable_PrintMessage,
#endif
#if RETRO_REV02
    FunctionTable_ClearDebugValues,
    FunctionTable_SetDebugValue,
#endif
#if RETRO_REV0U
    FunctionTable_NotifyCallback,
    FunctionTable_SetGameFinished,
#endif
    FunctionTable_Count,
};

extern void *RSDKFunctionTable[FunctionTable_Count];
#if RETRO_REV02
extern void *APIFunctionTable[APITable_Count];
#else
struct APITableEntry {
    void *ptr;
    RETRO_HASH_MD5(hash);
};

extern APITableEntry APIFunctionTable[APITABLE_COUNT];
extern int32 APIFunctionTableCount;
#endif

#if RETRO_REV02
struct EngineInfo {
    void *functionTable;
    void *APITable;

    GameVersionInfo *gameInfo;
    SKU::SKUInfo *currentSKU;
    SceneInfo *sceneInfo;

    ControllerState *controller;
    AnalogState *stickL;
    AnalogState *stickR;
    TriggerState *triggerL;
    TriggerState *triggerR;
    TouchInfo *touchMouse;

    SKU::UnknownInfo *unknown;

    ScreenInfo *screenInfo;

#if RETRO_REV0U
    // only for origins, not technically needed for v5U if standalone I think
    void *hedgehogLink;
#endif

#if RETRO_USE_MOD_LOADER
    void *modTable;
#endif
};
#else
struct EngineInfo {
    void *functionTable;

    GameVersionInfo *gameInfo;
    SceneInfo *sceneInfo;

    ControllerState *controllerInfo;
    AnalogState *stickInfo;

    TouchInfo *touchInfo;

    ScreenInfo *screenInfo;

#if RETRO_USE_MOD_LOADER
    void *modTable;
#endif
};
#endif

void SetupFunctionTables();

#if RETRO_REV02
void LinkGameLogic(void *info);
#else
void LinkGameLogic(EngineInfo info);
#endif

// ORIGINAL CLASS

// Windows.h already included by master header
#if !(RETRO_PLATFORM == RETRO_WIN || RETRO_PLATFORM == RETRO_SWITCH)
#include <dlfcn.h>
#endif

// Only define this if you want to prioritize checking libraries first (Game_x64.dll then Game.dll)
// e.x. -DRETRO_ARCHIITECTURE="x64"
#ifndef RETRO_ARCHITECTURE
#define RETRO_ARCHITECTURE NULL
#endif

class Link
{
public:
#if RETRO_PLATFORM == RETRO_WIN
    typedef HMODULE Handle;
    // constexpr was added in C++11 this is safe don't kill me
    static constexpr const char *extention = ".dll";
    static constexpr const char *prefix    = NULL;
#elif RETRO_PLATFORM == RETRO_SWITCH
    typedef DynModule *Handle;
    static constexpr const char *extention = ".elf";
    static constexpr const char *prefix    = NULL;

    static Handle dlopen(const char *, int);
    static void *dlsym(Handle, const char *);
    static int dlclose(Handle);
    static char *dlerror();

    static constexpr const int RTLD_LOCAL = 0;
    static constexpr const int RTLD_LAZY  = 0;

private:
    static Result err;

public:
#else
    typedef void *Handle;
    static constexpr const char *prefix    = "lib";
#if RETRO_PLATFORM == RETRO_OSX
    static constexpr const char *extention = ".dylib";
#else
    static constexpr const char *extention = ".so";
#endif
#endif

    static inline Handle PlatformLoadLibrary(std::string path)
    {
        Handle ret;
#if RETRO_PLATFORM == RETRO_WIN
        ret = (Handle)LoadLibraryA(path.c_str());
#else
#if RETRO_PLATFORM == RETRO_ANDROID
        // path should only load local libs
        if (path.find_last_of('/') != std::string::npos)
            path = path.substr(path.find_last_of('/') + 1);
        path = "lib" + path;
#endif // ! RETRO_PLATFORM == ANDROID
        ret  = (Handle)dlopen(path.c_str(), RTLD_LOCAL | RTLD_LAZY);
#if RETRO_PLATFORM != RETRO_SWITCH
        // try loading the library globally on linux
        if (!ret) {
            if (path.find_last_of('/') != std::string::npos)
                path = path.substr(path.find_last_of('/') + 1);
            ret = (Handle)dlopen(path.c_str(), RTLD_LOCAL | RTLD_LAZY);
        }
#endif // ! RETRO_PLATFORM != SWITCH
#endif // ! RETRO_PLATFORM == WIN
        return ret;
    }

    static inline Handle Open(std::string path)
    {
        std::string original_path = path;

        // if it ends with extension
        if (path.length() >= strlen(extention) && 0 == path.compare(path.length() - strlen(extention), strlen(extention), extention)) {
            // remove it!
            path = path.substr(0, path.size() - strlen(extention));
        }

#if RETRO_ARCHITECTURE
        path += "_" RETRO_ARCHITECTURE;
#endif // ! RETRO_ARCHITECTURE

        // put it again!
        path += extention;

        Handle ret = NULL;
        if (prefix) {
            int32 last = path.find_last_of('/') + 1;
            if (last == std::string::npos + 1)
                ret = PlatformLoadLibrary(prefix + path);
            else
                ret = PlatformLoadLibrary(path.substr(0, last) + prefix + path.substr(last));
        }
        if (!ret)
            ret = PlatformLoadLibrary(path);

#if RETRO_ARCHITECTURE
        if (!ret) {
            if (prefix) {
                int32 last = original_path.find_last_of('/') + 1;
                if (last == std::string::npos + 1)
                    ret = PlatformLoadLibrary(prefix + original_path);
                else
                    ret = PlatformLoadLibrary(original_path.substr(0, last) + prefix + original_path.substr(last));
            }
            if (!ret)
                ret = PlatformLoadLibrary(original_path);
        }
#endif // ! RETRO_ARCHITECTURE
        return ret;
    }

    static inline void Close(Handle handle)
    {
        if (handle)
#if RETRO_PLATFORM == RETRO_WIN
            FreeLibrary(handle);
#else
            dlclose(handle);
#endif
    }

    static inline void *GetSymbol(Handle handle, const char *symbol)
    {
        if (!handle)
            return NULL;
#if RETRO_PLATFORM == RETRO_WIN
        return (void *)GetProcAddress(handle, symbol);
#else
        return (void *)dlsym(handle, symbol);
#endif
    }

    static inline char *GetError()
    {
#if RETRO_PLATFORM == RETRO_WIN
        return (char *)GetLastErrorAsString();
#else
        return dlerror();
#endif
    }

private:
#if RETRO_PLATFORM == RETRO_WIN
#if _MSC_VER
    // from here: https://stackoverflow.com/a/17387176
    // WINAPI sucks lol
    // Returns the last Win32 error, in string format. Returns an empty string if there is no error.
    static inline char *GetLastErrorAsString()
    {
        // Get the error message ID, if any.
        DWORD errorMessageID = ::GetLastError();
        if (errorMessageID == 0) {
            return (char *)""; // No error message has been recorded
        }

        LPSTR messageBuffer = nullptr;

        // Ask Win32 to give us the string version of that message ID.
        // The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message
        // string will be).
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                                     errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

        // Copy the error message into a std::string.
        std::string message(messageBuffer, size);

        // Free the Win32's string's buffer.
        LocalFree(messageBuffer);

        strcpy(textBuffer, message.c_str());
        return textBuffer;
    }
#else
    static inline char *GetLastErrorAsString() { return (char *)""; }
#endif
#endif
};

} // namespace RSDK

#endif
