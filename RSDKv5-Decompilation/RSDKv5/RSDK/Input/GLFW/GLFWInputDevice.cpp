#ifdef STFU_INTELLISENSE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "GLFWInputDevice.hpp"
#endif

using namespace RSDK;

void RSDK::SKU::InputDeviceGLFW::UpdateInput()
{
    currentState ^= 1;
    glfwGetGamepadState(jid, &states[currentState]);
    bool32 changedButtons = (bool32)(memcmp(&states[currentState], &states[currentState ^ 1], sizeof(GLFWgamepadstate)));

    if (changedButtons) {
        this->inactiveTimer[0] = 0;
        this->anyPress         = true;
    }
    else {
        ++this->inactiveTimer[0];
        this->anyPress = false;
    }

    if (changedButtons
        && ((states[currentState].buttons[GLFW_GAMEPAD_BUTTON_A] != states[currentState ^ 1].buttons[GLFW_GAMEPAD_BUTTON_A])
            || states[currentState].buttons[GLFW_GAMEPAD_BUTTON_START] != states[currentState ^ 1].buttons[GLFW_GAMEPAD_BUTTON_START])) {
        this->inactiveTimer[1] = 0;
    }
    else
        ++this->inactiveTimer[1];

    ProcessInput(CONT_ANY);
}

void RSDK::SKU::InputDeviceGLFW::ProcessInput(int32 controllerID)
{
    controller[controllerID].keyUp.press |= this->states[currentState].buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP];
    controller[controllerID].keyDown.press |= this->states[currentState].buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN];
    controller[controllerID].keyLeft.press |= this->states[currentState].buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];
    controller[controllerID].keyRight.press |= this->states[currentState].buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];
    controller[controllerID].keyA.press |= this->states[currentState].buttons[swapABXY ? GLFW_GAMEPAD_BUTTON_B : GLFW_GAMEPAD_BUTTON_A];
    controller[controllerID].keyB.press |= this->states[currentState].buttons[swapABXY ? GLFW_GAMEPAD_BUTTON_A : GLFW_GAMEPAD_BUTTON_B];
    // controller[controllerID].keyC.press |= this->states[currentState].buttons[GLFW_GAMEPAD_BUTTON_C];
    controller[controllerID].keyX.press |= this->states[currentState].buttons[swapABXY ? GLFW_GAMEPAD_BUTTON_Y : GLFW_GAMEPAD_BUTTON_X];
    controller[controllerID].keyY.press |= this->states[currentState].buttons[swapABXY ? GLFW_GAMEPAD_BUTTON_X : GLFW_GAMEPAD_BUTTON_Y];
    // controller[controllerID].keyZ.press |= this->states[currentState].buttons[GLFW_GAMEPAD_BUTTON_Z];
    controller[controllerID].keyStart.press |= this->states[currentState].buttons[GLFW_GAMEPAD_BUTTON_START];
    controller[controllerID].keySelect.press |= this->states[currentState].buttons[GLFW_GAMEPAD_BUTTON_BACK];

#if RETRO_REV02
    stickL[controllerID].keyStick.press |= this->states[currentState].buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB];
    stickL[controllerID].hDelta = this->states[currentState].axes[GLFW_GAMEPAD_AXIS_LEFT_X];
    stickL[controllerID].vDelta = -this->states[currentState].axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
    stickL[controllerID].keyUp.press |= stickL[controllerID].vDelta > INPUT_DEADZONE;
    stickL[controllerID].keyDown.press |= stickL[controllerID].vDelta < -INPUT_DEADZONE;
    stickL[controllerID].keyLeft.press |= stickL[controllerID].hDelta < -INPUT_DEADZONE;
    stickL[controllerID].keyRight.press |= stickL[controllerID].hDelta > INPUT_DEADZONE;

    stickR[controllerID].keyStick.press |= this->states[currentState].buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB];
    stickR[controllerID].hDelta = this->states[currentState].axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
    stickR[controllerID].vDelta = this->states[currentState].axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
    stickR[controllerID].keyUp.press |= stickR[controllerID].vDelta > INPUT_DEADZONE;
    stickR[controllerID].keyDown.press |= stickR[controllerID].vDelta < -INPUT_DEADZONE;
    stickR[controllerID].keyLeft.press |= stickR[controllerID].hDelta < -INPUT_DEADZONE;
    stickR[controllerID].keyRight.press |= stickR[controllerID].hDelta > INPUT_DEADZONE;

    triggerL[controllerID].keyBumper.press |= this->states[currentState].buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER];
    triggerL[controllerID].keyTrigger.press |= this->states[currentState].axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] > INPUT_DEADZONE;
    triggerL[controllerID].bumperDelta  = triggerL[controllerID].keyBumper.press ? 1.0f : 0.0f;
    triggerL[controllerID].triggerDelta = this->states[currentState].axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];

    triggerR[controllerID].keyBumper.press |= this->states[currentState].buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER];
    triggerR[controllerID].keyTrigger.press |= this->states[currentState].axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > INPUT_DEADZONE;
    triggerR[controllerID].bumperDelta  = triggerR[controllerID].keyBumper.press ? 1.0f : 0.0f;
    triggerR[controllerID].triggerDelta = this->states[currentState].axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
