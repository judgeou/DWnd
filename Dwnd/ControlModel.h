#include <string>
#include <list>
#include <Windows.h>
#include "DWnd.h"

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
protected:
	DWnd& dwd;
	int rcid;
	void update();
};

class StaticModel : public ControlModel<std::wstring> {
public:
	using ControlModel::operator=;
	using ControlModel::operator*;
	StaticModel(HWND hWnd);
protected:
	HWND hWnd;
	void update();
};