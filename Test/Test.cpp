﻿#include <stdio.h>
#include <string>
#include "resource.h"
#include "../Dwnd/DWnd.h"
#include "../Dwnd/ControlModel.h"

using std::wstring;

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
) {
    DWnd dwd(hInstance, IDD_DIALOG1);
    DWnd d2(hInstance, IDD_PAGE_1, dwd.mainHWnd);
    DWnd d3(hInstance, IDD_PAGE_2, dwd.mainHWnd);

    EditModel edit1(dwd, IDC_EDIT1);
    EditModel edit1_2(d2, IDC_EDIT1);
    StaticModel static1(d3.GetControl(IDC_STATIC1));
    ComboboxModel<std::wstring> combo1(d3, IDC_COMBO1);
    CheckBoxModel cbox1(d2, IDC_CHECK1);

    RadioButtonModel<wstring> radioGroup1(d2, {
        { IDC_RADIO1, L"物理VALUE" },
        { IDC_RADIO2, L"魔法VALUE" }
    });
    RadioButtonModel<wstring> radioGroup2(d2, {
        { IDC_RADIO3, L"范围VALUE" },
        { IDC_RADIO4, L"单体VALUE" }
    });
    
    TabModel tab1(dwd, IDC_TAB1);
    TabModel tab2(dwd, IDC_TAB2);

    tab1.AddPage({ L"登录", 0, d2.mainHWnd });
    tab1.AddPage({ L"注册", 1, NULL });
    tab1.AddPage({ L"修改密码", 2, NULL });

    tab2.AddPage({ L"角色", 0, d3.mainHWnd });
    tab2.AddPage({ L"ABC", 1, NULL });

    combo1.AddItem({ L"1 ミヤコ", L"【物理】最前衛で、ひたすら敵の攻撃を避ける幽霊少女。" });
    combo1.AddItem({ L"2 クウカ", L"【物理】前衛で囮となり、攻撃を引き付ける暴走ドＭ娘。" });
    combo1.AddItem({ L"3 ニノン", L"【物理】中衛で、強力な範囲攻撃を操る、忍術の使い手。" });
    combo1.AddItem({ L"4 キョウカ", L"【魔法】後衛から強力な魔法で攻撃するちびっこ優等生。" });

    static1 = (*combo1).value;

    radioGroup1 = IDC_RADIO1;
    radioGroup2 = IDC_RADIO3;

    radioGroup1.OnChange([&d2](const wstring& str) {
        SetWindowText(d2.GetControl(IDC_B1), str.c_str());
    });

    radioGroup2.OnChange([&d2](const wstring& str) {
        SetWindowText(d2.GetControl(IDC_B1), str.c_str());
    });

    cbox1.OnChange([&cbox1](const char& value) {
        std::wstring title = value == 0 ? L"No" : L"Yes";
        cbox1.SetTitle(title);
    });
    cbox1 = 1;

    dwd.AddMessageListener(WM_MOUSEMOVE, [&edit1](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        auto x = GET_X_LPARAM(lParam);
        auto y = GET_Y_LPARAM(lParam);

        auto xstr = std::to_wstring(x);
        auto ystr = std::to_wstring(y);
        auto str = xstr + L", " + ystr;

        edit1 = str;
    });

    dwd.AddCommandListener(IDC_BUTTON1, [](HWND hWnd, auto...) {
        MessageBox(hWnd, L"拜拜了您", L"bye bye", MB_OK | MB_ICONINFORMATION);
        SendMessage(hWnd, WM_CLOSE, 0, 0);
        });

    auto cb1 = dwd.AddMessageListener(WM_LBUTTONUP, [](HWND hWnd, auto...) {
        MessageBox(hWnd, L"只要你点了下面的 X 按钮，就再也见不到我了", L"bye bye", MB_OK | MB_ICONINFORMATION);
        });

    dwd.AddCommandListener(IDC_BUTTON2, [&dwd, &cb1](auto...) {
        static bool ok = true;
        if (ok) {
            dwd.RemoveMessageListener(WM_LBUTTONUP, cb1);
            ok = false;
        }
    });

    d2.AddCommandListener(IDC_B1, [&edit1_2, &static1, &radioGroup1, &radioGroup2](HWND hWnd, auto...) {
        static1 = *edit1_2;
        radioGroup1 = IDC_RADIO2;
        radioGroup2 = IDC_RADIO4;
    });

    d2.AddMessageListener(WM_MOUSEMOVE, [&edit1_2, &static1](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        auto x = GET_X_LPARAM(lParam);
        auto y = GET_Y_LPARAM(lParam);

        auto xstr = std::to_wstring(x);
        auto ystr = std::to_wstring(y);
        auto str = xstr + L", " + ystr;

        edit1_2 = str;
        });
    
    d3.AddCommandEventListener(IDC_COMBO1, CBN_SELCHANGE, [&combo1, &static1](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        // 此时下拉控件内部已经处理完变更，我们直接拿数据
        static1 = (*combo1).value;
    });

    d3.AddCommandListener(IDC_BUTTON1, [&combo1](auto...) {
        combo1.RemoveItem(combo1.index);
    });

    d3.AddCommandListener(IDC_BUTTON2, [&combo1](auto...) {
        combo1.ClearItem();
    });

    return dwd.Run();
}
