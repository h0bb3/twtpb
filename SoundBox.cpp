// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "stdafx.h"
#include "soundbox.h"

#include <Common/PComplex.h>

namespace view
{

SoundBox::SoundBox() : m_volume(1.0f) {
}

void SoundBox::Init(const dxrnd::DXSoundEngine& anEngine)
{
	mySounds[S_PlayerShoot].Init(anEngine, "data/sounds/SE_PlayerShoot.wav", 1);
	mySounds[S_EnemyExplode].Init(anEngine, "data/sounds/SE_EnemyExplode.wav", dxrnd::DXSound::MAX_VARIANTS);
	mySounds[S_EnemyDamage].Init(anEngine, "data/sounds/SE_EnemyDamage.wav", 1);
	mySounds[S_PlayerExplode].Init(anEngine, "data/sounds/SE_PlayerExplode.wav", 1);
	mySounds[S_EnergyPickup].Init(anEngine, "data/sounds/SE_EnergyPickup.wav", 1);
	//mySounds[S_LifePickup].Init(anEngine, "data/sounds/SE_LifePickup.wav", 1);
	mySounds[S_WeaponPickup].Init(anEngine, "data/sounds/SE_WeaponPickup.wav", 1);
	mySounds[S_PlayerHit].Init(anEngine, "data/sounds/SE_PlayerHit.wav", 1);
	mySounds[S_MissileFire].Init(anEngine, "data/sounds/SE_MissileFire.wav", 1);
	mySounds[S_MissileHit].Init(anEngine, "data/sounds/SE_MissileHit.wav", dxrnd::DXSound::MAX_VARIANTS);
	mySounds[S_GuiClick].Init(anEngine, "data/sounds/SE_GuiClick.wav", 1);
	mySounds[S_GuiHot].Init(anEngine, "data/sounds/SE_GuiHot.wav", 1);
}

void SoundBox::Register(cmn::PComplex& aComplex)
{
	int i;
	cmn::String str;

	for(i = 0; i < S_Count; i++)
	{
		str.Format("sfxVolume%d", i);
		aComplex.Register(str, mySounds[i].myVolume, mySounds[i].myVolume);
	}
}

SoundBox::Sound::Sound()
:myVolume(100)
{
}

void SoundBox::Sound::Init(const dxrnd::DXSoundEngine& anEngine, const char* aFile, int a_numCopies)
{
	for (int i = 0; i < a_numCopies && i < dxrnd::DXSound::MAX_VARIANTS; i++ ) {
		mySound.AddSound(anEngine, aFile);
	}
}

void SoundBox::Sound::Play(const float a_volume)
{
	mySound.Play((float)myVolume * 0.01f * a_volume);
}


}