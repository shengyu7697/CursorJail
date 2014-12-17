// CursorJail.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "CursorJail.h"
#include <stdio.h>
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
int limit = 0;
int resolutionW;
int resolutionH;
POINT p1 = {10, 10};
POINT p2 = {400, 200};
HMENU hMenu;
HWND hStatic1;
HWND hStatic2;
HWND hStatic3;
HWND hEdit1;
HWND hEdit2;
HWND hEdit3;
HWND hEdit4;
HWND hDlgCurrent = NULL;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void GetCurrentSetting(POINT *point1, POINT *point2);
void SetCurrentSetting(POINT point1, POINT point2);
void LoadSetting(POINT *point1, POINT *point2);
void SaveSetting(POINT point1, POINT point2);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CURSORJAIL, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CURSORJAIL));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (NULL == hDlgCurrent || !IsDialogMessage(hDlgCurrent, &msg))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CURSORJAIL));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CURSORJAIL);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      100, 100, 230, 180, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR buf[256];

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_START:
			limit = 1;
			EnableMenuItem(hMenu, IDM_START, MF_GRAYED);
			EnableMenuItem(hMenu, IDM_STOP, MF_ENABLED);
			SetTimer(hWnd, 1, 100, NULL);
			break;
		case IDM_STOP:
			limit = 0;
			EnableMenuItem(hMenu, IDM_START, MF_ENABLED);
			EnableMenuItem(hMenu, IDM_STOP, MF_GRAYED);
			KillTimer(hWnd, 1);
			ClipCursor(NULL);
			break;
		case IDM_SAVE:
			GetCurrentSetting(&p1, &p2);
			SaveSetting(p1, p2);
			SetWindowText(hWnd, TEXT("Saved"));
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_TIMER:
		switch(wParam)
		{
		case 1: // Limit timer
			{
			GetCurrentSetting(&p1, &p2);
			RECT rect = {p1.x, p1.y, p2.x, p2.y};
			ClipCursor(&rect);
			}
			break;
		case 2: // Cursor state timer
			{
			POINT point;
			GetCursorPos(&point);
			wsprintf(buf, TEXT("CursorPos: (%d,%d)\nResolution: %dx%d\nState: %s"),
				point.x, point.y,
				resolutionW, resolutionH,
				limit?TEXT("On"):TEXT("Off"));
			SetWindowText(hStatic3, buf);
			}
			break;
		}
		break;
	case WM_HOTKEY:
		switch(wParam)
		{
		case 1:
			SendMessage(hWnd, WM_COMMAND, IDM_START, 0);
			break;
		case 2:
			SendMessage(hWnd, WM_COMMAND, IDM_STOP, 0);
			break;
		}
		break;
	case WM_ACTIVATE:
		if (0 == wParam)	// becoming inactive
			hDlgCurrent = NULL;
		else				// becoming active
			hDlgCurrent = hWnd;
		break;
	case WM_SETFOCUS:
		SetFocus(hEdit1);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_CREATE:
		{
		// Initialize something.
		hMenu = GetMenu(hWnd);
		hStatic1 = CreateWindowEx(NULL, TEXT("STATIC"), TEXT("UpperLeft"),
			WS_CHILD | WS_VISIBLE, 10, 5, 80, 24,
			hWnd, NULL, LPCREATESTRUCT(lParam)->hInstance, NULL);
		hStatic2 = CreateWindowEx(NULL, TEXT("STATIC"), TEXT("LowerRight"),
			WS_CHILD | WS_VISIBLE, 10, 35, 80, 24,
			hWnd, NULL, LPCREATESTRUCT(lParam)->hInstance, NULL);
		hStatic3 = CreateWindowEx(NULL, TEXT("STATIC"), TEXT(""),
			WS_CHILD | WS_VISIBLE, 10, 65, 200, 48,
			hWnd, NULL, LPCREATESTRUCT(lParam)->hInstance, NULL);
		hEdit1 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""),
			WS_CHILD | ES_NUMBER | WS_TABSTOP | WS_VISIBLE, 80, 5, 40, 24,
			hWnd, NULL, LPCREATESTRUCT(lParam)->hInstance, NULL);
		hEdit2 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""),
			WS_CHILD | ES_NUMBER | WS_TABSTOP | WS_VISIBLE, 125, 5, 40, 24,
			hWnd, NULL, LPCREATESTRUCT(lParam)->hInstance, NULL);
		hEdit3 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""),
			WS_CHILD | ES_NUMBER | WS_TABSTOP | WS_VISIBLE, 80, 35, 40, 24,
			hWnd, NULL, LPCREATESTRUCT(lParam)->hInstance, NULL);
		hEdit4 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""),
			WS_CHILD | ES_NUMBER | WS_TABSTOP | WS_VISIBLE, 125, 35, 40, 24,
			hWnd, NULL, LPCREATESTRUCT(lParam)->hInstance, NULL);

		// Set font.
		int PointSize = 10;
		hdc = GetDC(hWnd);
		int nHeight = -MulDiv(PointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
		ReleaseDC(hWnd, hdc);
		HFONT hFont = CreateFont(nHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH, TEXT("Arial"));
		SendMessage(hStatic1, WM_SETFONT, (WPARAM)hFont, FALSE);
		SendMessage(hStatic2, WM_SETFONT, (WPARAM)hFont, FALSE);
		SendMessage(hStatic3, WM_SETFONT, (WPARAM)hFont, FALSE);
		SendMessage(hEdit1, WM_SETFONT, (WPARAM)hFont, FALSE);
		SendMessage(hEdit2, WM_SETFONT, (WPARAM)hFont, FALSE);
		SendMessage(hEdit3, WM_SETFONT, (WPARAM)hFont, FALSE);
		SendMessage(hEdit4, WM_SETFONT, (WPARAM)hFont, FALSE);

		// Load setting.
		LoadSetting(&p1, &p2);
		SetCurrentSetting(p1, p2);

		// Get resolution.
		resolutionW = GetSystemMetrics(SM_CXSCREEN);
		resolutionH = GetSystemMetrics(SM_CYSCREEN);

		// Register hot key, Ctrl+F1 and Ctrl+F2.
		RegisterHotKey(hWnd, 1, MOD_CONTROL, VK_F1);
		RegisterHotKey(hWnd, 2, MOD_CONTROL, VK_F2);

		// Send stop limit msg.
		SendMessage(hWnd, WM_COMMAND, IDM_STOP, 0);
		SetTimer(hWnd, 2, 50, NULL);

		// Set focus
		SetFocus(hEdit1);
		}
		break;
	case WM_DESTROY:
		ClipCursor(NULL);
		KillTimer(hWnd, 1);
		KillTimer(hWnd, 2);
		UnregisterHotKey(hWnd, 1);
		UnregisterHotKey(hWnd, 2);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

