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

	// 添加默认事件处理器
	AddMessageListener(WM_CLOSE, [](HWND hWnd, auto...args) { DestroyWindow(hWnd); });
	AddMessageListener(WM_DESTROY, [](auto...args) { PostQuitMessage(0); });
	AddMessageListener(WM_COMMAND, [this](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		auto cmd = LOWORD(wParam);
		auto cb = cmdHandlerMap[cmd];
		for (auto& call : cb) {
			call(hWnd, msg, wParam, lParam);
		}
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

std::list<DWnd::MsgHandler>::const_iterator DWnd::AddMessageListener(UINT msg, MsgHandler cb)
{
	auto& handlers = msgHandlerMap[msg];
	handlers.push_back(cb);
	auto index = handlers.cend();
	index--;
	return index;
}

std::list<DWnd::MsgHandler>::const_iterator DWnd::AddCommandListener(int command, MsgHandler cb)
{
	auto& handlers = cmdHandlerMap[command];
	handlers.push_back(cb);
	auto index = handlers.cend();
	index--;
	return index;
}

// 千万不要使用已经删除过的index，一定会出现异常
void DWnd::RemoveMessageListener(UINT msg, std::list<DWnd::MsgHandler>::const_iterator index)
{
	auto& handlers = msgHandlerMap[msg];
	handlers.erase(index);
}

void DWnd::RemoveCommandListener(int command, std::list<DWnd::MsgHandler>::const_iterator index)
{
	auto& handlers = cmdHandlerMap[command];
	handlers.erase(index);
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

		// 处理tab的初始化
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
			// 必须要实现多次监听消息
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
	auto& cbset = msgHandlerMap[msg];
	for (auto& cb : cbset) {
		cb(hWnd, msg, wParam, lParam);
	}
	return cbset.empty() ? FALSE : TRUE;
}
