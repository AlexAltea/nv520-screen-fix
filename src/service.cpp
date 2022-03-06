/**
 * (c) 2022 Alexandro Sanchez Bach
 * Released under MIT license. Read LICENSE for more details.
 */

#include <cctype>
#include <cstdio>
#include <vector>
#include <string>

#include <Windows.h>

// Configuration
#define NV520_SERVICE    L"Nv520ScreenFix"
#define NV520_DURATION   (120*1000)  // Run for two minutes
#define NV520_FREQUENCY  (3*1000)    // Every three seconds

#ifdef _DEBUG
#define LOGD(msg, ...) printf(msg "\n", __VA_ARGS__)
#else
#define LOGD
#endif

void ApplyTopologyFix(ULONGLONG msDuration = NV520_DURATION, DWORD msFrequency = NV520_FREQUENCY)
{
    HRESULT hr;

    // Allocate display configuration buffers
    UINT32 countPath = 0;
    UINT32 countMode = 0;
    hr = GetDisplayConfigBufferSizes(QDC_ALL_PATHS, &countPath, &countMode);
    std::vector<DISPLAYCONFIG_PATH_INFO> pathArray(countPath);
    std::vector<DISPLAYCONFIG_MODE_INFO> modeArray(countMode);

    // Run at least once, for the specified duration
    auto stop = GetTickCount64() + msDuration;
    do {
        // Query display configuration
        DISPLAYCONFIG_TOPOLOGY_ID currentMode = {};
        hr = QueryDisplayConfig(QDC_DATABASE_CURRENT, &countPath, &pathArray[0], &countMode, &modeArray[0], &currentMode);
        if (hr != ERROR_SUCCESS) {
            LOGD("Could not query display configuration");
        }

        // Revert unusual topologies to clone
        if (currentMode != DISPLAYCONFIG_TOPOLOGY_CLONE &&
            currentMode != DISPLAYCONFIG_TOPOLOGY_EXTEND) {
            SetDisplayConfig(0, NULL, 0, NULL, (SDC_APPLY | SDC_TOPOLOGY_CLONE));
            break;
        }

        // Wait a bit, and check if we are finished
        Sleep(msFrequency);
    } while (stop > GetTickCount64());
}

LRESULT WINAPI HiddenWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_POWERBROADCAST:
        // Waking-up from sleep
        if (wParam == PBT_APMRESUMEAUTOMATIC) {
            ApplyTopologyFix();
        }
        break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
    HINSTANCE instance = GetModuleHandle(NULL);

    // Attempt to fix topology right after startup
    ApplyTopologyFix();

    // Define empty window class
    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = &HiddenWindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = instance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = NV520_SERVICE;
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    // Create empty window
    RegisterClassEx(&wcex);
    HWND hwnd = CreateWindow(NV520_SERVICE, NV520_SERVICE, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, instance, NULL);
    ShowWindow(hwnd, SW_HIDE);
    UpdateWindow(hwnd);

    // Process messages
    MSG msg = {};
    while (GetMessage(&msg, NULL, NULL, NULL)) {
        if (msg.message == WM_QUIT) {
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
