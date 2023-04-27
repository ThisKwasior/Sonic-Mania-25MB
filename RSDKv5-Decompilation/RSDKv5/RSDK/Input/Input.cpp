#include "RSDK/Core/RetroEngine.hpp"

using namespace RSDK;

InputDevice *RSDK::inputDeviceList[INPUTDEVICE_COUNT];
int32 RSDK::inputDeviceCount = 0;

int32 RSDK::inputSlots[PLAYER_COUNT]              = { INPUT_NONE, INPUT_NONE, INPUT_NONE, INPUT_NONE };
InputDevice *RSDK::inputSlotDevices[PLAYER_COUNT] = { NULL, NULL, NULL, NULL };

ControllerState RSDK::controller[PLAYER_COUNT + 1];
AnalogState RSDK::stickL[PLAYER_COUNT + 1];
#if RETRO_REV02
AnalogState RSDK::stickR[PLAYER_COUNT + 1];
TriggerState RSDK::triggerL[PLAYER_COUNT + 1];
TriggerState RSDK::triggerR[PLAYER_COUNT + 1];
#endif
TouchInfo RSDK::touchInfo;

GamePadMappings *RSDK::gamePadMappings = NULL;
int32 RSDK::gamePadCount               = 0;

#if RETRO_INPUTDEVICE_KEYBOARD
#include "Keyboard/KBInputDevice.cpp"
#endif

#if RETRO_INPUTDEVICE_XINPUT
#include "XInput/XInputDevice.cpp"
#endif

#if RETRO_INPUTDEVICE_RAWINPUT
#include "RawInput/RawInputDevice.cpp"
#endif

#if RETRO_INPUTDEVICE_STEAM
#include "Steam/SteamInputDevice.cpp"
#endif

#if RETRO_INPUTDEVICE_NX
#include "NX/NXInputDevice.cpp"
#endif

#if RETRO_INPUTDEVICE_SDL2
#include "SDL2/SDL2InputDevice.cpp"
#endif

#if RETRO_INPUTDEVICE_GLFW
#include "GLFW/GLFWInputDevice.cpp"
#endif

#if RETRO_INPUTDEVICE_PDBOAT
#include "Paddleboat/PDBInputDevice.cpp"
#endif

void RSDK::RemoveInputDevice(InputDevice *targetDevice)
{
    if (targetDevice) {
        for (int32 d = 0; d < inputDeviceCount; ++d) {
            if (inputDeviceList[d] && inputDeviceList[d] == targetDevice) {
                uint32 deviceID = targetDevice->id;
                targetDevice->CloseDevice();
                inputDeviceCount--;

                delete inputDeviceList[d];
                inputDeviceList[d] = NULL;

                for (int32 id = d + 1; id <= inputDeviceCount && id < INPUTDEVICE_COUNT; ++id) inputDeviceList[id - 1] = inputDeviceList[id];
                // clear end device duplicate, prevents issues with new devices deleting stuff they shouldn't be
                if (inputDeviceCount < INPUTDEVICE_COUNT)
                    inputDeviceList[inputDeviceCount] = NULL;

                for (int32 id = 0; id < PLAYER_COUNT; ++id) {
                    if (inputSlots[id] == deviceID) {
#if !RETRO_REV02
                        inputSlots[id] = INPUT_NONE;
#endif
                        inputSlotDevices[id] = NULL;
                    }
                }

                for (int32 id = 0; id < PLAYER_COUNT; ++id) {
                    for (int32 c = 0; c < inputDeviceCount; ++c) {
                        if (inputDeviceList[c] && inputDeviceList[c]->id == inputSlots[id]) {
                            if (inputSlotDevices[id] != inputDeviceList[c])
                                inputSlotDevices[id] = inputDeviceList[c];
                        }
                    }
                }
            }
        }
    }
}

