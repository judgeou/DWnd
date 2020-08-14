#include "DWnd.h"
#include <CommCtrl.h>

std::map<HWND, DWnd*> DWnd::dWndThisMap;

DWnd::DWnd(HMODULE hInstance, int rcid) {
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

	// 添加默认事件处理器
	AddMessageListener(WM_CLOSE, [](HWND hWnd, auto...args) { DestroyWindow(hWnd); });
	AddMessageListener(WM_DESTROY, [](auto...args) { PostQuitMessage(0); });
	AddMessageListener(WM_COMMAND, [this](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		auto cmd = LOWORD(wParam);
		auto cb = cmdHandlerMap[cmd];
		if (cb) { cb(hWnd, msg, wParam, lParam); }
	});

	// 对窗体做必要初始设置
	ShowWindow(hWnd, SW_SHOW);

	// 消息循环
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

		// 处理tab的初始化
		for (const auto& item : dwd->allTabPages) {
			auto tabid = item.first;
			auto tabWnd = GetDlgItem(hWnd, tabid);
			RECT tabRect;
			GetWindowRect(tabWnd, &tabRect);
			
			for (const auto& page : item.second) {
				TCITEM tie;
				tie.mask = TCIF_TEXT;
				tie.pszText = (WCHAR*)page.title.c_str();
				TabCtrl_InsertItem(tabWnd, page.index, &tie);

				if (page.rcid) {

				}
			}
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
