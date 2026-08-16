#include <windows.h>
#include <mmsystem.h>
#include "Game.h"
#include <iostream>

#pragma comment(lib, "winmm.lib")

static Game* g_pGame = nullptr;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_SIZE:
        if (g_pGame) {
            int w = LOWORD(lParam);
            int h = HIWORD(lParam);
            g_pGame->resize(w, h);
        }
        return 0;

    case WM_KEYDOWN:
        if (g_pGame) {
            g_pGame->onKeyDown(wParam);
        }
        return 0;

    case WM_KEYUP:
        if (g_pGame) {
            g_pGame->onKeyUp(wParam);
        }
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        if (g_pGame) {
            g_pGame->render(hdc);
        }
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1; // Prevent flicker in double buffering

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Request 1ms timer resolution from OS for ultra-smooth 60 FPS frame pacing
    timeBeginPeriod(1);

    const char* CLASS_NAME = "ApexCyberDriveWindowClass";
    const char* WINDOW_TITLE = "Apex Cyber Drive 2D - Butter Smooth Precision Racing";

    WNDCLASSEXA wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClassExA(&wc)) {
        timeEndPeriod(1);
        MessageBoxA(NULL, "Failed to register window class.", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    int clientWidth = 1024;
    int clientHeight = 768;

    RECT rect = { 0, 0, clientWidth, clientHeight };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;

    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    int posX = (screenW - windowWidth) / 2;
    int posY = (screenH - windowHeight) / 2;

    HWND hwnd = CreateWindowExA(
        0,
        CLASS_NAME,
        WINDOW_TITLE,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        posX, posY, windowWidth, windowHeight,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) {
        timeEndPeriod(1);
        MessageBoxA(NULL, "Failed to create window.", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    Game game;
    g_pGame = &game;
    if (!game.init(hwnd, clientWidth, clientHeight)) {
        timeEndPeriod(1);
        MessageBoxA(NULL, "Failed to initialize game engine.", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // High resolution timer
    LARGE_INTEGER freq, lastTime, curTime;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&lastTime);

    MSG msg = { 0 };
    bool running = true;
    const double targetFrameTime = 1.0 / 60.0; // 60 FPS target

    while (running) {
        while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                running = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }

        if (!running) break;

        QueryPerformanceCounter(&curTime);
        float dt = (float)(curTime.QuadPart - lastTime.QuadPart) / (float)freq.QuadPart;
        
        // Frame limiter for consistent 60 FPS cadence
        if (dt < targetFrameTime) {
            double sleepMs = (targetFrameTime - dt) * 1000.0;
            if (sleepMs > 1.5) {
                Sleep((DWORD)(sleepMs - 1.0));
            }
            while (true) {
                QueryPerformanceCounter(&curTime);
                dt = (float)(curTime.QuadPart - lastTime.QuadPart) / (float)freq.QuadPart;
                if (dt >= targetFrameTime) break;
            }
        }

        lastTime = curTime;

        // Cap dt to eliminate physics spikes
        if (dt > 0.033f) dt = 0.033f;

        game.update(dt);

        // Repaint client area smoothly
        HDC hdc = GetDC(hwnd);
        game.render(hdc);
        ReleaseDC(hwnd, hdc);
    }

    game.shutdown();
    g_pGame = nullptr;
    timeEndPeriod(1);

    return (int)msg.wParam;
}

int main() {
    return WinMain(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_SHOW);
}
