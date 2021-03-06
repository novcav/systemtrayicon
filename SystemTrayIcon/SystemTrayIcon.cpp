//
// This is a program to put an icon in system tray.
//
// To show a window         - double click icon in system tray.
// To hide a window         - click X on the upper right corner of the window.
// To exit program          - right click icon and select exit menu.
// To show about dialog box - right click icon and select about menu.
//
#include "stdafx.h"
#include "SystemTrayIcon.h"

#define MAX_LOADSTRING 100
#define MAX_LINE 4
#define MAX_CHAR 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

//
wchar_t str[MAX_LINE][MAX_CHAR];
NOTIFYICONDATA nid = {};

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

void OnLButtonDoubleClick(HWND hWnd);
void OnContextMenu(HWND hWnd, int x, int y);

void OnPaint(HWND hWnd);
void OnCreate(HWND hWnd);
void OnDestroy(HWND hWnd);
void OnClose(HWND hWnd);

void OnFileAbout(HWND hWnd, HINSTANCE hInstance);
void OnFileExit(HWND hWnd);


// main window
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SYSTEMTRAYICON, szWindowClass, MAX_LOADSTRING);

    //  Registers the window class.
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SYSTEMTRAYICON));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SYSTEMTRAYICON);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);

    // Store instance handle in our global variable.
    hInst = hInstance; 

    // Create the main program window.
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) return FALSE;

    // Display the main program window.
    //ShowWindow(hWnd, nCmdShow);
	ShowWindow(hWnd, SW_HIDE);
    UpdateWindow(hWnd);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SYSTEMTRAYICON));

    MSG msg;

    // Main message loop
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

//  Processes messages for the main window.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_SYSTEM_TRAY_ICON:
		switch (LOWORD(lParam))
		{
		case WM_LBUTTONDBLCLK: OnLButtonDoubleClick(hWnd);                          break;
		case WM_CONTEXTMENU:   OnContextMenu(hWnd, LOWORD(wParam), HIWORD(wParam)); break;
		}
		break;
    case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_ABOUT: OnFileAbout(hWnd, hInst);				break;
		case IDM_EXIT:  OnFileExit(hWnd);						break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
        break;
	case WM_PAINT:   OnPaint(hWnd);									break;
	case WM_CREATE:  OnCreate(hWnd);								break;
	case WM_DESTROY: OnDestroy(hWnd);								break;
	case WM_CLOSE:   OnClose(hWnd);                                 break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }

    return (INT_PTR)FALSE;
}

// show window
void OnLButtonDoubleClick(HWND hWnd)
{
	ShowWindow(hWnd, SW_SHOW);
}

// display pop-up menu at position x, y
void OnContextMenu(HWND hWnd, int x, int y)
{
	HMENU hMenu;
	HMENU hSubMenu;
 
	hMenu = GetMenu(hWnd);
	hSubMenu = GetSubMenu(hMenu, 0);
	TrackPopupMenu(hSubMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, x, y, 0, hWnd, NULL);
}

//
void OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;
	HDC hDC;
	HFONT hFont;
	TEXTMETRIC tm;
	int i, x, y, d;

	x = y = 96;

	hDC = BeginPaint(hWnd, &ps);

	// Create and set font.
	hFont = CreateFont(-16, 0, 0, 0, FW_REGULAR, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_MODERN, L"Courier New");
	SelectObject(hDC,hFont);

	// get spacing between lines
	GetTextMetrics(hDC, &tm); 
	d = tm.tmHeight + tm.tmExternalLeading;

	for (i = 0; i < MAX_LINE; i++) {
		TextOut(hDC, x, y, str[i], wcslen(str[i]));
		y += d;
	}

	// Delete font.
	DeleteObject(hFont);

	EndPaint(hWnd, &ps);
}

//
void OnCreate(HWND hWnd)
{
	wcscpy_s(str[0], MAX_CHAR, L"To show this window      - double click icon in system tray");
	wcscpy_s(str[1], MAX_CHAR, L"To hide this window      - click X on the upper right corner of the window");
	wcscpy_s(str[2], MAX_CHAR, L"To show about dialog box - right click icon and select about menu");
	wcscpy_s(str[3], MAX_CHAR, L"To exit program          - right click icon and select exit menu");

	// add icon in system tray
    nid.cbSize           = sizeof(nid);
    nid.hWnd             = hWnd;
	nid.uID              = IDI_SYSTEMTRAYICON;
    nid.uFlags           =  NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_SHOWTIP;
	nid.hIcon            = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SYSTEMTRAYICON));
    nid.uCallbackMessage = WM_SYSTEM_TRAY_ICON;
	nid.uVersion         = NOTIFYICON_VERSION_4;
	wcscpy_s(nid.szTip,128,L"program icon in system tray");

	Shell_NotifyIcon(NIM_ADD, &nid);
	Shell_NotifyIcon(NIM_SETVERSION, &nid);
}

//
void OnDestroy(HWND hWnd)
{
	// remove icon from system tray
	Shell_NotifyIcon(NIM_DELETE, &nid);

	PostQuitMessage(0);
}

// hide window
void OnClose(HWND hWnd)
{
	ShowWindow(hWnd, SW_HIDE);
}

// display about dialog box
void OnFileAbout(HWND hWnd, HINSTANCE hInstance)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, DlgProc);
}

//
void OnFileExit(HWND hWnd)
{
	DestroyWindow(hWnd);
}

