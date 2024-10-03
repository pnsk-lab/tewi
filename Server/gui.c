/* $Id$ */

#include "../config.h"

#include "gui.h"

#include <windows.h>
#include <commctrl.h>

HINSTANCE hInst;
HBRUSH pbtewi_brush;
HWND logarea;
HWND button_start;
HWND button_stop;
HWND button_about;
HWND status;
BOOL tewi_alive;
BOOL was_starting;
BOOL exiting;

#define WINWIDTH(rc) (rc.right - rc.left)
#define WINHEIGHT(rc) (rc.bottom - rc.top)

#define DIALOG_X(n) (HIWORD(GetDialogBaseUnits()) * n)
#define DIALOG_Y(n) (LOWORD(GetDialogBaseUnits()) * n)

void ShowBitmapSize(HWND hWnd, HDC hdc, const char* name, int x, int y, int w, int h){
	HBITMAP hBitmap = LoadBitmap(hInst, name);
	BITMAP bmp;
	HDC hmdc;
	GetObject(hBitmap, sizeof(bmp), &bmp);
	hmdc = CreateCompatibleDC(hdc);
	SelectObject(hmdc, hBitmap);
	if(w == 0 && h == 0){
		StretchBlt(hdc, x, y, bmp.bmWidth, bmp.bmHeight, hmdc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
	}else{
		StretchBlt(hdc, x, y, w, h, hmdc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
	}
	DeleteDC(hmdc);
	DeleteObject(hBitmap);
}

void ShowBitmap(HWND hWnd, HDC hdc, const char* name, int x, int y){
	ShowBitmapSize(hWnd, hdc, name, x, y, 0, 0);
}

LRESULT CALLBACK VersionDialog(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp){
	if(msg == WM_COMMAND){
		if(LOWORD(wp) == IDOK) EndDialog(hWnd, IDOK);
	}else if(msg == WM_PAINT){
		HDC hdc;
		PAINTSTRUCT ps;
		RECT size;

		size.left = size.top = 5;
		size.right = size.bottom = 32 + 5;
		MapDialogRect(hWnd, &size);

		hdc = BeginPaint(hWnd, &ps);
		ShowBitmapSize(hWnd, hdc, "TEWILOGO", size.left, size.top, WINWIDTH(size), WINWIDTH(size));
		EndPaint(hWnd, &ps);
	}else if(msg == WM_CTLCOLORDLG || msg == WM_CTLCOLORSTATIC){
		HDC dc = (HDC)wp;
		SetBkMode(dc, TRANSPARENT);
		return GetSysColorBrush(COLOR_MENU);
	}else{
		return FALSE;
	}
	return TRUE;
}

void StartTewi(void){
	was_starting = TRUE;
}

void StopTewi(void){
	was_starting = TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp){
	if(msg == WM_COMMAND){
		int trig = LOWORD(wp);
		int ev = HIWORD(wp);
		if(trig == GUI_BUTTON_ABOUT){
			if(ev == BN_CLICKED){
				DialogBox(hInst, "VERSIONDLG", hWnd, (DLGPROC)VersionDialog);
			}
		}else if(trig == GUI_BUTTON_START){
			if(ev == BN_CLICKED){
				SendMessage(status, SB_SETTEXT, 0, (LPARAM)"Starting Tewi HTTPd");
				StartTewi();
			}
		}else if(trig == GUI_BUTTON_STOP){
			if(ev == BN_CLICKED){
				SendMessage(status, SB_SETTEXT, 0, (LPARAM)"Stopping Tewi HTTPd");
				StopTewi();
			}
		}else if(trig == GUI_BUTTON_EXIT){
			if(ev == BN_CLICKED){
				if(tewi_alive){
					SendMessage(status, SB_SETTEXT, 0, (LPARAM)"Stopping Tewi HTTPd");
					StopTewi();
					exiting = TRUE;
				}else{
					SendMessage(hWnd, WM_CLOSE, 0, 0);
				}
			}
		}else if(trig == GUI_LOG){
		}
	}else if(msg == WM_CLOSE){
		DestroyWindow(hWnd);
	}else if(msg == WM_DESTROY){
		DeleteObject(pbtewi_brush);
		PostQuitMessage(0);
	}else if(msg == WM_CREATE){
		RECT rc, src;
		GetClientRect(hWnd, &rc);

		InitCommonControls();

		status = CreateStatusWindow(WS_CHILD | WS_VISIBLE | CCS_BOTTOM, NULL, hWnd, GUI_STATUS);
		SendMessage(status, SB_SIMPLE, 0, 0);
		SendMessage(status, SB_SETTEXT, 0, (LPARAM)"Welcome to Tewi HTTPd");
		SendMessage(status, SB_GETRECT, 0, (LPARAM)&src);

		pbtewi_brush = CreateSolidBrush(RGB(0xf7, 0xc9, 0xf3));
		button_start = CreateWindow("BUTTON", "&Start", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, WINWIDTH(rc) - 100, 20 * 0, 100, 20, hWnd, (HMENU)GUI_BUTTON_START, hInst, NULL);
		button_stop = CreateWindow("BUTTON", "S&top", WS_CHILD | WS_VISIBLE | WS_DISABLED | BS_PUSHBUTTON, WINWIDTH(rc) - 100, 20 * 1, 100, 20, hWnd, (HMENU)GUI_BUTTON_STOP, hInst, NULL);
		button_about = CreateWindow("BUTTON", "&About", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, WINWIDTH(rc) - 100, 20 * 2, 100, 20, hWnd, (HMENU)GUI_BUTTON_ABOUT, hInst, NULL);
		button_about = CreateWindow("BUTTON", "E&xit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, WINWIDTH(rc) - 100, WINHEIGHT(rc) - WINHEIGHT(src) - 20, 100, 20, hWnd, (HMENU)GUI_BUTTON_EXIT, hInst, NULL);
		logarea = CreateWindow("LISTBOX", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL, 0, 40, WINWIDTH(rc) - 100, WINHEIGHT(rc) - 40 - WINHEIGHT(src), hWnd, (HMENU)GUI_LOG, hInst, NULL);
		SetTimer(hWnd, TIMER_WATCH_TEWI, 100, NULL);
	}else if(msg == WM_TIMER){
		if(wp == TIMER_WATCH_TEWI){
			if(tewi_alive){
				if(was_starting){
					was_starting = FALSE;
					SendMessage(status, SB_SETTEXT, 0, (LPARAM)"Started Tewi HTTPd");
				}
				EnableWindow(button_start, FALSE);
				EnableWindow(button_stop, TRUE);
			}else{
				if(was_starting){
					was_starting = FALSE;
					SendMessage(status, SB_SETTEXT, 0, (LPARAM)"Stopped Tewi HTTPd");
				}
				EnableWindow(button_start, TRUE);
				EnableWindow(button_stop, FALSE);
				if(exiting){
					KillTimer(hWnd, TIMER_WATCH_TEWI);
					SendMessage(hWnd, WM_CLOSE, 0, 0);
				}
			}
		}
	}else if(msg == WM_PAINT){
		HDC hdc;
		PAINTSTRUCT ps;
		RECT rc;
		RECT fill;

		GetClientRect(hWnd, &rc);
		hdc = BeginPaint(hWnd, &ps);
		SetRect(&fill, 0, 0, WINWIDTH(rc), 40);
		FillRect(hdc, &fill, pbtewi_brush);
		ShowBitmap(hWnd, hdc, "PBTEWI", 0, 0);
		EndPaint(hWnd, &ps);
	}else{
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return 0;
}

BOOL InitApp(void){
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(hInst, "TEWI");
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetSysColorBrush(COLOR_MENU);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "tewihttpd";
	return RegisterClass(&wc);
}

BOOL InitWindow(int nCmdShow){
	HWND hWnd;
	RECT deskrc, rc;
	HWND hDeskWnd = GetDesktopWindow();
	GetWindowRect(hDeskWnd, &deskrc);
	hWnd = CreateWindow("tewihttpd", "Tewi HTTPd", (WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME) ^ WS_MAXIMIZEBOX, 0, 0, 600, 200, NULL, 0, hInst, NULL);

	if(!hWnd){
		return FALSE;
	}
	GetWindowRect(hWnd, &rc);
	SetWindowPos(hWnd, HWND_TOP, (deskrc.right - (rc.right - rc.left)) / 2, (deskrc.bottom - (rc.bottom - rc.top)) / 2, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow){
	MSG msg;
	BOOL bret;
	hInst = hCurInst;
	tewi_alive = FALSE;
	was_starting = FALSE;
	exiting = FALSE;
	if(!InitApp()){
		return FALSE;
	}
	if(!InitWindow(nCmdShow)){
		return FALSE;
	}

	while((bret = GetMessage(&msg, NULL, 0, 0)) != 0){
		if(bret == -1) {
			break;
		} else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}