void RSDK::InitInputDevices()
{
#if !RETRO_USE_ORIGINAL_CODE
    // default the input slot state to "auto assign" rather than "none"
    // this fixes the "controller disconnected" popup since the engine handles the autoassign
    // without this, the engine has to wait for the game to tell the engine to start autoassignments
    for (int32 i = 0; i < PLAYER_COUNT; ++i) inputSlots[i] = INPUT_AUTOASSIGN;
#endif

#if RETRO_INPUTDEVICE_KEYBOARD
    SKU::InitKeyboardInputAPI();
#endif

#if RETRO_INPUTDEVICE_RAWINPUT
    SKU::InitHIDAPI();
#endif

#if RETRO_INPUTDEVICE_XINPUT
    SKU::InitXInputAPI();
#endif

#if RETRO_INPUTDEVICE_STEAM
    SKU::InitSteamInputAPI();
#endif

#if RETRO_INPUTDEVICE_NX
    SKU::InitNXInputAPI();
#endif

#if RETRO_INPUTDEVICE_SDL2
    SKU::InitSDL2InputAPI();
#endif

#if RETRO_INPUTDEVICE_GLFW
    SKU::InitGLFWInputAPI();
#endif

#if RETRO_INPUTDEVICE_PDBOAT
    SKU::InitPaddleboatInputAPI();
#endif
}

void RSDK::ReleaseInputDevices()
{
#if RETRO_INPUTDEVICE_SDL2
    SKU::ReleaseSDL2InputAPI();
#endif
}

void RSDK::ClearInput()
{
    for (int32 i = 0; i <= PLAYER_COUNT; ++i) {
        if (i != 0 && inputSlots[i - 1] == INPUT_UNASSIGNED)
            continue;

        controller[i].keyUp.press     = false;
        controller[i].keyDown.press   = false;
        controller[i].keyLeft.press   = false;
        controller[i].keyRight.press  = false;
        controller[i].keyA.press      = false;
        controller[i].keyB.press      = false;
        controller[i].keyC.press      = false;
        controller[i].keyX.press      = false;
        controller[i].keyY.press      = false;
        controller[i].keyZ.press      = false;
        controller[i].keyStart.press  = false;
        controller[i].keySelect.press = false;

        stickL[i].keyUp.press    = false;
        stickL[i].keyDown.press  = false;
        stickL[i].keyLeft.press  = false;
        stickL[i].keyRight.press = false;

#if RETRO_REV02
        stickL[i].keyStick.press = false;

        stickR[i].keyUp.press    = false;
        stickR[i].keyDown.press  = false;
        stickR[i].keyLeft.press  = false;
        stickR[i].keyRight.press = false;
        stickR[i].keyStick.press = false;

        triggerL[i].keyBumper.press  = false;
        triggerL[i].keyTrigger.press = false;

        triggerR[i].keyBumper.press  = false;
        triggerR[i].keyTrigger.press = false;
#else
        controller[i].keyStickL.press = false;
        controller[i].keyStickR.press = false;

        controller[i].keyBumperL.press  = false;
        controller[i].keyTriggerL.press = false;

        controller[i].keyBumperR.press  = false;
        controller[i].keyTriggerR.press = false;
#endif
    }
}

