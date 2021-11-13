// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include <DXRenderer/DXSound.h>

namespace cmn
{
	class PComplex;
}

namespace view
{

class SoundBox
{
public:

	SoundBox();
	void Init(const dxrnd::DXSoundEngine& anEngine);
	void Register(cmn::PComplex& aComplex);

	enum Type
	{
		S_PlayerShoot = 0,
		S_EnemyExplode,
		S_EnemyDamage,
		S_PlayerExplode,
		S_EnergyPickup,
		S_NULL,
		S_WeaponPickup,
		S_PlayerHit,
		S_MissileFire,
		S_MissileHit,
		S_GuiClick,
		S_GuiHot,
		S_Count
	};

	class Sound
	{
	public:

		Sound();
		void Init(const dxrnd::DXSoundEngine& anEngine, const char* aFile, int a_numCopies);
		void Play(const float a_volume);

		//members
		int myVolume;

	private:

		//members
		dxrnd::DXSound mySound;
	};

	//members
	Sound mySounds[S_Count];
	float m_volume;	// global volume for all sounds
};

}