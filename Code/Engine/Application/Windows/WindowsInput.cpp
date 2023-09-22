/**
* WindowsInput.cpp
* 21.09.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Application/Windows/WindowsInput.h"
#include "Core/LCException.h"
#include "Core/LCUtils.h"

#include <dinputd.h>


const static int UpDirValue = 0;
const static int UpRightDirValue = 4500;
const static int RightDirValue = 9000;
const static int RightDownDirValue = 13500;
const static int DownDirValue = 18000;
const static int DownLeftDirValue = 22500;
const static int LeftDirValue = 27000;
const static int LeftUpDirValue = 31500;


LcDirectInputSystem::LcDirectInputSystem()
{
    if (FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED)))
    {
        throw std::exception("LcDirectInputSystem(): Cannot initialize");
    }
}

LcDirectInputSystem::~LcDirectInputSystem()
{
    Shutdown();

    CoUninitialize();
}

void LcDirectInputSystem::Init(struct LcAppContext& context)
{
    if (FAILED(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)directInput.GetAddressOf(), nullptr)))
    {
        throw std::exception("LcDirectInputSystem::Init(): Cannot create DirectInput");
    }

    ComPtr<IDirectInputJoyConfig8> joyConfig;
    if (FAILED(directInput->QueryInterface(IID_IDirectInputJoyConfig8, (void**)&joyConfig)))
    {
        throw std::exception("LcDirectInputSystem::Init(): Cannot get config");
    }

    struct DI_ENUM_CONTEXT
    {
        LPDIRECTINPUT8 directInput;
        TInputDevicesList* devices;
        HWND windowHandle;
        int id;
    };

    DI_ENUM_CONTEXT enumContext{};
    enumContext.directInput = directInput.Get();
    enumContext.devices = &devices;
    enumContext.windowHandle = (HWND)context.windowHandle;
    enumContext.id = 0;

    auto onNextDevice = [](LPCDIDEVICEINSTANCEW deviceInstance, LPVOID contextPtr) -> BOOL
    {
        DI_ENUM_CONTEXT* context = reinterpret_cast<DI_ENUM_CONTEXT*>(contextPtr);
        auto newJoystick = std::make_shared<LcDirectInputJoystick>((WCHAR*)deviceInstance->tszInstanceName, context->id);
        newJoystick->Deactivate();

        auto& device = newJoystick->GetDevice();
        if (FAILED(context->directInput->CreateDevice(deviceInstance->guidInstance, device.GetAddressOf(), nullptr)))
        {
            return DIENUM_CONTINUE;
        }

        if (FAILED(device.Get()->SetDataFormat(&c_dfDIJoystick2)))
        {
            return DIENUM_CONTINUE;
        }

        if (FAILED(device.Get()->SetCooperativeLevel(context->windowHandle, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND)))
        {
            return DIENUM_CONTINUE;
        }

        context->devices->push_back(newJoystick);
        context->id++;

        return DIENUM_CONTINUE;
    };

    if (FAILED(directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, onNextDevice, &enumContext, DIEDFL_ATTACHEDONLY)))
    {
        throw std::exception("LcDirectInputSystem::Init(): Cannot enumerate devices");
    }
}

void LcDirectInputSystem::Shutdown()
{
    activeDevice = nullptr;
    devices.clear();
    directInput.Reset();
}

void LcDirectInputSystem::Update(float deltaSeconds, struct LcAppContext& context)
{
    if (!keysHandler) return;

    LC_TRY

    for (auto& device : devices)
    {
        if (device->GetType() != LcInputDeviceType::Keyboard)
        {
            auto joystick = static_cast<LcDirectInputJoystick*>(device.get());
            if (!joystick->IsActive()) continue;

            auto device = joystick->GetDevice();
            if (FAILED(device->Poll()))
            {
                device->Acquire();
                continue;
            }

            DIJOYSTATE2 newState{};
            if (FAILED(device->GetDeviceState(sizeof(DIJOYSTATE2), &newState)))
            {
                continue;
            }

            KEYS prevState, curState;
            joystick->GetButtonsState(prevState.Get());

            UINT prevArrows = -1, curArrows = newState.rgdwPOV[0];
            joystick->GetArrowsState(prevArrows);

            // check buttons changes
            const int keysWithoutArrows = (int)LcJKeys::StartArrows - LC_JOYSTICK_KEYS_OFFSET;
            for (int key = 0; key < keysWithoutArrows; key++)
            {
                BYTE prev = prevState[LC_JOYSTICK_KEYS_OFFSET + key];
                BYTE cur = (newState.rgbButtons[key] == 0) ? 0 : 1;
                if (cur != prev)
                {
                    auto action = (cur == 0) ? LcKeyState::Up : LcKeyState::Down;
                    keysHandler(LC_JOYSTICK_KEYS_OFFSET + key, action, context.app);
                }

                curState[LC_JOYSTICK_KEYS_OFFSET + key] = cur;
            }

            // check arrows changes
            if (prevArrows != curArrows)
            {
                switch (curArrows)
                {
                    case LeftDirValue: curState[(int)LcJKeys::Left] = 1; break;
                    case RightDirValue: curState[(int)LcJKeys::Right] = 1; break;
                    case UpDirValue: curState[(int)LcJKeys::Up] = 1; break;
                    case DownDirValue: curState[(int)LcJKeys::Down] = 1; break;
                    case LeftUpDirValue: curState[(int)LcJKeys::Left] = curState[(int)LcJKeys::Up] = 1; break;
                    case UpRightDirValue: curState[(int)LcJKeys::Right] = curState[(int)LcJKeys::Up] = 1; break;
                    case DownLeftDirValue: curState[(int)LcJKeys::Left] = curState[(int)LcJKeys::Down] = 1; break;
                    case RightDownDirValue: curState[(int)LcJKeys::Right] = curState[(int)LcJKeys::Down] = 1; break;
                }

                for (int i = (int)LcJKeys::StartArrows; i <= (int)LcJKeys::EndArrows; i++)
                {
                    BYTE prev = prevState[i];
                    BYTE cur = curState[i];
                    if (cur != prev)
                    {
                        auto action = (cur == 0) ? LcKeyState::Up : LcKeyState::Down;
                        keysHandler(i, action, context.app);
                    }
                }
            }
            else
            {
                for (int i = (int)LcJKeys::StartArrows; i <= (int)LcJKeys::EndArrows; i++)
                {
                    curState[i] = prevState[i];
                }
            }

            joystick->SetButtonsState(curState.Get());
            joystick->SetArrowsState(curArrows);
        }
    }

    LC_CATCH{ LC_THROW("LcDirectInputSystem::Update()") }
}


LcDirectInputJoystick::LcDirectInputJoystick(const std::wstring& inName, int inDeviceId)
{
    name = inName;
    deviceId = inDeviceId;
    arrows = -1;
}

void LcDirectInputJoystick::SetButtonsState(const BYTE* inKeys)
{
    memcpy(keys.Get(), inKeys, sizeof(keys));
}

void LcDirectInputJoystick::GetButtonsState(BYTE* inKeys) const
{
    memcpy(inKeys, keys.Get(), sizeof(keys));
}

void LcDirectInputJoystick::Activate()
{
    LcKeyboard::Activate();

    if (device) device->Acquire();
}

void LcDirectInputJoystick::Deactivate()
{
    LcKeyboard::Deactivate();

    if (device) device->Unacquire();
}

LcInputDeviceType LcDirectInputJoystick::GetType() const
{
    return static_cast<LcInputDeviceType>((int)LcInputDeviceType::Joystick1 + deviceId);
}


LcWindowsInputSystem::LcWindowsInputSystem() : activeDevice(nullptr)
{
    devices.push_back(std::make_shared<LcKeyboard>());
    activeDevice = devices[0].get();
}

void LcWindowsInputSystem::SetActiveDevice(const IInputDevice* inActiveDevice)
{
    for (auto& device : devices)
    {
        if (device.get() == inActiveDevice)
        {
            activeDevice = device.get();
            activeDevice->Activate();
        }
        else
        {
            device->Deactivate();
        }
    }
}

TInputSystemPtr GetWindowsInputSystem()
{
    return std::make_unique<LcWindowsInputSystem>();
}

TInputSystemPtr GetDirectInputSystem()
{
    return std::make_unique<LcDirectInputSystem>();
}
