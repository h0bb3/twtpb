// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include <DXRenderer/DXStreamSound.h>

namespace dxrnd
{
	class DXSoundEngine;
}

namespace model
{
	class Model;
}

namespace cmn
{
	class PComplex;
}

namespace view
{

class MusicBox
{
public:

	MusicBox();
	bool Init(const dxrnd::DXSoundEngine& anEngine);
	void Register(cmn::PComplex& aComplex);
	void Terminate();
	void Update(const model::Model& aModel, const bool aShowingScores, const bool a_showingCredits, const bool a_showingBriefing);

	//members
	enum Track
	{
		T_Level = 0,
		T_Boss,
		T_Hiscore,
		T_Briefing,
		T_Menu,
		T_Count,
	};

	int m_defaultVolume;
	float m_volume;	// user edited
	dxrnd::DXStreamSound myTracks[T_Count];

private:

	void Enable(const Track aTrack, const bool anEnable);
	void Enable(const Track aTrack, const bool anEnable, const bool aVolumeEnable);
};

}