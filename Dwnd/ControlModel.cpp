#include "ControlModel.h"

EditModel::EditModel(HWND hWnd) : ControlModel()
{
	this->hWnd = hWnd;
}

void EditModel::update() {
	SetWindowText(hWnd, _value.c_str());
}