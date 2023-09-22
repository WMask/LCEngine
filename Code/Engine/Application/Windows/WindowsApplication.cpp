/**
* WindowsApplication.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Application/Windows/WindowsApplication.h"
#include "RenderSystem/RenderSystem.h"
#include "RenderSystem/WidgetRender.h"
#include "World/WorldInterface.h"
#include "Core/LCException.h"
#include "Core/ScriptSystem.h"
#include "Core/Physics.h"
#include "Core/Audio.h"
#include "GUI/GuiManager.h"

#define WS_LC_WINDOW_MENU   (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)
#define WS_LC_WINDOW         WS_POPUPWINDOW


static const WCHAR* LcWindowClassName = L"LcWindowClassName";
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

struct LcWin32Handles
{
    LcKeysHandler keysHandler;
    LcMouseMoveHandler mouseMoveHandler;
    LcMouseButtonHandler mouseButtonHandler;
    LcAppContext& appContext;
    IApplication& app;
};


LcWindowsApplication::LcWindowsApplication()
    : world(::GetWorld(context))
    , context(*world.get())
    , inputSystem(GetWindowsInputSystem())
{
    hInstance = nullptr;
    hWnd = nullptr;
    cmds.clear();
    cmdsCount = 0;
    windowSize = LcSize(800, 600);
    winMode = LcWinMode::Windowed;
    quit = false;
    vSync = true;
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

    // get window size
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    BOOL windowedStyle = (windowSize.y < screenHeight) ? TRUE : FALSE;
    int style = windowedStyle ? WS_LC_WINDOW_MENU : WS_LC_WINDOW;

    RECT clientRect{ 0, 0, windowSize.x, windowSize.y };
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
        throw std::exception("LcWindowsApplication::Run(): Cannot register window class");
    }

    hWnd = CreateWindowW(LcWindowClassName, L"Game Window", style, CW_USEDEFAULT, CW_USEDEFAULT,
        winWidth, winHeight, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd)
    {
        throw std::exception("LcWindowsApplication::Run(): Cannot create window");
    }

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    // set context
    context.render = renderSystem.get();
    context.scripts = scriptSystem.get();
    context.audio = audioSystem.get();
    context.input = inputSystem.get();
    context.gui = guiManager.get();
    context.physics = physWorld.get();
    context.windowHandle = hWnd;

    inputSystem->Init(context);

    // set handles
    LcWin32Handles handles {
        inputSystem->GetKeysHandler(),
        inputSystem->GetMouseMoveHandler(),
        inputSystem->GetMouseButtonHandler(),
        context, *this
    };
    SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&handles));

    // set initial world size
    if (renderSystem) renderSystem->Subscribe(context);

    world->GetWorldScale().UpdateWorldScale(windowSize);

    // init subsystems
    if (renderSystem)
    {
        if (!shadersPath.empty()) renderSystem->LoadShaders(shadersPath.c_str());

        renderSystem->Create(hWnd, winMode, vSync, context);
    }

    if (audioSystem)
    {
        audioSystem->Init(context);
    }

    // call app init handler
    if (initHandler)
    {
        LC_TRY

        initHandler(this);

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
            OnUpdate();
        }
	}

    SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);

    LC_CATCH{ LC_THROW("LcWindowsApplication::Run()") }
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

        if (inputSystem)
        {
            inputSystem->Update(deltaFloat, context);
        }

        if (renderSystem && world)
        {
            renderSystem->Update(deltaFloat, context);
            renderSystem->Render(context);
        }

        if (audioSystem)
        {
            audioSystem->Update(deltaFloat, context);
        }

        if (guiManager)
        {
            guiManager->Update(deltaFloat, context);
        }

        if (updateHandler)
        {
            updateHandler(deltaFloat, this);
        }
    }

    LC_CATCH{ LC_THROW("LcWindowsApplication::OnUpdate()") }
}

void LcWindowsApplication::SetWindowSize(int width, int height)
{
    auto newSize = LcSize(width, height);
    if (windowSize == newSize) return;

    windowSize = newSize;

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

LcMouseBtn MapMouseKeys(WPARAM wParam)
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
    IInputDevice* activeDevice = inputSystem ? inputSystem->GetActiveInputDevice() : nullptr;

    // for joysticks callbacks called in IInputSystem::Update
    bool isKeyboardActive = activeDevice ? (activeDevice->GetType() == LcInputDeviceType::Keyboard) : false;

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
        if (isKeyboardActive)
        {
            if (inputSystem) inputSystem->GetState()[(int)wParam] = true;
            if (handles && handles->keysHandler) handles->keysHandler((int)wParam, LcKeyState::Down, &handles->app);
            if (guiManager) guiManager->OnKeys((int)wParam, LcKeyState::Down, handles->appContext);
        }
        break;
    case WM_KEYUP:
        if (isKeyboardActive)
        {
            if (inputSystem) inputSystem->GetState()[(int)wParam] = false;
            if (handles && handles->keysHandler) handles->keysHandler((int)wParam, LcKeyState::Up, &handles->app);
            if (guiManager) guiManager->OnKeys((int)wParam, LcKeyState::Up, handles->appContext);
        }
        break;
    case WM_MOUSEMOVE:
        if (guiManager) guiManager->OnMouseMove(x, y, handles->appContext);
        break;
    case WM_LBUTTONDOWN:
        if (handles && handles->mouseButtonHandler) handles->mouseButtonHandler(MapMouseKeys(wParam), LcKeyState::Down, (float)x, (float)y, &handles->app);
        if (guiManager) guiManager->OnMouseButton(LcMouseBtn::Left, LcKeyState::Down, x, y, handles->appContext);
        break;
    case WM_LBUTTONUP:
        if (handles && handles->mouseButtonHandler) handles->mouseButtonHandler(MapMouseKeys(wParam), LcKeyState::Up, (float)x, (float)y, &handles->app);
        if (guiManager) guiManager->OnMouseButton(LcMouseBtn::Left, LcKeyState::Up, x, y, handles->appContext);
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
