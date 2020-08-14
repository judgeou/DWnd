#include <Windows.h>
#include <windowsx.h>
#include "resource.h"
#include <CommCtrl.h>
// #pragma comment(lib, "Comctl32.lib")
#include <string>

using std::wstring;

HMODULE g_module;
HWND tabs[2];

BOOL CALLBACK TabProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_BUTTON_T1:
			MessageBox(hwnd, L"一", L"", MB_OK);
			break;
		case IDC_BUTTON_T2:
			MessageBox(hwnd, L"二", L"", MB_OK);
			break;
		}
		break;
	case WM_MOUSEMOVE:
	{
		auto x = GET_X_LPARAM(lParam);
		auto y = GET_Y_LPARAM(lParam);

		auto xstr = std::to_wstring(x);
		auto ystr = std::to_wstring(y);
		auto str = xstr + L", " + ystr;

		SetDlgItemText(hwnd, IDC_EDIT_T, str.c_str());
		break;
	}
	default:
		return FALSE;
	}
	return TRUE;
}

void ActiveTabPage(int i) {
	ShowWindow(tabs[0], (i == 0) ? SW_SHOW : SW_HIDE);
	ShowWindow(tabs[1], (i == 1) ? SW_SHOW : SW_HIDE);
}

INT_PTR CALLBACK WindProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg)
	{
	case WM_NOTIFY: {
		switch (((LPNMHDR)lParam)->code) {
		case TCN_SELCHANGE:
			auto tabWnd = GetDlgItem(hWnd, IDC_TAB1);
			int iPage = TabCtrl_GetCurSel(tabWnd);
			ActiveTabPage(iPage);
			break;
		}
		break;
	}
	case WM_CHAR:
	{
		auto tabWnd = GetDlgItem(hWnd, IDC_TAB1);

		RECT rect;
		GetWindowRect(tabWnd, &rect);
		POINT offset = { 0 };
		ScreenToClient(hWnd, &offset);
		OffsetRect(&rect, offset.x, offset.y);
		OutputDebugString(L"");
		break;
	}
	case WM_INITDIALOG:
	{
		if (lParam == 0) { // main dialog init
			auto tabWnd = GetDlgItem(hWnd, IDC_TAB1);
			
			TCITEM tie;
			tie.mask = TCIF_TEXT;
			tie.pszText = (WCHAR*)L"大威天龙";
			TabCtrl_InsertItem(tabWnd, 0, &tie);
			tie.pszText = (WCHAR*)L"你不是人";
			TabCtrl_InsertItem(tabWnd, 1, &tie);

			RECT rect;
			GetWindowRect(tabWnd, &rect);

			auto tab1 = tabs[0] = CreateDialog(g_module, MAKEINTRESOURCE(IDD_DIALOG_T1), tabWnd, TabProc);
			auto tab2 = tabs[1] = CreateDialog(g_module, MAKEINTRESOURCE(IDD_DIALOG_T2), tabWnd, TabProc);

			auto left = 2;
			auto top = 30;
			auto sx = rect.right - rect.left - 6;
			auto sy = rect.bottom - rect.top - 6;

			SetWindowPos(tab1, HWND_TOP, left, top, sx, sy, SWP_HIDEWINDOW);
			SetWindowPos(tab2, HWND_TOP, left, top, sx, sy, SWP_HIDEWINDOW);

			ActiveTabPage(0);
		}

		break;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_EXIT:
			MessageBox(hWnd, L"拜拜了您", L"bye bye", MB_OK | MB_ICONINFORMATION);
			SendMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		default:
			break;
		}
		break;
	case WM_MOUSEMOVE:
	{
		auto x = GET_X_LPARAM(lParam);
		auto y = GET_Y_LPARAM(lParam);

		auto xstr = std::to_wstring(x);
		auto ystr = std::to_wstring(y);
		auto str = xstr + L", " + ystr;

		SetDlgItemText(hWnd, IDC_EDIT1, str.c_str());
		break;
	}
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

INT_PTR ApplicationStart(HMODULE hModule) {
	//auto hWnd = CreateDialog(hModule, MAKEINTRESOURCE(IDD_DIALOG1), NULL, WindProc);
	//ShowWindow(hWnd, SW_SHOW);

	//MSG msg;
	//while (GetMessage(&msg, hWnd, 0, 0) > 0) {
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	//}
	//return msg.wParam;
	g_module = hModule;
	auto r = DialogBox(hModule, MAKEINTRESOURCE(IDD_DIALOG1), NULL, WindProc);
	return r;
}
