/**
* WindowsApplication.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Application/Windows/WindowsApplication.h"
#include "RenderSystem/DX10RenderSystem/DX10RenderSystem.h"
#include "GUI/GuiManager.h"
#include "Core/LCUtils.h"


static const WCHAR* LCWindowClassName = L"LCGameWindowClass";
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

struct LCWin32Handles
{
    KeyboardHandler keyboardHandler;
    MouseHandler mouseHandler;
};


LCWindowsApplication::LCWindowsApplication()
{
	hInstance = nullptr;
    hWnd = nullptr;
    renderSystem = nullptr;
    type = ERenderSystemType::Null;
    cmds = nullptr;
    cmdsCount = 0;
    windowSize = LCSize(800, 600);
	quit = false;
    useNoesis = false;
    prevTick = 0;
}

LCWindowsApplication::~LCWindowsApplication()
{
    if (renderSystem)
    {
        renderSystem->Shutdown();
        renderSystem.reset();
    }

    if (hWnd)
    {
        DestroyWindow(hWnd);
        UnregisterClassW(LCWindowClassName, hInstance);
        hWnd = nullptr;
    }
}

void LCWindowsApplication::Init(void* Handle, LCSTR* inCmds, int inCmdsCount) noexcept
{
	hInstance = (HINSTANCE)Handle;
    cmds = inCmds;
    cmdsCount = inCmdsCount;
}

void LCWindowsApplication::Init(void* Handle, LCSTR inCmds) noexcept
{
    Init(Handle, inCmds ? &inCmds : nullptr, inCmds ? 1 : 0);
}

void LCWindowsApplication::LoadShaders(const std::string& folderPath)
{
    using namespace std::filesystem;

    for (auto& entry : directory_iterator(folderPath))
    {
        if (entry.is_regular_file())
        {
            auto name = entry.path().filename().string();
            auto content = ReadTextFile(entry.path().string());
            if (!name.empty() && !content.empty())
            {
                shaders[name] = content;
            }
        }
    }
}

void LCWindowsApplication::Run()
{
	if (!hInstance) throw std::exception("LCWindowsApplication::Run(): Invalid platform handle");

    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEXW);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = LCWindowClassName;
    wcex.hIconSm = NULL;
    if (0 == RegisterClassExW(&wcex))
    {
        throw std::exception("LCWindowsApplication::Run(): Cannot register window class");
    }

    RECT clientRect{ 0, 0, windowSize.x(), windowSize .y() };
    AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, FALSE);

    hWnd = CreateWindowW(LCWindowClassName, L"Game Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd)
    {
        throw std::exception("LCWindowsApplication::Run(): Cannot create window");
    }

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    LCWin32Handles handles{ keyboardHandler, mouseHandler };
    SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&handles));

    if (type == ERenderSystemType::DX10)
    {
        renderSystem.reset(new DX10RenderSystem(*this));
        renderSystem->Create(hWnd, windowSize, true);
    }

	prevTick = GetTickCount64();
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
            Sleep(1);
            OnUpdate();
        }
	}
}

void LCWindowsApplication::OnUpdate()
{
    auto curTick = GetTickCount64();
    auto ticksCount = (double)curTick - (double)prevTick;
    float deltaSeconds = float(ticksCount / 1000.0);

    if (curTick != prevTick)
    {
        prevTick = curTick;

        if (renderSystem)
        {
            renderSystem->Update(deltaSeconds);
            renderSystem->Render();
        }

        if (updateHandler) updateHandler(deltaSeconds);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LCWin32Handles* handles = reinterpret_cast<LCWin32Handles*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
        if (handles && handles->keyboardHandler) handles->keyboardHandler((int)wParam, EInputKeyEvent::Down);
        break;
    case WM_KEYUP:
        if (handles && handles->keyboardHandler) handles->keyboardHandler((int)wParam, EInputKeyEvent::Up);
        break;
    case WM_LBUTTONDOWN:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            if (handles && handles->mouseHandler) handles->mouseHandler((int)wParam, EInputKeyEvent::Down, (float)x, (float)y);
            LCGUIManager::GetInstance().MouseButtonDown(x, y);
        }
        break;
    case WM_LBUTTONUP:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            if (handles && handles->mouseHandler) handles->mouseHandler((int)wParam, EInputKeyEvent::Down, (float)x, (float)y);
            LCGUIManager::GetInstance().MouseButtonUp(x, y);
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
