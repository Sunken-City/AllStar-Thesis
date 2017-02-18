#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "Engine/Core/Memory/MemoryTracking.hpp"
#include "Engine/Math/Vector2.hpp"
#include "Engine/Time/Time.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Audio/Audio.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/Console.hpp"
#include "Engine/Input/Logging.hpp"
#include "Engine/Core/ProfilingUtils.h"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Memory/MemoryOutputWindow.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Fonts/BitmapFont.hpp"
#include "Game/TheGame.hpp"
#include "Engine/Renderer/2D/SpriteGameRenderer.hpp"
#include "Engine/Core/Events/Event.hpp"
#include "Engine/Core/Events/EventSystem.hpp"
#include "Engine/UI/UISystem.hpp"

//-----------------------------------------------------------------------------------------------
#define UNUSED(x) (void)(x);

//-----------------------------------------------------------------------------------------------
const int OFFSET_FROM_WINDOWS_DESKTOP = 50;
const extern int WINDOW_PHYSICAL_WIDTH = 1600; //Gameboy: 160
const extern int WINDOW_PHYSICAL_HEIGHT = 900; //Gameboy: 144
const extern int IMPORT_RESOLUTION = 900;
const extern float VIRTUAL_SIZE = 10.0f;
const float VIEW_LEFT = 0.0;
const float VIEW_RIGHT = static_cast<float>(WINDOW_PHYSICAL_WIDTH);
const float VIEW_BOTTOM = 0.0;
const float VIEW_TOP = VIEW_RIGHT * static_cast<float>(WINDOW_PHYSICAL_HEIGHT) / static_cast<float>(WINDOW_PHYSICAL_WIDTH);
const Vector2 BOTTOM_LEFT = Vector2(VIEW_LEFT, VIEW_BOTTOM);
const Vector2 TOP_RIGHT = Vector2(VIEW_RIGHT, VIEW_TOP);

bool g_isQuitting = false;
HWND g_hWnd = nullptr;
HDC g_displayDeviceContext = nullptr;
HGLRC g_openGLRenderingContext = nullptr;
const char* APP_NAME = "AllStar";

//-----------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
    unsigned char asKey = (unsigned char)wParam;
    switch (wmMessageCode)
    {
    case WM_CLOSE:
    case WM_DESTROY:
    case WM_QUIT:
        g_isQuitting = true;
        return 0;

    case WM_CHAR:
        InputSystem::instance->SetLastPressedChar(asKey);
        break;

    case WM_KEYDOWN:
        InputSystem::instance->SetKeyDownStatus(asKey, true);
        if (asKey == VK_ESCAPE)
        {
            g_isQuitting = true;
            return 0;
        }
        break;

    case WM_KEYUP:
        InputSystem::instance->SetKeyDownStatus(asKey, false);
        break;

    case WM_LBUTTONDOWN:
        InputSystem::instance->SetMouseDownStatus(InputSystem::MouseButton::LEFT_MOUSE_BUTTON, true);
        break;

    case WM_RBUTTONDOWN:
        InputSystem::instance->SetMouseDownStatus(InputSystem::MouseButton::RIGHT_MOUSE_BUTTON, true);
        break;

    case WM_MBUTTONDOWN:
        InputSystem::instance->SetMouseDownStatus(InputSystem::MouseButton::MIDDLE_MOUSE_BUTTON, true);
        break;

    case WM_LBUTTONUP:
        InputSystem::instance->SetMouseDownStatus(InputSystem::MouseButton::LEFT_MOUSE_BUTTON, false);
        break;

    case WM_RBUTTONUP:
        InputSystem::instance->SetMouseDownStatus(InputSystem::MouseButton::RIGHT_MOUSE_BUTTON, false);
        break;

    case WM_MBUTTONUP:
        InputSystem::instance->SetMouseDownStatus(InputSystem::MouseButton::MIDDLE_MOUSE_BUTTON, false);
        break;

    case WM_MOUSEWHEEL:
        short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        InputSystem::instance->SetMouseWheelStatus(zDelta);
        break;
    }

    return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}

