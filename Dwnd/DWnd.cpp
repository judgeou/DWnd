#include "DWnd.h"
#include <CommCtrl.h>

std::map<HWND, DWnd*> DWnd::dWndThisMap;

DWnd::DWnd(HMODULE hInstance, int rcid) {
	this->dpiFactor = 1;
	this->mainHWnd = nullptr;
	this->hInstance = hInstance;
	this->rcid = rcid;
}

DWnd::~DWnd()
{
	if (mainHWnd) {
		dWndThisMap.erase(mainHWnd);
	}
}

INT_PTR DWnd::Run(bool selfMessageLoop)
{
	auto hWnd = CreateDialogParam(hInstance, MAKEINTRESOURCE(rcid), NULL, WindProc, (LPARAM)this);

	// ���Ĭ���¼�������
	AddMessageListener(WM_CLOSE, [](HWND hWnd, auto...args) { DestroyWindow(hWnd); });
	AddMessageListener(WM_DESTROY, [](auto...args) { PostQuitMessage(0); });
	AddMessageListener(WM_COMMAND, [this](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		auto cmd = LOWORD(wParam);
		auto cb = cmdHandlerMap[cmd];
		if (cb) { cb(hWnd, msg, wParam, lParam); }
	});

	// �Դ�������Ҫ��ʼ����
	ShowWindow(hWnd, SW_SHOW);

	// ��Ϣѭ��
	if (selfMessageLoop) {
		MSG msg;
		while (GetMessage(&msg, hWnd, 0, 0) > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return msg.wParam;
	}
	else {
		return 0;
	}
}

void DWnd::AddMessageListener(UINT msg, MsgHandler cb)
{
	msgHandlerMap[msg] = cb;
}

void DWnd::AddCommandListener(int command, MsgHandler cb)
{
	cmdHandlerMap[command] = cb;
}

void DWnd::AddTabPage(int tabid, const TabPage& page)
{
	allTabPages[tabid].push_back(page);
}

INT_PTR WINAPI DWnd::WindProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_INITDIALOG) {
		DWnd* dwd = (DWnd*)lParam;
		dWndThisMap[hWnd] = dwd;
		dwd->mainHWnd = hWnd;

		auto hdc = GetDC(hWnd);
		dwd->dpiFactor = GetDeviceCaps(hdc, LOGPIXELSX) / 96.0;
		ReleaseDC(hWnd, hdc);

		// ����tab�ĳ�ʼ��
		for (auto& item : dwd->allTabPages) {
			auto tabid = item.first;
			auto tabWnd = GetDlgItem(hWnd, tabid);
			
			RECT tabRect;
			GetWindowRect(tabWnd, &tabRect);

			auto left = 1 * dwd->dpiFactor;
			auto top = 20 * dwd->dpiFactor;
			auto sx = tabRect.right - tabRect.left - left * 2;
			auto sy = tabRect.bottom - tabRect.top - top - dwd->dpiFactor;
			
			for (auto& page : item.second) {
				TCITEM tie;
				tie.mask = TCIF_TEXT;
				tie.pszText = (WCHAR*)page.title.c_str();
				TabCtrl_InsertItem(tabWnd, page.index, &tie);

				if (page.rcid) {
					auto tabHwnd = CreateDialog(dwd->hInstance, MAKEINTRESOURCE(page.rcid), tabWnd, NULL);
					SetWindowPos(tabHwnd, HWND_TOP, left, top, sx, sy, SWP_HIDEWINDOW);
					page.hWnd = tabHwnd;
				}
			}
			// ����Ҫʵ�ֶ�μ�����Ϣ
			dwd->AddMessageListener(WM_NOTIFY, [tabWnd, tabid, dwd](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
				switch (((LPNMHDR)lParam)->code)
				{
				case TCN_SELCHANGE:
				{
					int iPage = TabCtrl_GetCurSel(tabWnd);
					for (auto& page : dwd->allTabPages[tabid]) {
						if (page.hWnd) {
							ShowWindow(page.hWnd, (iPage == page.index) ? SW_SHOW : SW_HIDE);
						}
					}

					break;
				}
				default:
					break;
				}
			});
		}
	}

	DWnd* dwd = dWndThisMap[hWnd];
	if (dwd) {
		return dwd->InternalWindProc(hWnd, msg, wParam, lParam);
	}
	else {
		return FALSE;
	}

}

INT_PTR DWnd::InternalWindProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	auto cb = msgHandlerMap[msg];
	if (cb) {
		cb(hWnd, msg, wParam, lParam);
		return TRUE;
	}
	else {
		return FALSE;
	}
}
