/**
* WindowsApplication.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Application/Windows/WindowsApplication.h"
#include "RenderSystem/RenderSystem.h"
#include "World/WorldInterface.h"
#include "Core/LCException.h"
#include "Core/ScriptSystem.h"
#include "Core/Physics.h"
#include "Core/Audio.h"
#include "GUI/GuiManager.h"

#include <timeapi.h>

#define WS_LC_WINDOW_MENU   (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
#define WS_LC_WINDOW         WS_POPUPWINDOW


static const WCHAR* LcWindowClassName = L"LcWindowClassName";
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

struct LcWin32Handles
{
    LcKeysHandler keysHandler;
    LcActionHandler actionHandler;
    LcMouseMoveHandler mouseMoveHandler;
    LcMouseButtonHandler mouseButtonHandler;
    LcAppContext& appContext;
    IApplication& app;
    LcAppConfig& cfg;
};


LcWindowsApplication::LcWindowsApplication()
    : world(::GetWorld(context))
    , inputSystem(GetDefaultInputSystem())
    , localization(std::make_shared<LcLocalizationManager>())
{
    hInstance = nullptr;
    hWnd = nullptr;
    cmds.clear();
    cmdsCount = 0;
    winMode = LcWinMode::Windowed;
    quit = false;
    prevTime.QuadPart = 0;
    frequency.QuadPart = 0;
}

LcWindowsApplication::~LcWindowsApplication()
{
    if (world) world.reset();

    if (inputSystem)
    {
        inputSystem->Shutdown();
        inputSystem.reset();
    }

    if (audioSystem)
    {
        audioSystem->Shutdown();
        audioSystem.reset();
    }

    if (guiManager)
    {
        guiManager->Shutdown();
        guiManager.reset();
    }

    if (renderSystem)
    {
        renderSystem->Shutdown();
        renderSystem.reset();
    }

    if (hWnd)
    {
        DestroyWindow(hWnd);
        UnregisterClassW(LcWindowClassName, hInstance);
        hWnd = nullptr;
    }
}

void LcWindowsApplication::Init(void* handle, const std::wstring& inCmds, int inCmdsCount, const char* inShadersPath) noexcept
{
	hInstance = (HINSTANCE)handle;
    cmds = inCmds;
    cmdsCount = inCmdsCount;
    if (inShadersPath) shadersPath = inShadersPath;
}

void LcWindowsApplication::Init(void* handle, const std::wstring& inCmds, const char* inShadersPath) noexcept
{
    Init(handle, inCmds, 1, inShadersPath);
}

void LcWindowsApplication::Init(void* handle) noexcept
{
    Init(handle, L"", 1, "../../../Shaders/HLSL/");
}

void LcWindowsApplication::Run()
{
    LC_TRY

	if (!hInstance) throw std::exception("LcWindowsApplication::Run(): Invalid platform handle");
    if (!world) throw std::exception("LcWindowsApplication::Run(): Invalid world");

    // set context
    context.app = this;
    context.world = world.get();
    context.render = renderSystem.get();
    context.scripts = scriptSystem.get();
    context.audio = audioSystem.get();
    context.input = inputSystem.get();
    context.gui = guiManager.get();
    context.physics = physWorld.get();
    context.text = localization.get();

    // get window size
    unsigned int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    BOOL windowedStyle = (cfg.WinHeight < screenHeight) ? TRUE : FALSE;
    int style = windowedStyle ? WS_LC_WINDOW_MENU : WS_LC_WINDOW;

    RECT clientRect{ 0, 0, (LONG)cfg.WinWidth, (LONG)cfg.WinHeight };
    AdjustWindowRect(&clientRect, style, FALSE);
    int winWidth = clientRect.right - clientRect.left;
    int winHeight = clientRect.bottom - clientRect.top;

    // create window
    WNDCLASSEXW wcex{};
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszClassName = LcWindowClassName;
    if (0 == RegisterClassExW(&wcex))
    {
        throw LcException("LcWindowsApplication::Run(): Cannot register window class");
    }

    hWnd = CreateWindowW(LcWindowClassName, L"Game Window", style, CW_USEDEFAULT, CW_USEDEFAULT,
        winWidth, winHeight, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd)
    {
        throw LcException("LcWindowsApplication::Run(): Cannot create window");
    }

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    context.windowHandle = hWnd;

    SYSTEMTIME time;
    GetSystemTime(&time);
    srand(time.wMilliseconds);

    inputSystem->Init(context);

    // set handles
    LcWin32Handles handles {
        inputSystem->GetKeysHandler(),
        inputSystem->GetActionHandler(),
        inputSystem->GetMouseMoveHandler(),
        inputSystem->GetMouseButtonHandler(),
        context, *this, cfg
    };
    SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&handles));

    if (context.text) context.text->Init(&context);

    // set initial world size
    if (renderSystem) renderSystem->Subscribe(context);

    world->UpdateWorldScale(LcSize{ cfg.WinWidth, cfg.WinHeight });

    // init subsystems
    if (renderSystem)
    {
        if (!shadersPath.empty()) renderSystem->LoadShaders(shadersPath.c_str());

        renderSystem->Create(hWnd, winMode, cfg.bVSync, cfg.bAllowFullscreen, context);
    }
    if (audioSystem) audioSystem->Init(context);
    if (scriptSystem) scriptSystem->Init(context);

    // call app init handler
    if (initHandler)
    {
        LC_TRY

        initHandler(context);

        LC_CATCH{ LC_THROW("LcWindowsApplication::Run(onInitHandler)") }
    }

    // run game loop
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&prevTime);

    MSG msg;
	while (!quit)
	{
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            if (!cfg.bNoDelay)
            {
                timeBeginPeriod(1);
                Sleep(1);
                timeEndPeriod(1);
            }

            // update and render
            OnUpdate();
        }
	}

    // set NULL to skip crash in WndProc
    SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);

    LC_CATCH {
        SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);
        LC_THROW("LcWindowsApplication::Run()")
    }
}

void LcWindowsApplication::ClearWorld(bool removeRooted)
{
    world->Clear(removeRooted);
    if (renderSystem) renderSystem->Clear(world.get(), removeRooted);
    if (audioSystem) audioSystem->Clear(removeRooted);
    if (physWorld) physWorld->Clear(removeRooted);
}

void LcWindowsApplication::OnUpdate()
{
    LC_TRY

    if (!world) throw std::exception("LcWindowsApplication::Run(): Invalid world");

    LARGE_INTEGER curTime, deltaTime;
    QueryPerformanceCounter(&curTime);
    deltaTime.QuadPart = (curTime.QuadPart - prevTime.QuadPart);

    if (curTime.QuadPart != prevTime.QuadPart)
    {
        prevTime.QuadPart = curTime.QuadPart;

        double deltaSeconds = static_cast<double>(deltaTime.QuadPart) / 10000000.0;
        float deltaFloat = static_cast<float>(deltaSeconds);

        double timeSeconds = static_cast<double>(curTime.QuadPart) / 10000000.0;
        context.gameTime = static_cast<float>(timeSeconds);

        if (inputSystem)
        {
            inputSystem->Update(deltaFloat, context);
        }

        if (renderSystem && world)
        {
            renderSystem->Update(deltaFloat, context);
            renderSystem->Render(context);
        }

        if (physWorld)
        {
            physWorld->Update(deltaFloat, context);
        }

        if (audioSystem)
        {
            audioSystem->Update(deltaFloat, context);
        }

        if (updateHandler)
        {
            updateHandler(deltaFloat, context);
        }
    }

    LC_CATCH{ LC_THROW("LcWindowsApplication::OnUpdate()") }
}

LcAppStats LcWindowsApplication::GetAppStats() const noexcept
{
    auto renderStats = renderSystem ? renderSystem->GetStats() : LcRSStats{};
    int numSprites = (int)std::count_if(world->GetVisuals().begin(), world->GetVisuals().end(), [](const std::shared_ptr<IVisual>& visual) {
        return visual->GetTypeId() == LcCreatables::Sprite;
    });
    int numWidgets = (int)world->GetVisuals().size() - numSprites;
    return LcAppStats{
        numSprites,
        numWidgets,
        renderStats.numTextures,
        renderStats.numTilemaps,
        renderStats.numFonts,
        audioSystem ? (int)audioSystem->GetSounds().size() : 0,
        physWorld ? (int)physWorld->GetDynamicBodies().size() : 0
    };
}

void LcWindowsApplication::SetWindowSize(unsigned int width, unsigned int height)
{
    auto oldSize = LcSize{ cfg.WinWidth, cfg.WinHeight };
    auto newSize = LcSize{ width, height };
    if (oldSize == newSize) return;

    cfg.WinWidth = width;
    cfg.WinHeight = height;

    if (renderSystem && renderSystem->CanRender())
    {
        // actual resize in WM_SIZE message handler
        renderSystem->RequestResize(width, height);
    }
}

void LcWindowsApplication::SetWindowMode(LcWinMode mode)
{
    winMode = mode;

    if (renderSystem && renderSystem->CanRender())
    {
        renderSystem->SetMode(winMode);
    }
}

int MapMouseKeys(WPARAM wParam)
{
    switch (wParam)
    {
    case MK_RBUTTON: return LcMouseBtn::Right;
    case MK_MBUTTON: return LcMouseBtn::Middle;
    }

    return LcMouseBtn::Left;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LcWin32Handles* handles = reinterpret_cast<LcWin32Handles*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    IGuiManager* guiManager = handles ? handles->appContext.gui : nullptr;
    IInputSystem* inputSystem = handles ? handles->appContext.input : nullptr;
    IInputDevice* activeKeyboard = nullptr;
    IInputDevice* activeMouse = nullptr;

    if (inputSystem)
    {
        const auto& devices = inputSystem->GetInputDevicesList();
        for (const auto& device : devices)
        {
            if (!device || !device->IsActive()) continue;
            if (device->GetType() == LcInputDeviceType::Keyboard) activeKeyboard = device.get();
            if (device->GetType() == LcInputDeviceType::Mouse) activeMouse = device.get();
        }
    }

    int x = GET_X_LPARAM(lParam);
    int y = GET_Y_LPARAM(lParam);

    LC_TRY

    switch (message)
    {
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_MENUCHAR:
        return MNC_CLOSE << 16; // disable exit fullscreen mode sound
    case WM_SIZE:
        if (handles && handles->appContext.render)
        {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            handles->appContext.render->Resize(width, height, handles->appContext);
        }
        break;
    case WM_KEYDOWN:
        if (activeKeyboard) activeKeyboard->GetState().at(static_cast<int>(wParam)) = true;
        break;
    case WM_KEYUP:
        if (activeKeyboard) activeKeyboard->GetState().at(static_cast<int>(wParam)) = false;
        break;
    case WM_MOUSEMOVE:
        if (activeMouse) activeMouse->SetPointerPos(LcVector2{ static_cast<float>(x), static_cast<float>(y) });
        break;
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
        if (activeMouse) activeMouse->GetState().at(MapMouseKeys(wParam)) = true;
        break;
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
        if (activeMouse) activeMouse->GetState().at(MapMouseKeys(wParam)) = false;
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    LC_CATCH{ LC_THROW("LcWindowsApplication::WndProc()") }

    return 0;
}

TAppPtr GetApp()
{
    return std::make_unique<LcWindowsApplication>();
}
