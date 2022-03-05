/**
 * (c) 2022 Alexandro Sanchez Bach
 * Released under MIT license. Read LICENSE for more details.
 */

#include <cctype>
#include <cstdio>

#include <Windows.h>

#ifdef _DEBUG
#define LOGD(msg, ...) printf(msg "\n", __VA_ARGS__)
#else
#define LOGD
#endif

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
    return 0;
}
