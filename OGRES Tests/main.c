#include <windows.h>
#include <commdlg.h>
#include <stdio.h>
#include "ogres.h"

// Globals
HWND g_hWnd;
HBITMAP g_hBitmap = NULL;
ogres_t g_ogres = {0};
image_t g_currentImage = {0};
WORD g_currentLayer = 0;

// Forward declarations
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL LoadOgresFile(HWND hWnd);
void DisplayCurrentLayer(void);
void Cleanup(void);

int WINAPI 
WinMain(HINSTANCE hInstance, 
    HINSTANCE hPrevInstance, 
    PSTR szCmdLine, 
    int iCmdShow)
{
    static TCHAR szAppName[] = TEXT("OGRES Viewer");
    HWND hWnd;
    MSG msg;
    WNDCLASS wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass))
    {
        MessageBox(NULL, TEXT("Window registration failed!"), szAppName, MB_ICONERROR);
        return 0;
    }
    hWnd = CreateWindow(szAppName,
        TEXT("OGRES Viewer"),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        800,
        600,
        NULL,
        NULL,
        hInstance,
        NULL);

    g_hWnd = hWnd;

    ShowWindow(hWnd, iCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    Cleanup();
    return msg.wParam;
}

LRESULT CALLBACK 
WndProc(HWND hWnd, 
    UINT message, 
    WPARAM wParam, 
    LPARAM lParam)
{
    static HWND hOpenBtn, hPrevBtn, hNextBtn;

    switch (message)
    {
    case WM_CREATE:
        hOpenBtn = CreateWindow(TEXT("BUTTON"), TEXT("Open OGRES"),
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
            10, 10, 100, 30, hWnd, (HMENU)1, NULL, NULL);

        hPrevBtn = CreateWindow(TEXT("BUTTON"), TEXT("Previous"),
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_DISABLED,
            120, 10, 80, 30, hWnd, (HMENU)2, NULL, NULL);

        hNextBtn = CreateWindow(TEXT("BUTTON"), TEXT("Next"),
            WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON | WS_DISABLED,
            210, 10, 80, 30, hWnd, (HMENU)3, NULL, NULL);
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case 1: // Open
            if (LoadOgresFile(hWnd))
            {
                EnableWindow(hPrevBtn, FALSE);
                EnableWindow(hNextBtn, g_ogres.layer_count > 1);
                DisplayCurrentLayer();
            }
            break;

        case 2: // Previous
            if (g_currentLayer > 0)
            {
                g_currentLayer--;
                EnableWindow(hNextBtn, TRUE);
                if (g_currentLayer == 0)
                    EnableWindow(hPrevBtn, FALSE);
                DisplayCurrentLayer();
            }
            break;

        case 3: // Next
            if (g_currentLayer < g_ogres.layer_count - 1)
            {
                g_currentLayer++;
                EnableWindow(hPrevBtn, TRUE);
                if (g_currentLayer == g_ogres.layer_count - 1)
                    EnableWindow(hNextBtn, FALSE);
                DisplayCurrentLayer();
            }
            break;
        }
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        if (g_hBitmap)
        {
            HDC hdcMem = CreateCompatibleDC(hdc);
            HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, g_hBitmap);

            BITMAP bm;
            GetObject(g_hBitmap, sizeof(BITMAP), &bm);

            // Center the image
            RECT rc;
            GetClientRect(hWnd, &rc);
            int x = (rc.right - bm.bmWidth) / 2;
            int y = (rc.bottom - bm.bmHeight) / 2 + 50; // Below buttons

            BitBlt(hdc, x, y, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

            SelectObject(hdcMem, hOldBitmap);
            DeleteDC(hdcMem);

            // Display layer info
            TCHAR szInfo[100];
            wsprintf(szInfo, TEXT("Layer %d/%d (%dx%d)"), 
                g_currentLayer + 1, g_ogres.layer_count,
                g_currentImage.width, g_currentImage.height);
            TextOut(hdc, 10, 50, szInfo, lstrlen(szInfo));
        }

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

BOOL 
LoadOgresFile(HWND hWnd)
{
    OPENFILENAME ofn;
    TCHAR szFile[MAX_PATH] = {0};
    char narrowPath[MAX_PATH] = {0};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = TEXT("OGRES Files\0*.ogres\0All Files\0*.*\0");
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (!GetOpenFileName(&ofn))
        return FALSE;

    Cleanup();

    // convert wide char to multibyte
    WideCharToMultiByte(CP_ACP, 0, ofn.lpstrFile, -1, 
        narrowPath, MAX_PATH, NULL, NULL);

    printf("Trying to open: %s\n", narrowPath);

    if (!OGRES_Load(&g_ogres, narrowPath))
    {
        MessageBox(hWnd, TEXT("Failed to load OGRES file\nCheck if file is valid and accessible"), 
            TEXT("Error"), MB_ICONERROR);
        return FALSE;
    }
    g_currentLayer = 0;
    return TRUE;
}

void 
DisplayCurrentLayer(void)
{
    // free previous resources
    if (g_hBitmap)
    {
        DeleteObject(g_hBitmap);
        g_hBitmap = NULL;
    }
    OGRES_FreeImage(&g_currentImage);

    if (!OGRES_GetImage(&g_ogres, &g_currentImage, g_currentLayer))
    {
        MessageBox(g_hWnd, TEXT("Failed to load layer"), TEXT("Error"), MB_ICONERROR);
        return;
    }
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = g_currentImage.width;
    bmi.bmiHeader.biHeight = -g_currentImage.height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC hdc = GetDC(g_hWnd);
    g_hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, NULL, NULL, 0);
    ReleaseDC(g_hWnd, hdc);

    if (!g_hBitmap)
    {
        MessageBox(g_hWnd, TEXT("Failed to create bitmap"), TEXT("Error"), MB_ICONERROR);
        return;
    }
    // convert BGR to RGB
    BITMAP bm;
    GetObject(g_hBitmap, sizeof(BITMAP), &bm);
    BYTE* pBits = (BYTE*)bm.bmBits;

    for (int y = 0; y < g_currentImage.height; y++)
    {
        for (int x = 0; x < g_currentImage.width; x++)
        {
            pixel_t* pixel = &g_currentImage.pixels[y * g_currentImage.width + x];
            BYTE* dest = &pBits[(y * g_currentImage.width + x) * 3];

            // BGR to RGB conversion
            dest[0] = pixel->r;
            dest[1] = pixel->g;
            dest[2] = pixel->b;
        }
    }
    InvalidateRect(g_hWnd, NULL, TRUE);
}

void 
Cleanup(void)
{
    if (g_hBitmap)
    {
        DeleteObject(g_hBitmap);
        g_hBitmap = NULL;
    }

    OGRES_FreeImage(&g_currentImage);
    OGRES_Free(&g_ogres);

    ZeroMemory(&g_currentImage, sizeof(g_currentImage));
    ZeroMemory(&g_ogres, sizeof(g_ogres));
}