//-----------------------------------------------------------------------------------------------
void CreateOpenGLWindow(HINSTANCE applicationInstanceHandle)
{
    // Define a window class
    WNDCLASSEX windowClassDescription;
    memset(&windowClassDescription, 0, sizeof(windowClassDescription));
    windowClassDescription.cbSize = sizeof(windowClassDescription);
    windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
    windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure); // Assign a win32 message-handling function
    windowClassDescription.hInstance = GetModuleHandle(NULL);
    windowClassDescription.hIcon = NULL;
    windowClassDescription.hCursor = NULL;
    windowClassDescription.lpszClassName = TEXT("Simple Window Class");
    RegisterClassEx(&windowClassDescription);

    const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
    const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

    RECT desktopRect;
    HWND desktopWindowHandle = GetDesktopWindow();
    GetClientRect(desktopWindowHandle, &desktopRect);

    RECT windowRect = { OFFSET_FROM_WINDOWS_DESKTOP, OFFSET_FROM_WINDOWS_DESKTOP, OFFSET_FROM_WINDOWS_DESKTOP + WINDOW_PHYSICAL_WIDTH, OFFSET_FROM_WINDOWS_DESKTOP + WINDOW_PHYSICAL_HEIGHT };
    AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

    WCHAR windowTitle[1024];
    MultiByteToWideChar(GetACP(), 0, APP_NAME, -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
    g_hWnd = CreateWindowEx(
        windowStyleExFlags,
        windowClassDescription.lpszClassName,
        windowTitle,
        windowStyleFlags,
        windowRect.left,
        windowRect.top,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        NULL,
        NULL,
        applicationInstanceHandle,
        NULL);

    ShowWindow(g_hWnd, SW_SHOW);
    SetForegroundWindow(g_hWnd);
    SetFocus(g_hWnd);

    g_displayDeviceContext = GetDC(g_hWnd);

    HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
    SetCursor(cursor);

    PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
    memset(&pixelFormatDescriptor, 0, sizeof(pixelFormatDescriptor));
    pixelFormatDescriptor.nSize = sizeof(pixelFormatDescriptor);
    pixelFormatDescriptor.nVersion = 1;
    pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
    pixelFormatDescriptor.cColorBits = 24;
    pixelFormatDescriptor.cDepthBits = 24;
    pixelFormatDescriptor.cAccumBits = 0;
    pixelFormatDescriptor.cStencilBits = 8;

    int pixelFormatCode = ChoosePixelFormat(g_displayDeviceContext, &pixelFormatDescriptor);
    SetPixelFormat(g_displayDeviceContext, pixelFormatCode, &pixelFormatDescriptor);
    g_openGLRenderingContext = wglCreateContext(g_displayDeviceContext);
    wglMakeCurrent(g_displayDeviceContext, g_openGLRenderingContext);

    
}

//-----------------------------------------------------------------------------------------------
void RunMessagePump()
{
    MSG queuedMessage;
    for (;;)
    {
        const BOOL wasMessagePresent = PeekMessage(&queuedMessage, NULL, 0, 0, PM_REMOVE);
        if (!wasMessagePresent)
        {
            break;
        }

        TranslateMessage(&queuedMessage);
        DispatchMessage(&queuedMessage);
    }
}

//-----------------------------------------------------------------------------------------------
void Update()
{
    static float MAX_FRAME_TIME = 0.04f;
    static double s_timeLastFrameStarted = GetCurrentTimeSeconds();
    double timeNow = GetCurrentTimeSeconds();
    float deltaSeconds = (float)(timeNow - s_timeLastFrameStarted);
    deltaSeconds = Clamp<float>(deltaSeconds, 0.0f, MAX_FRAME_TIME);
    s_timeLastFrameStarted = timeNow;

    if (InputSystem::instance->IsKeyDown('T'))
    {
        deltaSeconds /= 4.0f;
    }

    InputSystem::instance->Update(deltaSeconds);
    AudioSystem::instance->Update(deltaSeconds);
    Console::instance->Update(deltaSeconds);
    UISystem::instance->Update(deltaSeconds);
    TheGame::instance->Update(deltaSeconds);
}

//-----------------------------------------------------------------------------------------------
void Render()
{
    TheGame::instance->Render();
    UISystem::instance->Render();
    Console::instance->Render();
    SwapBuffers(g_displayDeviceContext);
}

//-----------------------------------------------------------------------------------------------
void RunFrame()
{
    InputSystem::instance->AdvanceFrameNumber();
    RunMessagePump();
    Update();
    Render();
}

//-----------------------------------------------------------------------------------------------
void Initialize(HINSTANCE applicationInstanceHandle)
{
    SetProcessDPIAware();
    CreateOpenGLWindow(applicationInstanceHandle);
    Renderer::instance = new Renderer();
    SpriteGameRenderer::instance = new SpriteGameRenderer(RGBA::CORNFLOWER_BLUE, WINDOW_PHYSICAL_WIDTH, WINDOW_PHYSICAL_HEIGHT, IMPORT_RESOLUTION, VIRTUAL_SIZE);
    AudioSystem::instance = new AudioSystem();
    InputSystem::instance = new InputSystem(g_hWnd, 4);
    Console::instance = new Console();
    UISystem::instance = new UISystem();
    UISystem::instance->LoadAndParseUIXML();
    TheGame::instance = new TheGame();
}

//TODO: Make an actual engine startup and shutdown.
//-----------------------------------------------------------------------------------
void EngineCleanup()
{
    Texture::CleanUpTextureRegistry();
    BitmapFont::CleanUpBitmapFontRegistry(); 
    EventSystem::CleanUpEventRegistry();
}

//-----------------------------------------------------------------------------------------------
void Shutdown()
{
    //Clean up all the engine subsystems.
    delete TheGame::instance;
    TheGame::instance = nullptr;
    delete UISystem::instance;
    UISystem::instance = nullptr;
    delete Console::instance;
    Console::instance = nullptr;
    delete InputSystem::instance;
    InputSystem::instance = nullptr;
    delete AudioSystem::instance;
    AudioSystem::instance = nullptr;
    delete SpriteGameRenderer::instance;
    SpriteGameRenderer::instance = nullptr;
    delete Renderer::instance;
    Renderer::instance = nullptr; 
    EngineCleanup();
}

//-----------------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int)
{
    UNUSED(commandLineString);
    MemoryAnalyticsStartup();
    LoggerStartup();
    Initialize(applicationInstanceHandle);
    while (!g_isQuitting)
    {
        RunFrame();
    }
    Shutdown();
    LoggerShutdown();
    MemoryAnalyticsShutdown();
    return 0;
}