#include <stdio.h>
#include <string>
#include "resource.h"
#include "../Dwnd/DWnd.h"

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
) {
    DWnd dwd(hInstance, IDD_DIALOG1);

    dwd.AddMessageListener(WM_MOUSEMOVE, [](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        auto x = GET_X_LPARAM(lParam);
        auto y = GET_Y_LPARAM(lParam);

        auto xstr = std::to_wstring(x);
        auto ystr = std::to_wstring(y);
        auto str = xstr + L", " + ystr;

        SetDlgItemText(hWnd, IDC_EDIT1, str.c_str());
    });

    dwd.AddCommandListener(IDC_BUTTON1, [](HWND hWnd, auto...) {
        MessageBox(hWnd, L"拜拜了您", L"bye bye", MB_OK | MB_ICONINFORMATION);
        SendMessage(hWnd, WM_CLOSE, 0, 0);
    });

    dwd.AddTabPage(IDC_TAB1, { L"登录", 0, IDD_PAGE_1 });
    dwd.AddTabPage(IDC_TAB1, { L"注册", 1, IDD_PAGE_2 });
    dwd.AddTabPage(IDC_TAB1, { L"修改密码", 2 });

    return dwd.Run();
}
