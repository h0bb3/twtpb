// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "InputDeviceFactory.h"

#define _CRT_SECURE_NO_DEPRECATE 
#ifndef _WIN32_DCOM
#define _WIN32_DCOM 
#endif

/*
#include <windows.h>
#include <commctrl.h>
#include <basetsd.h>
#include <dinput.h>
#include <dinputd.h>
#include <assert.h>
#include <shellapi.h>
*/

#include <wbemidl.h>
#include <oleauto.h>

namespace dxrnd {

InputDeviceFactory::InputDeviceFactory(void) {
}

InputDeviceFactory::~InputDeviceFactory(void) {
}

InputDevice *InputDeviceFactory::CreateDevice(IDirectInput8 &a_dInput, HWND a_hWnd) {
	HRESULT hr;
	ControllerCallbackData callbackData;

	callbackData.m_pDInput = &a_dInput;
	callbackData.m_pDevice = NULL;
	callbackData.m_hWnd = a_hWnd;
	hr = a_dInput.EnumDevices(DI8DEVCLASS_GAMECTRL, EnumGameControllerCallback, &callbackData, DIEDFL_ATTACHEDONLY);
	if(FAILED(hr)) {
		delete callbackData.m_pDevice;
		callbackData.m_pDevice = NULL;
	}

	return callbackData.m_pDevice;
}

BOOL CALLBACK InputDeviceFactory::EnumGameControllerCallback(LPCDIDEVICEINSTANCE a_pDeviceObject, VOID* a_pCallbackData) {
	ControllerCallbackData &data(*(ControllerCallbackData*)a_pCallbackData);
	LPDIRECTINPUTDEVICE8 pDevice;
	HRESULT hr;
	hr = data.m_pDInput->CreateDevice(a_pDeviceObject->guidInstance, &pDevice, NULL);
	if(FAILED(hr)) {
		return DIENUM_CONTINUE;
	}

	if (IsXInputDevice(&a_pDeviceObject->guidProduct)) {
		LOG_3_A2("Found XInput Game Controller: %s, %s", a_pDeviceObject->tszProductName, a_pDeviceObject->tszInstanceName);LOG_3_A2("Found Game Controller: %s, %s", a_pDeviceObject->tszProductName, a_pDeviceObject->tszInstanceName);

		data.m_pDevice = XInputDevice::Create(pDevice);

		if (data.m_pDevice == NULL) {
			return DIENUM_CONTINUE;
		}
		LOG_3_A2("Using Game Controller: %s, %s", a_pDeviceObject->tszProductName, a_pDeviceObject->tszInstanceName);

	} else {
		LOG_3_A2("Found DInput Game Controller: %s, %s", a_pDeviceObject->tszProductName, a_pDeviceObject->tszInstanceName);LOG_3_A2("Found Game Controller: %s, %s", a_pDeviceObject->tszProductName, a_pDeviceObject->tszInstanceName);
		data.m_pDevice = DInputDevice::Create(pDevice, data.m_hWnd);

		if (data.m_pDevice == NULL) {
			return DIENUM_CONTINUE;
		}
		LOG_3_A2("Using Game Controller: %s, %s", a_pDeviceObject->tszProductName, a_pDeviceObject->tszInstanceName);
	}

	return DIENUM_CONTINUE;
}

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }
bool InputDeviceFactory::IsXInputDevice(const GUID* a_pGuidProductFromDirectInput) {
	IWbemServices*          pIWbemServices = NULL;
    IEnumWbemClassObject*   pEnumDevices   = NULL;
    IWbemLocator*           pIWbemLocator  = NULL;
    IWbemClassObject*       pDevices[20]   = {0};
    BSTR                    bstrDeviceID   = NULL;
    BSTR                    bstrClassName  = NULL;
    BSTR                    bstrNamespace  = NULL;
    DWORD                   uReturned      = 0;
    bool                    bCleanupCOM    = false;
    UINT                    iDevice        = 0;
    VARIANT                 var;
    HRESULT                 hr;
	bool					ret(false);

    // CoInit if needed
    hr = CoInitialize(NULL);
    bCleanupCOM = SUCCEEDED(hr);

    // Create WMI
    hr = CoCreateInstance( __uuidof(WbemLocator),
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           __uuidof(IWbemLocator),
                           (LPVOID*) &pIWbemLocator);
    if( FAILED(hr) || pIWbemLocator == NULL ) 
        goto LCleanup; 

    // Create BSTRs for WMI
    bstrNamespace = SysAllocString( L"\\\\.\\root\\cimv2" ); if( bstrNamespace == NULL ) goto LCleanup;
    bstrDeviceID  = SysAllocString( L"DeviceID" );           if( bstrDeviceID == NULL )  goto LCleanup;        
    bstrClassName = SysAllocString( L"Win32_PNPEntity" );    if( bstrClassName == NULL ) goto LCleanup;        
    
    // Connect to WMI 
    hr = pIWbemLocator->ConnectServer( bstrNamespace, NULL, NULL, 0L, 
                                       0L, NULL, NULL, &pIWbemServices );
    if( FAILED(hr) || pIWbemServices == NULL )
        goto LCleanup; 

    // Switch security level to IMPERSONATE
    CoSetProxyBlanket( pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, 
                       RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0 );                    

    // Get list of Win32_PNPEntity devices
    hr = pIWbemServices->CreateInstanceEnum( bstrClassName, 0, NULL, &pEnumDevices ); 
    if( FAILED(hr) || pEnumDevices == NULL )
        goto LCleanup;

    // Loop over all devices
    for( ;; )
    {
        // Get 20 at a time
        hr = pEnumDevices->Next( 10000, 20, pDevices, &uReturned );
        if( FAILED(hr) )
            goto LCleanup;
        if( uReturned == 0 )
            break;

        for( iDevice=0; iDevice<uReturned; iDevice++ )
        {
            // For each device, get its device ID
            hr = pDevices[iDevice]->Get( bstrDeviceID, 0L, &var, NULL, NULL );
            if( SUCCEEDED( hr ) && var.vt == VT_BSTR && var.bstrVal != NULL )
            {
                // Check if the device ID contains "IG_".  If it does, then it�s an XInput device
                // Unfortunately this information can not be found by just using DirectInput 
                if( wcsstr( var.bstrVal, L"IG_" ) )
                {
                    // If it does, then get the VID/PID from var.bstrVal
                    DWORD dwPid = 0, dwVid = 0;
                    WCHAR* strVid = wcsstr( var.bstrVal, L"VID_" );
                    if( strVid && swscanf( strVid, L"VID_%4X", &dwVid ) != 1 )
                        dwVid = 0;
                    WCHAR* strPid = wcsstr( var.bstrVal, L"PID_" );
                    if( strPid && swscanf( strPid, L"PID_%4X", &dwPid ) != 1 )
                        dwPid = 0;

                    DWORD dwVidPid = MAKELONG( dwVid, dwPid );

					if (dwVidPid == a_pGuidProductFromDirectInput->Data1) {
						ret = true;
						goto LCleanup;
					}
                }
            }   
            SAFE_RELEASE( pDevices[iDevice] );
        }
    }

LCleanup:
    if(bstrNamespace)
        SysFreeString(bstrNamespace);
    if(bstrDeviceID)
        SysFreeString(bstrDeviceID);
    if(bstrClassName)
        SysFreeString(bstrClassName);
    for( iDevice=0; iDevice<20; iDevice++ )
        SAFE_RELEASE( pDevices[iDevice] );
    SAFE_RELEASE( pEnumDevices );
     SAFE_RELEASE( pIWbemLocator );
    SAFE_RELEASE( pIWbemServices );

    return ret;
}

}