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
		int rcid;
		HWND hWnd;
	};

	DWnd(HMODULE hInstance, int rcid);
	~DWnd();

	INT_PTR Run(bool selfMessageLoop = true);
	std::list<DWnd::MsgHandler>::const_iterator AddMessageListener(UINT msg, MsgHandler cb);
	void AddCommandListener(int command, MsgHandler cb);
	void RemoveMessageListener(UINT msg, std::list<DWnd::MsgHandler>::const_iterator index);

	void AddTabPage(int tabid, const TabPage& page);
private:
	static INT_PTR WINAPI WindProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	INT_PTR InternalWindProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static std::map<HWND, DWnd*> dWndThisMap;
	HMODULE hInstance;
	int rcid;
	HWND mainHWnd;
	double dpiFactor;
	std::map<UINT, std::list<MsgHandler>> msgHandlerMap;
	std::map<WORD, MsgHandler> cmdHandlerMap;
	std::map<int, std::vector<TabPage>> allTabPages;
};
