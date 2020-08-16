#include "ControlModel.h"

EditModel::EditModel(DWnd& dwd, int rcid): dwd(dwd)
{
	this->dwd = dwd;
	this->rcid = rcid;

	dwd.AddCommandEventListener(rcid, EN_CHANGE, [this](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
		auto edit = (HWND)lParam;
		int len = GetWindowTextLength(edit);
		std::wstring text(len, '\0');
		GetWindowText(edit, &text[0], len);
		_value = text;
    });
}

void EditModel::update() {
	SetWindowText(dwd.GetControl(rcid), _value.c_str());
}

StaticModel::StaticModel(HWND hWnd)
{
	this->hWnd = hWnd;
}

void StaticModel::update() {
	SetWindowText(hWnd, _value.c_str());
}