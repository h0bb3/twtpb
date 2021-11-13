// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include <DInput.h>
#include <XInput.h>

namespace dxrnd {

// wraps DInput device info and input format for buttons and axes
class InputDevice {
	public:
		static InputDevice* Create(LPDIRECTINPUTDEVICE8 a_pGameCtrl);
		virtual ~InputDevice(void);

		virtual HRESULT GetState() = 0;
		virtual LONG GetAxis(int a_axisIx) = 0;
		virtual LONG GetButton(int a_buttonIx) = 0;

		bool IsAlive();

		int m_axisCount;
		int m_buttonCount;

		typedef std::vector<cmn::String> StringVector;
		StringVector m_axisNames;
		StringVector m_buttonNames;

		int m_xAxisIx;
		int m_yAxisIx;

		DIDEVICEINSTANCE m_deviceInfo;

	protected:
		InputDevice(const InputDevice& a_device);	// not implemented to produce linker error
		InputDevice(LPDIRECTINPUTDEVICE8 a_pGameCtrl);

		LPDIRECTINPUTDEVICE8 m_pGameCtrl;

};

class DInputDevice : public InputDevice {
	public:
		static DInputDevice * Create(LPDIRECTINPUTDEVICE8 a_pGameCtrl, HWND a_hWnd);
		~DInputDevice();

		HRESULT GetState();
		LONG GetAxis(int a_axisIx);
		LONG GetButton(int a_buttonIx);

	private:
		DInputDevice(LPDIRECTINPUTDEVICE8 a_pGameCtrl, HWND a_hwnd);
		DInputDevice(const DInputDevice& a_copy);	// not implemented to produce link error

		static BOOL CALLBACK EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, VOID* a_pCallbackData);
		static BOOL CALLBACK EnumButtonsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, VOID* a_pCallbackData);

		int m_dataSize;
		BYTE *m_internalData;

		struct CallbackData {
			int m_formatIx;
			int m_axisIx;
			int m_xAxisIx;
			int m_yAxisIx;
			StringVector *m_pAxisNames;
			StringVector *m_pButtonNames;
			LPDIRECTINPUTDEVICE8 m_pDevice;
			DIOBJECTDATAFORMAT *m_pFormat;
		};
};

class XInputDevice : public InputDevice {
	public:
		static XInputDevice* Create(LPDIRECTINPUTDEVICE8 a_pGameCtrl);

		HRESULT GetState();
		LONG GetAxis(int a_axisIx);
		LONG GetButton(int a_buttonIx);

	private:
		XInputDevice(LPDIRECTINPUTDEVICE8 a_pGameCtrl);
		XInputDevice(const XInputDevice &a_copy);	// not implemented to produce link error

		XINPUT_STATE m_state;
};

}
