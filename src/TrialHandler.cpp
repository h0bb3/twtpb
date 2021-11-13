// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include ".\trialhandler.h"
#include "RegistryWrapper.h"
#include "securedsections.h"
#include <Shellapi.h>

//Registry key
#define  g_TrialKey "SOFTWARE\\SPELL_OF_PLAY_STUDIOS\\TWTPB\\17"
#define g_TrialValueName "cd1"
#define g_TrialKeyValueName "key"

static const DWORD g_TrialTotalTime = 45*60; //should be 30*60
static const DWORD g_TrialExtraTime = 1*30;

bool GetValue(const char* a_keyName, const char* a_valueName, char *a_pData);

//Asprotect functions...
void ExecuteTrial() {
	DWORD time = g_TrialExtraTime; //you get two minutes by default
	
	RegistryWrapper wrapper(g_TrialKey);
	if (wrapper.GetValue(g_TrialValueName, time) == false) { //Get the value from registry

		//No value was found customer gets 30 min
		if (wrapper.SetValue(g_TrialValueName, g_TrialTotalTime)) { //If no value was found set the value to 30 min
			time = g_TrialTotalTime; 
		}
	}
	if (time < g_TrialExtraTime) { //If the value was less than 2 minutes get 2 minutes...
		time = g_TrialExtraTime;
	} else if (time > g_TrialTotalTime) { //if the value has been tampered with you only get 2 min
		time = g_TrialExtraTime;
	}
	TrialHandler::g_timeLeftOfTrial = (float)time;
}

float TrialHandler::g_timeLeftOfTrial = g_TrialExtraTime;

TrialHandler::TrialHandler() {
	{
		SECUREBEGIN_A 
		{
			ExecuteTrial();
		}
		SECUREEND;
	}
}

TrialHandler::~TrialHandler(void)
{
	{
		SECUREBEGIN_A
		{
			DWORD time = (DWORD)TrialHandler::g_timeLeftOfTrial;
			RegistryWrapper wrapper(g_TrialKey);
			if (wrapper.SetValue(g_TrialValueName, time)) {
				//success	
			}
		}
		SECUREEND
	}
}



void TrialHandler::DoStartBrowser() {
	SHELLEXECUTEINFO info;
						
	ZeroMemory( &info, sizeof(info) );
	info.cbSize = sizeof(info);
	info.lpVerb = "open";
	info.lpFile = "https://secure.bmtmicro.com/servlets/Orders.ShoppingCart?CID=2731&PRODUCTID=27310000";
	LONG r = ::ShellExecuteEx(&info);
}