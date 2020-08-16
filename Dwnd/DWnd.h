#ifndef DWND_20200816
#define DWND_20200816

#include <map>
#include <list>
#include <vector>
#include <functional>
#include <string>
#include <Windows.h>
#include <windowsx.h>

class DWnd {
public:
	typedef std::function<void(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)> MsgHandler;
	struct TabPage
	{
		std::wstring title;
		int index;
		HWND hWnd;
	};

	HWND mainHWnd;

	DWnd(HMODULE hInstance, int rcid, HWND fatherHwnd = NULL);
	~DWnd();

	INT_PTR Run(bool selfMessageLoop = true);
	std::list<DWnd::MsgHandler>::const_iterator AddMessageListener(UINT msg, MsgHandler cb);
	std::list<DWnd::MsgHandler>::const_iterator AddCommandListener(int command, MsgHandler cb);
	std::list<DWnd::MsgHandler>::const_iterator AddCommandEventListener(int rcid, WORD msg, MsgHandler cb);
	
	void RemoveMessageListener(UINT msg, std::list<DWnd::MsgHandler>::const_iterator index);
	void RemoveCommandListener(int command, std::list<DWnd::MsgHandler>::const_iterator index);
	void RemoveCommandEventListener(int command, std::list<DWnd::MsgHandler>::const_iterator index);

	void AddTabPage(int tabid, const TabPage& page);
	void SelectTabPage(int tabid, int index);
	void Hide();
	HWND GetControl(int rcid);
private:
	static INT_PTR WINAPI WindProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	INT_PTR InternalWindProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static std::map<HWND, DWnd*> dWndThisMap;
	HMODULE hInstance;
	int rcid;
	HWND fatherHwnd;
	double dpiFactor;
	std::map<UINT, std::list<MsgHandler>> msgHandlerMap;
	std::map<WORD, std::list<MsgHandler>> cmdHandlerMap;
	std::map<uint64_t, std::list<MsgHandler>> cmdEventHandlerMap;
	std::map<int, std::vector<TabPage>> allTabPages;
};

#endif
