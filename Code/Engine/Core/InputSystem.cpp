/**
* InputSystem.cpp
* 21.09.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Module.h"
#include "GUI/GuiManager.h"
#include "Core/InputSystem.h"
#include "Core/LCException.h"
#include "Application/ApplicationInterface.h"

const std::wstring IInputDevice::MouseAndKeyboard = L"[MK]";
const std::wstring LcKeyboardInputDevice::Name = L"[MK] Keyboard";
const std::wstring LcMouseInputDevice::Name = L"[MK] Mouse";


LcDefaultInputSystem::LcDefaultInputSystem() : activeDevice(nullptr), cfg(nullptr)
{
}

void LcDefaultInputSystem::Init(const LcAppContext& context)
{
    cfg = context.app ? &context.app->GetConfig() : nullptr;

#ifdef _WINDOWS

    devices.push_back(std::make_unique<LcKeyboardInputDevice>(cfg));
    devices.push_back(std::make_unique<LcMouseInputDevice>(cfg));
    activeDevice = devices[0].get();

#endif
}

std::deque<LcActionBinding> GetActions(LcActionType type, int id, LcAppConfig& cfg)
{
    std::deque<LcActionBinding> actions;

    for (auto& action : cfg.Actions)
    {
        switch (type)
        {
        case LcActionType::Key:
            if (id == action.Key) actions.push_back(action);
            break;
        case LcActionType::Mouse:
            if (id == action.MouseBtn) actions.push_back(action);
            break;
        }
    }

    return actions;
}

void LcDefaultInputSystem::Update(float deltaSeconds, const LcAppContext& context)
{
    for (auto& device : devices)
    {
        uint8_t deviceId = static_cast<uint8_t>(device->GetType());
        if (!device->IsActive() || deviceId >= FirstJoystickId) continue;

        if (device->GetType() == LcInputDeviceType::Keyboard)
        {
            for (int key = 0; key < LcJoystickKeysOffset; key++)
            {
                bool bIsPressed = device->GetState().keys[key] && !device->GetPrevState().keys[key];
                bool bIsReleased = !device->GetState().keys[key] && device->GetPrevState().keys[key];

                if (bIsPressed || bIsReleased)
                {
                    LcKeyState keyState = bIsPressed ? LcKeyState::Down : LcKeyState::Up;

                    if (keysHandler) keysHandler(key, keyState, context);
                    if (context.gui) context.gui->OnKeys(key, keyState, context);
                    if (actionHandler && cfg)
                    {
                        auto actions = GetActions(LcActionType::Key, key, *cfg);
                        for (auto& action : actions)
                        {
                            actionHandler(LcKeyAction(action.Name, key, keyState), context);
                        }
                    }
                }
            }
        }
        else if (device->GetType() == LcInputDeviceType::Mouse)
        {
            LcVector2 mousePos = device->GetPointerPos();
            LcPoint mousePosInt{ std::lround(mousePos.x), std::lround(mousePos.y) };

            for (int button = 0; button < LcMouseKeysCount; button++)
            {
                bool bIsPressed = device->GetState().keys[button] && !device->GetPrevState().keys[button];
                bool bIsReleased = !device->GetState().keys[button] && device->GetPrevState().keys[button];

                if (bIsPressed || bIsReleased)
                {
                    LcKeyState buttonState = bIsPressed ? LcKeyState::Down : LcKeyState::Up;

                    if (mouseButtonHandler) mouseButtonHandler(button, buttonState, mousePos.x, mousePos.y, context);
                    if (context.gui) context.gui->OnMouseButton(button, buttonState, mousePosInt.x, mousePosInt.y, context);
                    if (actionHandler && cfg)
                    {
                        auto actions = GetActions(LcActionType::Mouse, button, *cfg);
                        for (auto& action : actions)
                        {
                            actionHandler(LcMouseAction(action.Name, button, buttonState, mousePos.x, mousePos.y), context);
                        }
                    }
                }
            }

            if (device->GetPointerPos() != device->GetPrevPointerPos())
            {
                if (mouseMoveHandler) mouseMoveHandler(mousePos.x, mousePos.y, context);
                if (context.gui) context.gui->OnMouseMove(mousePosInt.x, mousePosInt.y, context);
            }
        }

        device->Update(deltaSeconds, context);
    }
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

void LcKeyboardInputDevice::Update(float deltaSeconds, const LcAppContext& context)
{
    memcpy(prevKeys.Get(), keys.Get(), sizeof(keys));
}

bool LcKeyboardInputDevice::Pressed(const std::string& actionName) const
{
    if (!cfg || !active) return false;

    for (auto& action : cfg->Actions)
    {
        if (action.Name == actionName)
        {
            if (action.Key >= 0 && action.Key < LcJoystickKeysOffset && (keys.keys[action.Key] != 0))
            {
                return true;
            }

            if (action.JoyKey >= LcJoystickKeysOffset && action.JoyKey < LcKeysCount && (keys.keys[action.JoyKey] != 0))
            {
                return true;
            }

            break;
        }
    }

    return false;
}

void LcMouseInputDevice::Update(float deltaSeconds, const LcAppContext& context)
{
    memcpy(prevButtons.Get(), buttons.Get(), sizeof(buttons));
    prevMousePos = mousePos;
}

bool LcMouseInputDevice::Pressed(const std::string& actionName) const
{
    if (!cfg || !active) return false;

    for (auto& action : cfg->Actions)
    {
        if (action.Name == actionName)
        {
            if (action.MouseBtn >= 0 && action.MouseBtn < LcMouseKeysCount && (buttons.keys[action.MouseBtn] != 0))
            {
                return true;
            }

            break;
        }
    }

    return false;
}

KEYS::KEYS()
{
    memset(keys, 0, sizeof(keys));
}

uint8_t& KEYS::at(int index)
{
    if (index < 0 || index >= LcKeysCount) throw LcException("KEYS::operator[]: Invalid index");

    return keys[index];
}


TInputSystemPtr GetDefaultInputSystem()
{
    return std::make_unique<LcDefaultInputSystem>();
}