void RSDK::ProcessInput()
{
    ClearInput();

    bool32 anyPress = false;
    for (int32 i = 0; i < inputDeviceCount; ++i) {
        if (inputDeviceList[i]) {
            inputDeviceList[i]->UpdateInput();

            anyPress |= inputDeviceList[i]->anyPress;
        }
    }

#if RETRO_REV02
    if (anyPress || touchInfo.count)
        videoSettings.dimTimer = 0;
    else if (videoSettings.dimTimer < videoSettings.dimLimit)
        ++videoSettings.dimTimer;
#endif

    for (int32 i = 0; i < PLAYER_COUNT; ++i) {
        int32 assign = inputSlots[i];
        if (assign && assign != INPUT_UNASSIGNED) {
            if (assign == INPUT_AUTOASSIGN) {
                int32 id      = GetAvaliableInputDevice();
                inputSlots[i] = id;
                if (id != INPUT_AUTOASSIGN)
                    AssignInputSlotToDevice(CONT_P1 + i, id);
            }
            else {
                InputDevice *device = inputSlotDevices[i];
                if (device && device->id == assign && device->active)
                    device->ProcessInput(CONT_P1 + i);
            }
        }
    }

#if !RETRO_REV02 && RETRO_INPUTDEVICE_KEYBOARD
    RSDK::SKU::HandleSpecialKeys();
#endif

    for (int32 c = 0; c <= PLAYER_COUNT; ++c) {
        if (c <= 0 || inputSlots[c - 1] != INPUT_UNASSIGNED) {
            InputState *cont[] = {
                &controller[c].keyUp, &controller[c].keyDown, &controller[c].keyLeft,  &controller[c].keyRight,
                &controller[c].keyA,  &controller[c].keyB,    &controller[c].keyC,     &controller[c].keyX,
                &controller[c].keyY,  &controller[c].keyZ,    &controller[c].keyStart, &controller[c].keySelect,
            };

#if RETRO_REV02
            InputState *lStick[] = { &stickL[c].keyUp, &stickL[c].keyDown, &stickL[c].keyLeft, &stickL[c].keyRight, &stickL[c].keyStick };
            InputState *rStick[] = { &stickR[c].keyUp, &stickR[c].keyDown, &stickR[c].keyLeft, &stickR[c].keyRight, &stickR[c].keyStick };

            InputState *lTrigger[] = { &triggerL[c].keyBumper, &triggerL[c].keyTrigger };
            InputState *rTrigger[] = { &triggerR[c].keyBumper, &triggerR[c].keyTrigger };
#else
            InputState *lStick[] = { &stickL[c].keyUp, &stickL[c].keyDown, &stickL[c].keyLeft, &stickL[c].keyRight, &controller[c].keyStickL };
            InputState *rStick[] = { NULL, NULL, NULL, NULL, &controller[c].keyStickR };

            InputState *lTrigger[] = { &controller[c].keyBumperL, &controller[c].keyTriggerL };
            InputState *rTrigger[] = { &controller[c].keyBumperR, &controller[c].keyTriggerR };
#endif

            for (int32 i = 0; i < 12; ++i) {
                if (cont[i]->press) {
                    if (cont[i]->down)
                        cont[i]->press = false;
                    else
                        cont[i]->down = true;
                }
                else
                    cont[i]->down = false;
            }

            for (int32 i = 0; i < 5; ++i) {
                if (lStick[i]->press) {
                    if (lStick[i]->down)
                        lStick[i]->press = false;
                    else
                        lStick[i]->down = true;
                }
                else
                    lStick[i]->down = false;

                if (rStick[i]) {
                    if (rStick[i]->press) {
                        if (rStick[i]->down)
                            rStick[i]->press = false;
                        else
                            rStick[i]->down = true;
                    }
                    else
                        rStick[i]->down = false;
                }
            }

            for (int32 i = 0; i < 2; ++i) {
                if (lTrigger[i]->press) {
                    if (lTrigger[i]->down)
                        lTrigger[i]->press = false;
                    else
                        lTrigger[i]->down = true;
                }
                else
                    lTrigger[i]->down = false;

                if (rTrigger[i]->press) {
                    if (rTrigger[i]->down)
                        rTrigger[i]->press = false;
                    else
                        rTrigger[i]->down = true;
                }
                else
                    rTrigger[i]->down = false;
            }
        }
    }
}

void RSDK::ProcessInputDevices()
{
#if RETRO_INPUTDEVICE_NX
    SKU::ProcessNXInputDevices();
#endif
#if RETRO_INPUTDEVICE_PDBOAT
    SKU::ProcessPaddleboatInputDevices();
#endif
}

int32 RSDK::GetInputDeviceType(uint32 deviceID)
{
    for (int32 i = 0; i < inputDeviceCount; ++i) {
        if (inputDeviceList[i] && inputDeviceList[i]->id == deviceID)
            return inputDeviceList[i]->gamepadType;
    }

#if RETRO_REV02
    return SKU::userCore->GetDefaultGamepadType();
#else
    int32 platform = gameVerInfo.platform;

    switch (platform) {

#if RETRO_INPUTDEVICE_NX
        return currentNXControllerType;
#else
        return (DEVICE_API_NONE << 16) | (DEVICE_TYPE_CONTROLLER << 8) | (DEVICE_SWITCH_HANDHELD << 0);
#endif

        default:
        case PLATFORM_PS4:
        case PLATFORM_XB1:
        case PLATFORM_PC:
        case PLATFORM_DEV: return (DEVICE_API_NONE << 16) | (DEVICE_TYPE_CONTROLLER << 8) | (0 << 0); break;
    }
#endif
}