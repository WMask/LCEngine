/**
* WindowsInput.cpp
* 21.09.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Application/Windows/WindowsInput.h"

#include <dinputd.h>


struct DI_ENUM_CONTEXT
{
    DIJOYCONFIG* preferredJoyCfg;
    bool preferredJoyCfgValid;
};

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

    DIJOYCONFIG preferredJoyCfg{};
    preferredJoyCfg.dwSize = sizeof(preferredJoyCfg);

    DI_ENUM_CONTEXT enumContext{};
    enumContext.preferredJoyCfg = &preferredJoyCfg;

    int joyId = 0;
    while (!enumContext.preferredJoyCfgValid && joyId < LC_JOYSTICK_MAX_COUNT)
    {
        if (SUCCEEDED(joyConfig->GetConfig(joyId, &preferredJoyCfg, DIJC_GUIDINSTANCE)))
        {
            enumContext.preferredJoyCfgValid = true;
        }

        joyId++;
    }

    if (enumContext.preferredJoyCfgValid)
    {
        EnumerateDevices();
    }
}

void LcDirectInputSystem::Shutdown()
{
    directInput.Reset();
}

void LcDirectInputSystem::Update(float deltaSeconds, struct LcAppContext& context)
{

}

void LcDirectInputSystem::EnumerateDevices()
{
}

LcWindowsInputSystem::LcWindowsInputSystem() : activeDevice(nullptr)
{
    devices.push_back(std::make_shared<LcKeyboard>());
    activeDevice = devices[0].get();
}

TInputSystemPtr GetWindowsInputSystem()
{
    return std::make_unique<LcWindowsInputSystem>();
}

TInputSystemPtr GetDirectInputSystem()
{
    return std::make_unique<LcDirectInputSystem>();
}
