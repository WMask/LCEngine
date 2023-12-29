/**
* InputSystem.cpp
* 21.09.2023
* (c) Denis Romakhov
*/

#ifdef _WINDOWS
#include "pch.h"
#endif
#include "Module.h"
#include "Core/InputSystem.h"
#include "Core/LCException.h"
#include "Application/ApplicationInterface.h"


const std::wstring LcDefaultInputDevice::Name = L"Keyboard";

LcDefaultInputSystem::LcDefaultInputSystem() : activeDevice(nullptr), cfg(nullptr)
{
}

void LcDefaultInputSystem::Init(const LcAppContext& context)
{
    cfg = context.app ? &context.app->GetConfig() : nullptr;
    devices.push_back(std::make_shared<LcDefaultInputDevice>(cfg));
    activeDevice = devices[0].get();
}

void LcDefaultInputSystem::SetActiveDevice(const std::wstring& deviceNamePart)
{
    for (auto& device : devices)
    {
        auto deviceName = device->GetName();
        if (deviceName.find(deviceNamePart) != deviceName.npos)
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

void LcDefaultInputSystem::SetActiveDevice(const IInputDevice* inActiveDevice)
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

bool LcDefaultInputDevice::Pressed(const std::string& actionName) const
{
    if (cfg)
    {
        for (auto& action : cfg->Actions)
        {
            if (action.Name == actionName)
            {
                if (action.Key >= 0 && action.Key < LcKeysCount && (keys.keys[action.Key] != 0))
                {
                    return true;
                }

                if (action.JoyKey >= 0 && action.JoyKey < LcKeysCount && (keys.keys[action.JoyKey] != 0))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

KEYS::KEYS()
{
    memset(keys, 0, sizeof(keys));
}

unsigned char& KEYS::operator[](int index)
{
    if (index < 0 || index >= LcKeysCount) throw LcException("KEYS::operator[]: Invalid index");

    return keys[index];
}


TInputSystemPtr GetDefaultInputSystem()
{
    return std::make_unique<LcDefaultInputSystem>();
}
