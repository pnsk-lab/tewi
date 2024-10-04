/* $Id$ */

#include "../config.h"

#include "gui.h"
#include "tw_server.h"

#include <cm_log.h>

#include <stdio.h>
#include <windows.h>
#include <process.h>
#include <commctrl.h>

HINSTANCE hInst;
HBRUSH pbtewi_brush;
HWND logarea;
HWND button_start;
HWND button_stop;
HWND button_about;
HWND button_reset;
HWND button_exit;
HWND status;
HFONT monospace;
BOOL tewi_alive;
BOOL was_starting;
BOOL exiting;
BOOL idle;
extern FILE* logfile;
extern int running;

#define WINWIDTH(rc) (rc.right - rc.left)
#define WINHEIGHT(rc) (rc.bottom - rc.top)

int startup(int argc, char** argv);

void ShowBitmapSize(HWND hWnd, HDC hdc, const char* name, int x, int y, int w, int h) {
	HBITMAP hBitmap = LoadBitmap(hInst, name);
	BITMAP bmp;
	HDC hmdc;
	GetObject(hBitmap, sizeof(bmp), &bmp);
	hmdc = CreateCompatibleDC(hdc);
	SelectObject(hmdc, hBitmap);
	if(w == 0 && h == 0) {
		StretchBlt(hdc, x, y, bmp.bmWidth, bmp.bmHeight, hmdc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
	} else {
		StretchBlt(hdc, x, y, w, h, hmdc, 0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
	}
	DeleteDC(hmdc);
	DeleteObject(hBitmap);
}

void ShowBitmap(HWND hWnd, HDC hdc, const char* name, int x, int y) { ShowBitmapSize(hWnd, hdc, name, x, y, 0, 0); }

int max = 0;
void AddLog(const char* str) {
	HDC hdc;
	PAINTSTRUCT ps;
	SIZE sz;

	SendMessage(logarea, LB_ADDSTRING, 0, (LPARAM)str);

	hdc = CreateCompatibleDC(NULL);
	SelectObject(hdc, monospace);
	GetTextExtentPoint32(hdc, str, strlen(str), &sz);
	DeleteDC(hdc);

	if(max < sz.cx) {
		max = sz.cx;
		SendMessage(logarea, LB_SETHORIZONTALEXTENT, max, 0);
	}
}

LRESULT CALLBACK VersionDialog(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	if(msg == WM_COMMAND) {
		if(LOWORD(wp) == IDOK) EndDialog(hWnd, IDOK);
	} else if(msg == WM_PAINT) {
		HDC hdc;
		PAINTSTRUCT ps;
		RECT size;

		size.left = size.top = 5;
		size.right = size.bottom = 32 + 5;
		MapDialogRect(hWnd, &size);

		hdc = BeginPaint(hWnd, &ps);
		ShowBitmapSize(hWnd, hdc, "TEWILOGO", size.left, size.top, WINWIDTH(size), WINWIDTH(size));
		EndPaint(hWnd, &ps);
	} else if(msg == WM_CTLCOLORDLG || msg == WM_CTLCOLORSTATIC) {
		HDC dc = (HDC)wp;
		SetBkMode(dc, TRANSPARENT);
		return (LRESULT)GetSysColorBrush(COLOR_MENU);
	} else {
		return FALSE;
	}
	return TRUE;
}

void tewi_thread(void* ptr) {
	int st = startup(0, NULL);
	was_starting = TRUE;
	if(st == -1) {
		tewi_alive = TRUE;
		idle = FALSE;
	} else {
		cm_force_log("Config error");
		idle = FALSE;
		_endthread();
	}
	running = 1;
	tw_server_loop();
	tewi_alive = FALSE;
	was_starting = TRUE;
	idle = FALSE;
	_endthread();
}

void StartTewi(void) {
	EnableWindow(button_start, FALSE);
	EnableWindow(button_stop, FALSE);
	_beginthread(tewi_thread, 0, NULL);
}

void StopTewi(void) {
	EnableWindow(button_start, FALSE);
	EnableWindow(button_stop, FALSE);
	running = 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
	if(msg == WM_COMMAND) {
		int trig = LOWORD(wp);
		int ev = HIWORD(wp);
		if(trig == GUI_BUTTON_ABOUT) {
			if(ev == BN_CLICKED) {
				DialogBox(hInst, "VERSIONDLG", hWnd, (DLGPROC)VersionDialog);
			}
		} else if(trig == GUI_BUTTON_START) {
			if(ev == BN_CLICKED) {
				SendMessage(status, SB_SETTEXT, 0, (LPARAM) "Starting Tewi HTTPd");
				StartTewi();
			}
		} else if(trig == GUI_BUTTON_STOP) {
			if(ev == BN_CLICKED) {
				SendMessage(status, SB_SETTEXT, 0, (LPARAM) "Stopping Tewi HTTPd");
				StopTewi();
			}
		} else if(trig == GUI_BUTTON_RESET) {
			if(ev == BN_CLICKED) {
				SendMessage(logarea, LB_RESETCONTENT, 0, 0);
				max = 0;
				SendMessage(logarea, LB_SETHORIZONTALEXTENT, max, 0);
			}
		} else if(trig == GUI_BUTTON_EXIT) {
			if(ev == BN_CLICKED) {
				if(tewi_alive) {
					SendMessage(status, SB_SETTEXT, 0, (LPARAM) "Stopping Tewi HTTPd");
					StopTewi();
					exiting = TRUE;
				} else {
					SendMessage(hWnd, WM_CLOSE, 0, 0);
				}
			}
		} else if(trig == GUI_LOG) {
		}
	} else if(msg == WM_CLOSE) {
		DestroyWindow(hWnd);
	} else if(msg == WM_DESTROY) {
		DeleteObject(pbtewi_brush);
		PostQuitMessage(0);
	} else if(msg == WM_CREATE) {
		RECT rc, src;
		GetClientRect(hWnd, &rc);

		InitCommonControls();

		monospace = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);

		status = CreateStatusWindow(WS_CHILD | WS_VISIBLE | CCS_BOTTOM, NULL, hWnd, GUI_STATUS);
		SendMessage(status, SB_SIMPLE, 0, 0);
		SendMessage(status, SB_SETTEXT, 0, (LPARAM) "Welcome to Tewi HTTPd");
		SendMessage(status, SB_GETRECT, 0, (LPARAM)&src);

		pbtewi_brush = CreateSolidBrush(RGB(0xf7, 0xc9, 0xf3));
		button_start = CreateWindow("BUTTON", "&Start", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, WINWIDTH(rc) - 100, 20 * 0, 100, 20, hWnd, (HMENU)GUI_BUTTON_START, hInst, NULL);
		button_stop = CreateWindow("BUTTON", "S&top", WS_CHILD | WS_VISIBLE | WS_DISABLED | BS_PUSHBUTTON, WINWIDTH(rc) - 100, 20 * 1, 100, 20, hWnd, (HMENU)GUI_BUTTON_STOP, hInst, NULL);
		button_about = CreateWindow("BUTTON", "&About", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, WINWIDTH(rc) - 100, 20 * 2, 100, 20, hWnd, (HMENU)GUI_BUTTON_ABOUT, hInst, NULL);
		button_reset = CreateWindow("BUTTON", "&Reset", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, WINWIDTH(rc) - 100, WINHEIGHT(rc) - WINHEIGHT(src) - 20 - 20, 100, 20, hWnd, (HMENU)GUI_BUTTON_RESET, hInst, NULL);
		button_exit = CreateWindow("BUTTON", "E&xit", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, WINWIDTH(rc) - 100, WINHEIGHT(rc) - WINHEIGHT(src) - 20, 100, 20, hWnd, (HMENU)GUI_BUTTON_EXIT, hInst, NULL);
		logarea = CreateWindow("LISTBOX", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | LBS_NOINTEGRALHEIGHT | LBS_NOSEL, 0, 40, WINWIDTH(rc) - 100, WINHEIGHT(rc) - 40 - WINHEIGHT(src), hWnd, (HMENU)GUI_LOG, hInst, NULL);

		SendMessage(logarea, WM_SETFONT, (WPARAM)monospace, TRUE);

		SetTimer(hWnd, TIMER_WATCH_TEWI, 100, NULL);
	} else if(msg == WM_TIMER) {
		if(wp == TIMER_WATCH_TEWI) {
			if(idle) {
			} else if(tewi_alive) {
				if(was_starting) {
					was_starting = FALSE;
					SendMessage(status, SB_SETTEXT, 0, (LPARAM) "Started Tewi HTTPd");
				}
				EnableWindow(button_start, FALSE);
				EnableWindow(button_stop, TRUE);
				idle = TRUE;
			} else {
				if(was_starting) {
					was_starting = FALSE;
					SendMessage(status, SB_SETTEXT, 0, (LPARAM) "Stopped Tewi HTTPd");
				}
				EnableWindow(button_start, TRUE);
				EnableWindow(button_stop, FALSE);
				if(exiting) {
					KillTimer(hWnd, TIMER_WATCH_TEWI);
					SendMessage(hWnd, WM_CLOSE, 0, 0);
				}
				idle = TRUE;
			}
		}
	} else if(msg == WM_PAINT) {
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
	} else {
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return 0;
}

BOOL InitApp(void) {
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
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
	wc.hIconSm = LoadIcon(hInst, "TEWI");
	return RegisterClassEx(&wc);
}

BOOL InitWindow(int nCmdShow) {
	HWND hWnd;
	RECT deskrc, rc;
	HWND hDeskWnd = GetDesktopWindow();
	GetWindowRect(hDeskWnd, &deskrc);
	hWnd = CreateWindow("tewihttpd", "Tewi HTTPd", (WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME) ^ WS_MAXIMIZEBOX, 0, 0, 600, 400, NULL, 0, hInst, NULL);

	if(!hWnd) {
		return FALSE;
	}
	GetWindowRect(hWnd, &rc);
	SetWindowPos(hWnd, HWND_TOP, (deskrc.right - (rc.right - rc.left)) / 2, (deskrc.bottom - (rc.bottom - rc.top)) / 2, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

int WINAPI WinMain(HINSTANCE hCurInst, HINSTANCE hPrevInst, LPSTR lpsCmdLine, int nCmdShow) {
	MSG msg;
	BOOL bret;
	hInst = hCurInst;
	tewi_alive = FALSE;
	was_starting = FALSE;
	exiting = FALSE;
	idle = TRUE;
	logfile = stderr;
	if(!InitApp()) {
		return FALSE;
	}
	if(!InitWindow(nCmdShow)) {
		return FALSE;
	}

	while((bret = GetMessage(&msg, NULL, 0, 0)) != 0) {
		if(bret == -1) {
			break;
		} else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}