#else
    controller[controllerID].keyStickL.press |= this->states[currentState].buttons[GLFW_GAMEPAD_BUTTON_LEFT_THUMB];
    stickL[controllerID].hDeltaL = this->states[currentState].axes[GLFW_GAMEPAD_AXIS_LEFT_X];
    stickL[controllerID].vDeltaL = -this->states[currentState].axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
    stickL[controllerID].keyUp.press |= stickL[controllerID].vDeltaL > INPUT_DEADZONE;
    stickL[controllerID].keyDown.press |= stickL[controllerID].vDeltaL < -INPUT_DEADZONE;
    stickL[controllerID].keyLeft.press |= stickL[controllerID].hDeltaL < -INPUT_DEADZONE;
    stickL[controllerID].keyRight.press |= stickL[controllerID].hDeltaL > INPUT_DEADZONE;

    controller[controllerID].keyStickR.press |= this->states[currentState].buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB];
    stickL[controllerID].hDeltaR = this->states[currentState].axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
    stickL[controllerID].vDeltaR = this->states[currentState].axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];

    controller[controllerID].keyBumperL.press |= this->states[currentState].buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER];
    controller[controllerID].keyTriggerL.press |= this->states[currentState].axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] > INPUT_DEADZONE;
    stickL[controllerID].triggerDeltaL = this->states[currentState].axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER];

    controller[controllerID].keyBumperR.press |= this->states[currentState].buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER];
    controller[controllerID].keyTriggerR.press |= this->states[currentState].axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > INPUT_DEADZONE;
    stickL[controllerID].triggerDeltaR = this->states[currentState].axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER];
#endif
}

void RSDK::SKU::InputDeviceGLFW::CloseDevice()
{
    this->active     = false;
    this->isAssigned = false;
    this->jid        = GLFW_JOYSTICK_LAST;
}

RSDK::SKU::InputDeviceGLFW *RSDK::SKU::InitGLFWInputDevice(uint32 id, uint8 controllerID)
{
    if (inputDeviceCount >= INPUTDEVICE_COUNT)
        return NULL;

    if (inputDeviceList[inputDeviceCount] && inputDeviceList[inputDeviceCount]->active)
        return NULL;

    if (inputDeviceList[inputDeviceCount])
        delete inputDeviceList[inputDeviceCount];

    inputDeviceList[inputDeviceCount] = new InputDeviceGLFW();

    InputDeviceGLFW *device = (InputDeviceGLFW *)inputDeviceList[inputDeviceCount];

    device->jid = controllerID;

    const char *name = glfwGetGamepadName(controllerID);

    device->swapABXY     = false;
    uint8 controllerType = DEVICE_XBOX;

    if (strstr(name, "Xbox"))
        controllerType = DEVICE_XBOX;
    else if (strstr(name, "PS4") || strstr(name, "PS5"))
        controllerType = DEVICE_PS4;
    else if (strstr(name, "Nintendo") || strstr(name, "Switch")) {
        controllerType   = DEVICE_SWITCH_PRO;
        device->swapABXY = true;
    }
    else if (strstr(name, "Saturn"))
        controllerType = DEVICE_SATURN;

    device->active      = true;
    device->disabled    = false;
    device->gamepadType = (DEVICE_API_GLFW << 16) | (DEVICE_TYPE_CONTROLLER << 8) | (controllerType << 0);
    device->id          = id;

    for (int32 i = 0; i < PLAYER_COUNT; ++i) {
        if (inputSlots[i] == id) {
            inputSlotDevices[i] = device;
            device->isAssigned  = true;
        }
    }

    inputDeviceCount++;
    return device;
}

void RSDK::SKU::InitGLFWInputAPI()
{
    char buffer[0x100];
#if RETRO_PLATFORM == RETRO_SWITCH
    glfwUpdateGamepadMappings("53776974636820436f6e74726f6c6c65,Switch "
                              "Controller,a:b0,b:b1,back:b11,dpdown:b15,dpleft:b12,dpright:b14,dpup:b13,leftshoulder:b6,leftstick:b4,lefttrigger:"
                              "b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b5,righttrigger:b9,rightx:a2,righty:a3,start:b10,x:b3,y:b2,");
    // Support for extra controller types SDL doesn't recognise
#endif // ! RETRO_PLATFORM == RETRO_SWITCH

    FileInfo f;
    InitFileInfo(&f);
    f.externalFile = true;
    char path[0x100]; 
    sprintf_s(path, sizeof(path), "%sgamecontrollerdb.txt", SKU::userFileDir);

    if (LoadFile(&f, path, FMODE_RB)) {
        char* buf;
        AllocateStorage((void**)&buf, f.fileSize + 1, DATASET_TMP, false);
        buf[f.fileSize] = 0;
        ReadBytes(&f, buf, f.fileSize);
        RSDK::PrintLog(PRINT_NORMAL, "[GLFW] gamecontrollerdb size %d: %s", f.fileSize, buf);

        if (glfwUpdateGamepadMappings(buf) == GLFW_FALSE)
            RSDK::PrintLog(PRINT_NORMAL, "[GLFW] failed to load from gamecontrollerdb");

        RemoveStorageEntry((void**)&buf);
        CloseFile(&f);
    }

    for (int32 i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_LAST; ++i) {
        RSDK::PrintLog(PRINT_NORMAL, "[GLFW] joystick %d is '%s', isGamepad %d", i, glfwGetJoystickName(i), glfwJoystickIsGamepad(i));
        if (glfwJoystickIsGamepad(i)) {
            uint32 hash;
            char idBuffer[0x20];
            sprintf_s(idBuffer, sizeof(idBuffer), "%s%d", "GLFWDevice", i);
            GenerateHashCRC(&hash, idBuffer);

            SKU::InitGLFWInputDevice(hash, i);
        }
    }
}