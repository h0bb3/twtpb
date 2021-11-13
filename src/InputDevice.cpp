// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "InputDevice.h"
#include "XInput.h"

namespace dxrnd {

InputDevice::InputDevice(LPDIRECTINPUTDEVICE8 a_pGameCtrl) : m_pGameCtrl(a_pGameCtrl) {
	HRESULT hr;

	DIDEVCAPS caps;
	caps.dwSize = sizeof(DIDEVCAPS);
	hr = m_pGameCtrl->GetCapabilities(&caps);
	if (FAILED(hr)) {
		LOG_3("Failed to get capabilities for Game Controller");
		m_pGameCtrl->Release();
		m_pGameCtrl = NULL;
	}

	m_axisCount = caps.dwAxes;
	m_buttonCount = caps.dwButtons;

	if (m_axisCount < 2 || m_buttonCount < 6) {
		LOG_3("Game Controller has to few axes or buttons, minimum are 2 axes and 6 buttons");
		m_pGameCtrl->Release();
		m_pGameCtrl = NULL;
	}

	m_deviceInfo.dwSize = sizeof(m_deviceInfo);
	hr = m_pGameCtrl->GetDeviceInfo(&m_deviceInfo);
	if (FAILED(hr)) {
		LOG_3("Failed to get device info for Game Controller");
		m_pGameCtrl->Release();
		m_pGameCtrl = NULL;
	}
}

InputDevice::~InputDevice() {
	if (m_pGameCtrl != NULL) {
		m_pGameCtrl->Release();
		m_pGameCtrl = NULL;
	}
}


BOOL CALLBACK DInputDevice::EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, VOID* a_pCallbackData) {
	DInputDevice::CallbackData *callbackData((DInputDevice::CallbackData*)a_pCallbackData);

	callbackData->m_pFormat[callbackData->m_formatIx].dwType = lpddoi->dwType;
	callbackData->m_pFormat[callbackData->m_formatIx].dwFlags = lpddoi->dwFlags;

	if (lpddoi->guidType == GUID_XAxis) {
		callbackData->m_xAxisIx = callbackData->m_formatIx;
	}
	if (lpddoi->guidType == GUID_YAxis) {
		callbackData->m_yAxisIx = callbackData->m_formatIx;
	}

	callbackData->m_pAxisNames->push_back(lpddoi->tszName);

	DIPROPRANGE diprg; 
	diprg.diph.dwSize       = sizeof(DIPROPRANGE); 
	diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
	diprg.diph.dwHow        = DIPH_BYID; 
	diprg.diph.dwObj        = lpddoi->dwType; 
	diprg.lMin              = -1000; 
	diprg.lMax              = +1000;

	HRESULT hr;
	hr = callbackData->m_pDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
	if (FAILED(hr)) {
		LOG_3_A1("Failed to set range for axis: %s", lpddoi->tszName);
	}

	callbackData->m_formatIx++;
	return DIENUM_CONTINUE;
}

BOOL CALLBACK DInputDevice::EnumButtonsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, VOID* a_pCallbackData) {
	DInputDevice::CallbackData *callbackData((DInputDevice::CallbackData*)a_pCallbackData);

	callbackData->m_pFormat[callbackData->m_formatIx].dwType = lpddoi->dwType;
	callbackData->m_pFormat[callbackData->m_formatIx].dwFlags = lpddoi->dwFlags;

	callbackData->m_pButtonNames->push_back(lpddoi->tszName);

	callbackData->m_formatIx++;
	return DIENUM_CONTINUE;
}

DInputDevice* DInputDevice::Create(LPDIRECTINPUTDEVICE8 a_pGameCtrl, HWND a_hwnd) {
	DInputDevice *ret = new DInputDevice(a_pGameCtrl, a_hwnd);
	if (ret->m_pGameCtrl == NULL) {
		delete ret;
		return NULL;
	}

	return ret;
}

DInputDevice::DInputDevice(LPDIRECTINPUTDEVICE8 a_pGameCtrl, HWND a_hwnd) : InputDevice(a_pGameCtrl), m_internalData(NULL) {
	HRESULT hr;

	if (m_pGameCtrl == NULL) {
		return;
	}

	m_dataSize = 0;
	if (m_axisCount > 0) {
		m_dataSize += sizeof(LONG) * m_axisCount;
	}

	if (m_buttonCount > 0) {
		m_dataSize += sizeof(BYTE) * m_buttonCount;
	}

	if (m_dataSize % sizeof(DWORD) > 0) {
		m_dataSize += sizeof(BYTE) * (sizeof(DWORD) - m_dataSize % sizeof(DWORD));
	}

	m_internalData = new BYTE[m_dataSize];

	DIOBJECTDATAFORMAT *dataFormat = new DIOBJECTDATAFORMAT[m_axisCount + m_buttonCount];
	for (int axIx = 0; axIx < m_axisCount; axIx++) {
		dataFormat[axIx].pguid = 0;
		dataFormat[axIx].dwOfs = axIx * sizeof(LONG);
		dataFormat[axIx].dwType =  DIDFT_MAKEINSTANCE(axIx);
		dataFormat[axIx].dwFlags = 0;
	}

	for (int bIx = 0; bIx < m_buttonCount; bIx++) {
		dataFormat[m_axisCount + bIx].pguid = 0;
		dataFormat[m_axisCount + bIx].dwOfs = m_axisCount * sizeof(LONG) + bIx * sizeof(BYTE);
		dataFormat[m_axisCount + bIx].dwType = DIDFT_MAKEINSTANCE(bIx);
		dataFormat[m_axisCount + bIx].dwFlags = 0;
	}

	DIDATAFORMAT format;
	format.dwSize = sizeof(format);
	format.dwObjSize = sizeof(DIOBJECTDATAFORMAT);
	format.dwNumObjs = m_axisCount + m_buttonCount;
	format.dwFlags = DIDF_ABSAXIS;
	format.dwDataSize = m_dataSize;
	format.rgodf = dataFormat;

	DIDATAFORMAT format2 = c_dfDIJoystick2;

	CallbackData callbackData;

	callbackData.m_formatIx = 0;
	callbackData.m_pAxisNames = &m_axisNames;
	callbackData.m_pButtonNames = &m_buttonNames;
	callbackData.m_pDevice = m_pGameCtrl;
	callbackData.m_pFormat = dataFormat;

	m_pGameCtrl->EnumObjects(EnumAxesCallback, &callbackData, DIDFT_AXIS);
	m_pGameCtrl->EnumObjects(EnumButtonsCallback, &callbackData, DIDFT_BUTTON);

	m_xAxisIx = callbackData.m_xAxisIx;
	m_yAxisIx = callbackData.m_yAxisIx;

	hr = m_pGameCtrl->SetDataFormat(&format);
	if (FAILED(hr)) {
		LOG_3("Failed to set data format for Game Controller");
		m_pGameCtrl->Release();
		m_pGameCtrl = NULL;
		delete [] dataFormat;
		return;
	}
	delete [] dataFormat;

	hr = m_pGameCtrl->SetCooperativeLevel(a_hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(hr)) {
		LOG_3("Failed to set cooperative level for Game Controller");
		m_pGameCtrl->Release();
		m_pGameCtrl = NULL;
	}
}

