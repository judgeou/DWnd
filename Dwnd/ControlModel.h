#include <string>
#include <Windows.h>

template<typename T>
class ControlModel {
public:
	void operator=(const T& newval) {
		_value = newval;
		update();
	}

	const T& operator*() {
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
	EditModel(HWND hWnd);
protected:
	HWND hWnd;
	void update();
};