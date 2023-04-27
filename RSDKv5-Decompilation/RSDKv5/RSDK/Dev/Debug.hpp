#ifndef DEBUG_H
#define DEBUG_H

// These being different sizes will surely cause issues if more then VIEWVAR_LIST_COUNT values are set
// But that's how the code is in the original so what can ya do
#define VIEWVAR_COUNT      (0x900)
#define VIEWVAR_LIST_COUNT (0x40)

#include <stdarg.h>

namespace RSDK
{

enum PrintModes {
    PRINT_NORMAL,
    PRINT_POPUP,
    PRINT_ERROR,
    PRINT_FATAL,
#if RETRO_REV0U
    PRINT_SCRIPTERR,
#endif
};

#if !RETRO_USE_ORIGINAL_CODE
enum TouchCornerButtons {
    CORNERBUTTON_START,
    CORNERBUTTON_LEFTRIGHT,
    CORNERBUTTON_SLIDER,
};
#endif

extern bool32 engineDebugMode;
extern bool32 useEndLine;
extern char outputString[0x400];

void PrintLog(int32 mode, const char *message, ...);

#if !RETRO_REV02
enum PrintMessageTypes {
    MESSAGE_STRING,
    MESSAGE_INT32,
    MESSAGE_UINT32,
    MESSAGE_FLOAT,
};

void PrintMessage(void *msg, uint8 type);
#endif

#if RETRO_REV02
inline void PrintText(int32 mode, const char *message) { PrintLog(mode, "%s", message); }
inline void PrintString(int32 mode, String *message)
{
    useEndLine = false;

    for (int32 c = 0; c < message->length; ++c) PrintLog(mode, "%c", message->chars[c]);
    PrintLog(mode, "\n");

    useEndLine = true;
}
inline void PrintUInt32(int32 mode, const char *message, uint32 integer) { PrintLog(mode, "%s: %u", message, integer); }
inline void PrintInt32(int32 mode, const char *message, int32 integer) { PrintLog(mode, "%s: %d", message, integer); }
inline void PrintFloat(int32 mode, const char *message, float f) { PrintLog(mode, "%s: %f", message, f); }
inline void PrintVector2(int32 mode, const char *message, Vector2 vec)
{
    PrintLog(mode, "%s: <%c%08X, %c%08X>", message, vec.x < 0 ? '-' : ' ', abs(vec.x), vec.y < 0 ? '-' : ' ', abs(vec.y));
}
inline void PrintHitbox(int32 mode, const char *message, Hitbox hitbox)
{
    PrintLog(mode, "%s: <l: %d, r: %d, t: %d, b: %d>", message, hitbox.left, hitbox.right, hitbox.top, hitbox.bottom);
}

struct ViewableVariable {
    char name[0x10];
    void *value;
    int32 type;
    int32 size;
    int32 min;
    int32 max;
};

typedef enum {
    VIEWVAR_INVALID,
    VIEWVAR_BOOL,
    VIEWVAR_UINT8,
    VIEWVAR_UINT16,
    VIEWVAR_UINT32,
    VIEWVAR_INT8,
    VIEWVAR_INT16,
    VIEWVAR_INT32,
} ViewableVarTypes;

typedef enum {
    VIEWVAR_DISPLAY_BOOL,
    VIEWVAR_DISPLAY_UNSIGNED,
    VIEWVAR_DISPLAY_SIGNED,
} DebugVarDisplayTypes;

extern int32 viewableVarCount;
extern ViewableVariable viewableVarList[VIEWVAR_LIST_COUNT];

inline void ClearViewableVariables() { viewableVarCount = 0; }
void AddViewableVariable(const char *name, void *value, int32 type, int32 min, int32 max);
#endif

struct DevMenu {
    void (*state)();
    int32 selection;
    int32 scrollPos;
    int32 timer;
    bool32 windowed;
    int8 sceneState;
    int8 listPos;
    int8 windowScale;
    int8 windowAspect;
#if RETRO_USE_MOD_LOADER
    bool32 modsChanged;
    uint8 startingVersion;
#endif
};

extern DevMenu devMenu;

void DevMenu_MainMenu();
void DevMenu_CategorySelectMenu();
void DevMenu_SceneSelectMenu();
void DevMenu_OptionsMenu();
void DevMenu_VideoOptionsMenu();
void DevMenu_AudioOptionsMenu();
void DevMenu_InputOptionsMenu();
void DevMenu_KeyMappingsMenu();
#if RETRO_REV02
void DevMenu_DebugOptionsMenu();
#endif
#if RETRO_USE_MOD_LOADER
void DevMenu_ModsMenu();
#endif

void OpenDevMenu();
void CloseDevMenu();

#endif

} // namespace RSDK