/**
* InputSystem.cpp
* 21.09.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Module.h"
#include "Core/InputSystem.h"


const std::wstring LcDefaultInputDevice::Name = L"Keyboard";

LcDefaultInputSystem::LcDefaultInputSystem() : activeDevice(nullptr)
{
    devices.push_back(std::make_shared<LcDefaultInputDevice>());
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

KEYS::KEYS()
{
    memset(keys, 0, sizeof(keys));
}

unsigned char& KEYS::operator[](int index)
{
    if (index < 0 || index >= LcKeysCount) throw std::exception("KEYS::operator[]: Invalid index");

    return keys[index];
}


TInputSystemPtr GetDefaultInputSystem()
{
    return std::make_unique<LcDefaultInputSystem>();
}