DInputDevice::~DInputDevice(void) {
	delete [] m_internalData;
}

HRESULT DInputDevice::GetState() {
	HRESULT hr;

	ZeroMemory(m_internalData, m_dataSize);



	hr = m_pGameCtrl->Poll();
	if (FAILED(hr)) {
		hr = m_pGameCtrl->Acquire();
	} else {
		hr = m_pGameCtrl->GetDeviceState(m_dataSize, m_internalData);
	}

	return hr;
}

bool InputDevice::IsAlive() {
	HRESULT hr;
	hr = m_pGameCtrl->Poll();

	return hr == DI_OK;
}


LONG DInputDevice::GetAxis(int a_axisIx) {
	if (a_axisIx >= m_axisCount) {
		return 0;
	}
	return ((LONG*)m_internalData)[a_axisIx];
}

LONG DInputDevice::GetButton(int a_buttonIx) {
	if (a_buttonIx >= m_buttonCount) {
		return 0;
	}
	return (LONG)(m_internalData[m_axisCount * sizeof(LONG) + a_buttonIx]);
}


////////////////////////////
// XInputDevice

XInputDevice* XInputDevice::Create(LPDIRECTINPUTDEVICE8 a_pGameCtrl) {
	XInputDevice *pRet = new XInputDevice(a_pGameCtrl);
	if (pRet->m_pGameCtrl == NULL) {
		delete pRet;
		return NULL;
	}

	return pRet;
}

XInputDevice::XInputDevice(LPDIRECTINPUTDEVICE8 a_pGameCtrl) : InputDevice(a_pGameCtrl) {
	// potentially dangerous?
	m_buttonCount = 8;
	m_axisCount = 4;
	m_xAxisIx = 1;
	m_yAxisIx = 0;

	m_axisNames.push_back("Right Y-Axis");
	m_axisNames.push_back("Right X-Axis");
	m_axisNames.push_back("Left Y-Axis");
	m_axisNames.push_back("Left X-Axis");

	m_buttonNames.push_back("X Button");
	m_buttonNames.push_back("Y Button");
	m_buttonNames.push_back("B Button");
	m_buttonNames.push_back("A Button");
	m_buttonNames.push_back("Left Shoulder");
	m_buttonNames.push_back("Right Shoulder");
	m_buttonNames.push_back("Left Trigger");
	m_buttonNames.push_back("Right Trigger");

}

HRESULT XInputDevice::GetState() {
	ZeroMemory(&m_state, sizeof(m_state));
	return XInputGetState(0, &m_state);
}

LONG XInputDevice::GetAxis(int a_axisIx) {
	if (a_axisIx == 0) {
		return (LONG)((m_state.Gamepad.sThumbRY / 32768.0f) * -1000);
	} else if (a_axisIx == 1) {
		return (LONG)((m_state.Gamepad.sThumbRX / 32768.0f) * 1000);
	} else if (a_axisIx == 2) {
		return (LONG)((m_state.Gamepad.sThumbLY / 32768.0f) * -1000);
	} else if (a_axisIx == 3) {
		return (LONG)((m_state.Gamepad.sThumbLX / 32768.0f) * 1000);
	}

	return 0;
}

LONG XInputDevice::GetButton(int a_buttonIx) {
	if (a_buttonIx == 0) {
		return m_state.Gamepad.wButtons & XINPUT_GAMEPAD_X;
	}
	if (a_buttonIx == 1) {
		return m_state.Gamepad.wButtons & XINPUT_GAMEPAD_Y;
	}
	if (a_buttonIx == 2) {
		return m_state.Gamepad.wButtons & XINPUT_GAMEPAD_B;
	}
	if (a_buttonIx == 3) {
		return m_state.Gamepad.wButtons & XINPUT_GAMEPAD_A;
	}

	if (a_buttonIx == 4) {
		return m_state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
	}
	if (a_buttonIx == 5) {
		return m_state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;
	}

	if (a_buttonIx == 6) {
		return m_state.Gamepad.bLeftTrigger;
	}

	if (a_buttonIx == 7) {
		return m_state.Gamepad.bRightTrigger;
	}

	return 0;
}

}