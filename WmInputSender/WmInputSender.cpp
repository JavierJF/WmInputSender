#include "pch.h"

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <string>
#include <iostream>

// Global variables

// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Windows Desktop Guided Tour Application");

HINSTANCE hInst;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void MesssageLoop(HWND hWnd) {
    HGLOBAL hRaw = ::GlobalAlloc(GHND, sizeof(RAWINPUT));
    RAWINPUT *pRaw = reinterpret_cast<RAWINPUT*>(::GlobalLock(hRaw));

    char c = 'W';
    //header
    pRaw->header.dwSize = sizeof(RAWINPUT);
    pRaw->header.dwType = RIM_TYPEKEYBOARD;
    pRaw->header.wParam = 0; //(wParam & 0xff =0 => 0)
    pRaw->header.hDevice = 0;

    //data
    pRaw->data.keyboard.Reserved = 0;
    pRaw->data.keyboard.Flags = RI_KEY_MAKE;
    pRaw->data.keyboard.MakeCode = static_cast<WORD>(MapVirtualKeyEx(c, MAPVK_VK_TO_VSC, GetKeyboardLayout(0)));
    pRaw->data.keyboard.Message = WM_KEYDOWN;
    pRaw->data.keyboard.VKey = VkKeyScanEx(c, GetKeyboardLayout(0));
    pRaw->data.keyboard.ExtraInformation = 0;

    ::GlobalUnlock(hRaw);

    UINT dataSz { 0 };

    while (true) {
        Sleep(1000);
        SendMessage(hWnd, WM_INPUT, (WPARAM)RIM_INPUT, (LPARAM)hRaw);
    }
}

int CALLBACK WinMain(
   _In_ HINSTANCE hInstance,
   _In_opt_ HINSTANCE hPrevInstance,
   _In_ LPSTR     lpCmdLine,
   _In_ int       nCmdShow
)
{
   WNDCLASSEX wcex;

   wcex.cbSize = sizeof(WNDCLASSEX);
   wcex.style          = CS_HREDRAW | CS_VREDRAW;
   wcex.lpfnWndProc    = WndProc;
   wcex.cbClsExtra     = 0;
   wcex.cbWndExtra     = 0;
   wcex.hInstance      = hInstance;
   wcex.hIcon          = LoadIcon(hInstance, IDI_APPLICATION);
   wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
   wcex.lpszMenuName   = NULL;
   wcex.lpszClassName  = szWindowClass;
   wcex.hIconSm        = LoadIcon(wcex.hInstance, IDI_APPLICATION);

   if (!RegisterClassEx(&wcex))
   {
      MessageBox(NULL,
         _T("Call to RegisterClassEx failed!"),
         _T("Windows Desktop Guided Tour"),
         NULL);

      return 1;
   }

   // Store instance handle in our global variable
   hInst = hInstance;

   HWND hWnd = CreateWindow(
      szWindowClass,
      szTitle,
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT,
      500, 100,
      NULL,
      NULL,
      hInstance,
      NULL
   );

   if (!hWnd)
   {
      MessageBox(NULL,
         _T("Call to CreateWindow failed!"),
         _T("Windows Desktop Guided Tour"),
         NULL);

      return 1;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   DWORD threadID { 0 };
   HANDLE thHandle { NULL };
    
   thHandle = CreateThread(
       NULL,
       0,
       (LPTHREAD_START_ROUTINE)MesssageLoop,
       (LPVOID)hWnd,
       0,
       &threadID
   );

   // Main message loop:
   MSG msg;
   while (GetMessage(&msg, NULL, 0, 0))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
   WaitForSingleObject(thHandle, INFINITE);

   return (int)0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    TCHAR greeting[] = _T("Hello, Windows desktop!");
    UINT dataSz { 0 };
    HRESULT res { 0 };
 
    switch (message) {
        std::wcout << "message";
 
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
        
            TextOut(hdc, 5, 5, greeting, static_cast<int>(_tcslen(greeting)));
            EndPaint(hWnd, &ps);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_INPUT:
            res = GetRawInputData(
              (HRAWINPUT)lParam,
              RIM_INPUT,
              NULL,
              &dataSz,
              sizeof(RAWINPUTHEADER)
            );
            std::wcout << "WM_INPUT messsage: " << std::to_wstring(GetLastError()) << "\r\n";
        
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
            break;
    }

   return 0;
}

int wmain(int argc, wchar_t *argv[]) {
    std::cout << "Output standard\n";

    return WinMain(GetModuleHandle(NULL), NULL, GetCommandLineA(), SW_SHOWNORMAL);
}