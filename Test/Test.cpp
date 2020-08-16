#include <stdio.h>
#include <string>
#include "resource.h"

#include "../Dwnd/ControlModel.h"

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

    auto cb1 = dwd.AddMessageListener(WM_LBUTTONUP, [](HWND hWnd, auto...) {
        MessageBox(hWnd, L"只要你点了下面的 X 按钮，就再也见不到我了", L"bye bye", MB_OK | MB_ICONINFORMATION);
    });

    dwd.AddCommandListener(IDC_BUTTON2, [&dwd, &cb1](auto...) {
        dwd.RemoveMessageListener(WM_LBUTTONUP, cb1);
    });

    EditModel edit1(dwd, IDC_EDIT1);

    DWnd d2(hInstance, IDD_PAGE_1, dwd.mainHWnd);
    DWnd d3(hInstance, IDD_PAGE_2, dwd.mainHWnd);

    EditModel edit1_2(d2, IDC_EDIT1);
    StaticModel static1(d3.GetControl(IDC_STATIC1));

    d2.AddCommandListener(IDC_B1, [&edit1_2, &static1](HWND hWnd, auto...) {
        static1 = *edit1_2;
    });
    d2.AddMessageListener(WM_MOUSEMOVE, [&edit1_2, &static1](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        auto x = GET_X_LPARAM(lParam);
        auto y = GET_Y_LPARAM(lParam);

        auto xstr = std::to_wstring(x);
        auto ystr = std::to_wstring(y);
        auto str = xstr + L", " + ystr;

        edit1_2 = str;
    });
    d3.AddMessageListener(WM_LBUTTONUP, [](HWND hWnd, auto...) {
        MessageBox(hWnd, L"你终于来了2", L"bye bye", MB_OK | MB_ICONINFORMATION);
        });

    dwd.AddTabPage(IDC_TAB1, { L"登录", 0, d2.mainHWnd });
    dwd.AddTabPage(IDC_TAB1, { L"注册", 1, NULL });
    dwd.AddTabPage(IDC_TAB1, { L"修改密码", 2, NULL });

    dwd.AddTabPage(IDC_TAB2, { L"ABC", 0, d3.mainHWnd });
    dwd.AddTabPage(IDC_TAB2, { L"DEF", 1, });



    return dwd.Run();
}
