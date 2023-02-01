/**
* WindowsApplication.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "Application/Windows/WindowsApplication.h"
#include "RenderSystem/RenderSystemDX10/RenderSystemDX10.h"
#include "GUI/GuiManager.h"
#include "Core/LCUtils.h"


static const WCHAR* LcWindowClassName = L"LcWindowClassName";
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

struct LcWin32Handles
{
    LcKeyboardHandler keyboardHandler;
    LcMouseHandler mouseHandler;
};


LcWindowsApplication::LcWindowsApplication()
{
	hInstance = nullptr;
    hWnd = nullptr;
    renderSystem = nullptr;
    type = LcRenderSystemType::Null;
    cmds.clear();
    cmdsCount = 0;
    windowSize = LcSize(800, 600);
	quit = false;
    useNoesis = false;
    prevTick = 0;
}

LcWindowsApplication::~LcWindowsApplication()
{
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

void LcWindowsApplication::Init(void* Handle, const std::wstring& inCmds, int inCmdsCount) noexcept
{
	hInstance = (HINSTANCE)Handle;
    cmds = inCmds;
    cmdsCount = inCmdsCount;
}

void LcWindowsApplication::Init(void* Handle, const std::wstring& inCmds) noexcept
{
    Init(Handle, inCmds, 1);
}

void LcWindowsApplication::LoadShaders(const std::string& folderPath)
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

void LcWindowsApplication::Run()
{
	if (!hInstance) throw std::exception("LcWindowsApplication::Run(): Invalid platform handle");

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
    wcex.lpszClassName = LcWindowClassName;
    wcex.hIconSm = NULL;
    if (0 == RegisterClassExW(&wcex))
    {
        throw std::exception("LcWindowsApplication::Run(): Cannot register window class");
    }

    RECT clientRect{ 0, 0, windowSize.x(), windowSize .y() };
    AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, FALSE);

    hWnd = CreateWindowW(LcWindowClassName, L"Game Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd)
    {
        throw std::exception("LcWindowsApplication::Run(): Cannot create window");
    }

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    LcWin32Handles handles{ keyboardHandler, mouseHandler };
    SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&handles));

    if (type == LcRenderSystemType::DX10)
    {
        renderSystem.reset(new LcRenderSystemDX10(*this));
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

void LcWindowsApplication::OnUpdate()
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

    switch (message)
    {
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
        if (handles && handles->keyboardHandler) handles->keyboardHandler((int)wParam, LcKeyState::Down);
        break;
    case WM_KEYUP:
        if (handles && handles->keyboardHandler) handles->keyboardHandler((int)wParam, LcKeyState::Up);
        break;
    case WM_LBUTTONDOWN:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            if (handles && handles->mouseHandler) handles->mouseHandler(MapMouseKeys(wParam), LcKeyState::Down, (float)x, (float)y);
            LcGUIManager::GetInstance().MouseButtonDown(LcMouseBtn::Left, x, y);
        }
        break;
    case WM_LBUTTONUP:
        {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            if (handles && handles->mouseHandler) handles->mouseHandler(MapMouseKeys(wParam), LcKeyState::Down, (float)x, (float)y);
            LcGUIManager::GetInstance().MouseButtonUp(LcMouseBtn::Left, x, y);
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
