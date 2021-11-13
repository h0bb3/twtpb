// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include <DInput.h>
#include "InputDevice.h"

namespace dxrnd {
class InputDeviceFactory {
	public:
		InputDeviceFactory(void);
		~InputDeviceFactory(void);

		InputDevice *CreateDevice(IDirectInput8 &a_dInput, HWND a_hWnd);
	private:

		static BOOL CALLBACK EnumGameControllerCallback(LPCDIDEVICEINSTANCE a_pDeviceObject, VOID* a_pCallbackData);
		static bool IsXInputDevice(const GUID* a_pGuidProductFromDirectInput);

		struct ControllerCallbackData {
			HWND m_hWnd;
			IDirectInput8 *m_pDInput;
			InputDevice *m_pDevice;
		};

};

}