// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include ".\registrywrapper.h"

RegistryWrapper::~RegistryWrapper(void) {
}


RegistryWrapper::RegistryWrapper(cmn::String a_keyName) {
	m_keyName = a_keyName;
}
		
bool RegistryWrapper::GetValue(cmn::String a_valueName, DWORD &a_value) {
	HKEY hkResult;
	LONG ret = RegOpenKeyEx(HKEY_CURRENT_USER, m_keyName.c_str(), 0, KEY_READ | KEY_WRITE, &hkResult);
	if (ret == ERROR_SUCCESS) {
		DWORD size = sizeof(a_value);
		DWORD type;
		DWORD value;
		
		LONG retQuery = RegQueryValueEx(hkResult, a_valueName.c_str(), 0, &type, (BYTE*)&value, &size);
		RegCloseKey(hkResult);
		if (retQuery == ERROR_SUCCESS) {
			
			value ^= 761421162;
			value -= 1919;
			value /= 1000;
			a_value = value;
			return true;
		}
	} 
	return false;
}


bool RegistryWrapper::SetValue(cmn::String a_valueName, DWORD a_value) {
	HKEY hkResult;
	DWORD reused = 0;
	LONG ret = RegOpenKeyEx(HKEY_CURRENT_USER, m_keyName.c_str(), 0, KEY_READ | KEY_WRITE, &hkResult);
	if (ret != ERROR_SUCCESS) {
		ret = RegCreateKeyEx(HKEY_CURRENT_USER, m_keyName.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hkResult, &reused);
		if (ret != ERROR_SUCCESS) {
			return false;
		}
	}

	DWORD value = a_value; 
	value *= 1000;
	value += 1919;
	value ^= 761421162;

	if (ERROR_SUCCESS == RegSetValueEx(hkResult, a_valueName.c_str(), 0, REG_DWORD, (const BYTE*)&value, sizeof(value)) ) {
		RegCloseKey(hkResult);

		//SUCCESS
		return true;
	} else {
		return false;
	}

	return false;
}

bool RegistryWrapper::SetValue(cmn::String a_valueName, cmn::String a_value) {
	HKEY hkResult;
	DWORD reused = 0;
	LONG ret = RegOpenKeyEx(HKEY_CURRENT_USER, m_keyName.c_str(), 0, KEY_READ | KEY_WRITE, &hkResult);
	if (ret != ERROR_SUCCESS) {
		ret = RegCreateKeyEx(HKEY_CURRENT_USER, m_keyName.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hkResult, &reused);
		if (ret != ERROR_SUCCESS) {
			return false;
		}
	}

	if (ERROR_SUCCESS == RegSetValueEx(hkResult, a_valueName.c_str(), 0, REG_SZ, (const BYTE*)(a_value.c_str()), (DWORD)a_value.length()) ) {
		RegCloseKey(hkResult);

		//SUCCESS
		return true;
	} else {
		return false;
	}

	return false;
}

