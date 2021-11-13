// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "../engineiii/common/stringdef.h"

class RegistryWrapper
{
	public:
		RegistryWrapper(cmn::String a_keyName);
		~RegistryWrapper(void);
		
		bool GetValue(cmn::String a_valueName, DWORD &a_value);
		bool SetValue(cmn::String a_valueName, DWORD a_value);
		bool SetValue( cmn::String a_valueName, cmn::String a_value);
	private:
		cmn::String m_keyName;
};


