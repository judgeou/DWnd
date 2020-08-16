#include "DWnd.h"
#include <CommCtrl.h>

std::map<HWND, DWnd*> DWnd::dWndThisMap;

DWnd::DWnd(HMODULE hInstance, int rcid, HWND fatherHwnd) {
	this->dpiFactor = 1;
	this->fatherHwnd = fatherHwnd;
	this->hInstance = hInstance;
	this->rcid = rcid;

	mainHWnd = CreateDialogParam(hInstance, MAKEINTRESOURCE(rcid), fatherHwnd, WindProc, (LPARAM)this);
	
	// 如果有父窗口，则直接进入Run(false)
	if (fatherHwnd) {
		Run(false);
	}
}

DWnd::~DWnd()
{
	if (mainHWnd) {
		dWndThisMap.erase(mainHWnd);
	}
}

INT_PTR DWnd::Run(bool selfMessageLoop)
{
	auto hWnd = mainHWnd;

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
		while (GetMessage(&msg, NULL, 0, 0) > 0) {
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

std::list<DWnd::MsgHandler>::const_iterator DWnd::AddCommandEventListener(int rcid, WORD msg, MsgHandler cb)
{
	return AddCommandListener(rcid, [msg, cb](HWND hWnd, UINT msg_, WPARAM wParam, LPARAM lParam) {
		auto code = HIWORD(wParam);
		if (code == msg) {
			cb(hWnd, msg_, wParam, lParam);
		}
    });
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

void DWnd::RemoveCommandEventListener(int command, std::list<DWnd::MsgHandler>::const_iterator index)
{
	RemoveCommandListener(command, index);
}

void DWnd::AddTabPage(int tabid, const TabPage& page)
{
	auto& tabPages = allTabPages[tabid];
	tabPages.push_back(page);

	// 处理tab的初始化
	auto tabWnd = GetDlgItem(mainHWnd, tabid);

	RECT tabRect;
	GetWindowRect(tabWnd, &tabRect);

	auto left = 1 * dpiFactor;
	auto top = 20 * dpiFactor;
	auto sx = tabRect.right - tabRect.left - left * 2;
	auto sy = tabRect.bottom - tabRect.top - top - dpiFactor;

	TCITEM tie;
	tie.mask = TCIF_TEXT;
	tie.pszText = (WCHAR*)page.title.c_str();
	TabCtrl_InsertItem(tabWnd, page.index, &tie);

	if (page.hWnd) {
		// 重新设置父窗口
		SetParent(page.hWnd, tabWnd);
		SetWindowPos(page.hWnd, HWND_TOP, left, top, sx, sy, SWP_HIDEWINDOW);
	}

	// 当一个tab第一次添加page的时候添加监听事件
	if (tabPages.size() == 1) {
		// 必须要实现多次监听消息
		AddMessageListener(WM_NOTIFY, [tabWnd, tabid, &tabPages](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			switch (((LPNMHDR)lParam)->code)
			{
			case TCN_SELCHANGE:
			{
				int iPage = TabCtrl_GetCurSel(tabWnd);
				for (auto& page : tabPages) {
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

		// 默认设置第一个tab为选中
		SelectTabPage(tabid, 0);
	}
}

void DWnd::SelectTabPage(int tabid, int index)
{
	auto tabWnd = GetDlgItem(mainHWnd, tabid);
	auto& tabPages = allTabPages[tabid];

	TabCtrl_SetCurSel(tabWnd, index);
	
	for (auto& page : tabPages) {
		if (page.hWnd) {
			ShowWindow(page.hWnd, (index == page.index) ? SW_SHOW : SW_HIDE);
		}
	}
}

void DWnd::Hide()
{
	ShowWindow(mainHWnd, SW_HIDE);
}

HWND DWnd::GetControl(int rcid)
{
	return GetDlgItem(mainHWnd, rcid);
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
