// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once


//#ifdef TIMEBREAKER_PROTECTION
class TrialHandler
{
public:
	TrialHandler();
	~TrialHandler(void);

	static void DoStartBrowser();

	
	static float g_timeLeftOfTrial;
};
//#endif