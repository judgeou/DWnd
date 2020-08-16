#ifndef CONTROLMODEL_20200816
#define CONTROLMODEL_20200816

#include <string>
#include <list>
#include <Windows.h>
#include "DWnd.h"

template<typename T>
struct ComboboxItem {
	std::wstring title;
	T value;
};

template<typename T>
class ControlModel {
public:
	void operator=(const T& newval) {
		if (_value != newval) {
			_value = newval;
			update();
		}
	}

	const T& operator*() const {
		return _value;
	}

protected:
	T _value;
	virtual void update() = 0;
};

class EditModel : public ControlModel<std::wstring> {
public:
	using ControlModel::operator=;
	using ControlModel::operator*;
	EditModel(DWnd& dwd, int rcid);
private:
	DWnd& dwd;
	int rcid;
	void update();
};

class StaticModel : public ControlModel<std::wstring> {
public:
	using ControlModel::operator=;
	using ControlModel::operator*;
	StaticModel(HWND hWnd);
private:
	HWND hWnd;
	void update();
};

template<typename T>
class ComboboxModel {
public:
	ComboboxModel(DWnd& dwd, int rcid): dwd(dwd), rcid(rcid) {
		index = 0;

		dwd.AddCommandEventListener(rcid, CBN_SELCHANGE, [this](HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
			auto combobox = (HWND)lParam;
			index = SendMessage(combobox, CB_GETCURSEL, 0, 0);
		});
	}

	void AddItem(const ComboboxItem<T>& item) {
		items.push_back(item);
		SendMessage(dwd.GetControl(rcid), (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)item.title.c_str());

		if (items.size() == 1) {
			Select(0);
		}
	}

	void Select(int index) {
		this->index = index;
		SendMessage(dwd.GetControl(rcid), CB_SETCURSEL, index, 0);
	}

	const ComboboxItem<T>& operator*() const {
		return items[index];
	}
private:
	DWnd& dwd;
	int rcid;
	int index;
	std::vector<ComboboxItem<T>> items;
};

#endif
