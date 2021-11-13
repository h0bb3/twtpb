// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "stdafx.h"
#include "mixerview.h"

namespace view
{

static const char* EFFECT_NAMES[] =
{
	"SE_PlayerShoot",
	"SE_EnemyExplode",
	"SE_EnemyDamage",
	"SE_PlayerExplode",
	"SE_EnergyPickup",
	"SE_LifePickup",
	"SE_WeaponPickup",
	"SE_PlayerHit",
	"SE_MissileFire",
	"SE_MissileHit",
	"SE_GuiClick",
	"SE_GuiHot"
};

//View implementation
View::ViewStatus MixerView::Render(float a_dt, IEventTarget &a_target)
{
	int i, y(128);

	UpdateKeys();

	//select fader
	if(m_core.m_input.IsClicked(VK_UP))
	{
		mySelection--;
		if(mySelection < 0)
			mySelection = SoundBox::S_Count;	//one extra for music
	}
	if(m_core.m_input.IsClicked(VK_DOWN))
	{
		mySelection++;
		if(mySelection > SoundBox::S_Count)	//one extra for music
			mySelection = 0;
	}

	//effects faders
	for(i = 0; i < SoundBox::S_Count; i++)
	{
		if(Fader(i, y, EFFECT_NAMES[i], m_core.m_assets.m_sfx.mySounds[i].myVolume))
			m_core.m_assets.m_sfx.mySounds[i].Play(m_core.m_assets.m_sfx.m_volume);
		y += 8;
	}

	//music fader
	if (Fader(SoundBox::S_Count, y, "Music", m_core.m_assets.m_music.m_defaultVolume)) {
		m_core.m_assets.m_music.m_volume = m_core.m_assets.m_music.m_defaultVolume * 0.01f;
	}

	//instructions
	y = 256;
	Label(320, y, "UP/DOWN to select fader");
	Label(320, y += 8, "LEFT/RIGHT to adjust volume");
	Label(320, y += 8, "SHIFT to increase step");
	Label(320, y += 16, "Sorry, no keyrepeat :(");

	return View::VS_Ok;
}

//specific interface
MixerView::MixerView(const model::Model &a_model, ViewCore &a_core)
:View(a_model, a_core)
,mySelection(0)
,myLastLeft(false)
,myLeft(false),
myLastRight(false),
myRight(false)
{
}

//private
bool MixerView::Fader(const int anIndex, const int aY, const char* aName, int& aVolume)
{
	int v;

	if(anIndex == mySelection)
		Label(g_width / 3 - 16, aY, "->");
	v = Fader(aY, aName, aVolume);
	if(anIndex == mySelection)
	{
		if(aVolume != v)
		{
			aVolume = v;
			return true;
		}
	}

	return false;
}

int MixerView::Fader(const int aY, const char* aName, const int aVolume)
{
	cmn::String str;

	Label(g_width / 3, aY, aName);
	str.Format("%d", aVolume);
	Label(g_width / 3 * 2, aY, str.c_str());

	if(Left())
	{
		int v(aVolume);
		v -= GetAsyncKeyState(VK_SHIFT) ? 10 : 1;
		if(v < 0)
			v = 0;
		return v;
	}

	if(Right())
	{
		int v(aVolume);
		v += GetAsyncKeyState(VK_SHIFT) ? 10 : 1;
		if(v > 100)
			v = 100;
		return v;
	}

	return aVolume;
}

void MixerView::UpdateKeys()
{
	myLastLeft = myLeft;
	myLeft = GetAsyncKeyState(VK_LEFT) != 0;
	myLastRight = myRight;
	myRight = GetAsyncKeyState(VK_RIGHT) != 0;
}

void MixerView::Label(const int aX, const int aY, const char* aLabel)
{
	m_core.DoLabel(aX, aY, g_width, aY + 1, aLabel, 0);
}

}