void GetCurrentSetting(POINT *point1, POINT *point2)
{
	TCHAR buf[256];
	GetWindowText(hEdit1, buf, sizeof(buf));
	point1->x = _ttoi(buf);
	GetWindowText(hEdit2, buf, sizeof(buf));
	point1->y = _ttoi(buf);
	GetWindowText(hEdit3, buf, sizeof(buf));
	point2->x = _ttoi(buf);
	GetWindowText(hEdit4, buf, sizeof(buf));
	point2->y = _ttoi(buf);
}

void SetCurrentSetting(POINT point1, POINT point2)
{
	TCHAR buf[256];
	wsprintf(buf, TEXT("%d"), point1.x);
	SetWindowText(hEdit1, buf);
	wsprintf(buf, TEXT("%d"), point1.y);
	SetWindowText(hEdit2, buf);
	wsprintf(buf, TEXT("%d"), point2.x);
	SetWindowText(hEdit3, buf);
	wsprintf(buf, TEXT("%d"), point2.y);
	SetWindowText(hEdit4, buf);
}

void LoadSetting(POINT *point1, POINT *point2)
{
	FILE *fp;
	fp = fopen("setting.cfg", "r");
	if (fp != NULL)
	{
		fscanf(fp, "%d, %d", &point1->x, &point1->y);
		fscanf(fp, "%d, %d", &point2->x, &point2->y);
		fclose(fp);
	}
}

void SaveSetting(POINT point1, POINT point2)
{
	FILE *fp;
	fp = fopen("setting.cfg", "w");
	if (fp != NULL)
	{
		fprintf(fp, "%d, %d\n", point1.x, point1.y);
		fprintf(fp, "%d, %d\n", point2.x, point2.y);
		fclose(fp);
	}
}
