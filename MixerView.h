// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "view.h"

namespace view
{

class MixerView : public View
{
public:

	//View implementation
	ViewStatus Render(float a_dt, IEventTarget &a_target);

	//specific interface
	MixerView(const model::Model &a_model, ViewCore &a_core);

private:

	bool Fader(const int anIndex, const int aY, const char* aName, int& aVolume);
	int Fader(const int aY, const char* aName, const int aVolume);
	void UpdateKeys();
	bool Left() const	{return myLeft && !myLastLeft;}
	bool Right() const	{return myRight && !myLastRight;}
	void Label(const int aX, const int aY, const char* aLabel);

	//members
	int mySelection;
	bool myLastLeft, myLeft, myLastRight, myRight;
};

}