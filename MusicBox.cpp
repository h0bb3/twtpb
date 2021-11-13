// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "stdafx.h"
#include "musicbox.h"

#include "model.h"
#include <DXRenderer/VorbisFile.h>

namespace view
{

static const char* TRACKS[] =
{
	"data/music/levelaa.ogg",
	"data/music/levelab.ogg",
	"data/music/hiscore.ogg",
	"data/music/briefing.ogg",
	"data/music/menu.ogg",
};

MusicBox::MusicBox():
	m_defaultVolume(100),
	m_volume(m_defaultVolume * 0.01f)
{
}

bool MusicBox::Init(const dxrnd::DXSoundEngine& anEngine)
{
	int i;

	for(i = 0; i < T_Count; i++)
	{
		if(!myTracks[i].Init(anEngine, dxrnd::VorbisFile::Create(TRACKS[i])))
			return false;
		myTracks[i].SetVolume(0.f);
	}

	return true;
}

void MusicBox::Register(cmn::PComplex& aComplex)
{
	aComplex.Register("musicVolume", m_defaultVolume, m_defaultVolume);
}

void MusicBox::Terminate()
{
	int i;

	for(i = 0; i < T_Count; i++)
		myTracks[i].Terminate();
}

void MusicBox::Update(const model::Model& aModel, const bool aShowingScores, const bool a_showingCredits, const bool a_showingBriefing)
{
	const model::Model::GameState S(aModel.GetState());
	int i;

	//note that these two tracks are played together to keep them in sync,
	//but the boss track volume is kept at zero until the boss is active
	Enable(T_Level, (S == model::Model::GS_Playing || (S == model::Model::GS_WaitingForNextLevel && !a_showingBriefing)) && aModel.m_isPaused != true);
	Enable(T_Boss, (S == model::Model::GS_Playing || (S == model::Model::GS_WaitingForNextLevel && !a_showingBriefing)) && aModel.m_isPaused != true, aModel.IsBossActive());

	Enable(T_Hiscore, a_showingCredits || aShowingScores || S == model::Model::GS_WaitingForName || S == model::Model::GS_GameCompleted);
	Enable(T_Briefing, a_showingBriefing);
	Enable(T_Menu, !(aShowingScores || a_showingCredits || a_showingBriefing) && (S == model::Model::GS_Starting || aModel.m_isPaused));

	for(i = 0; i < T_Count; i++)
		myTracks[i].Update();
}

void MusicBox::Enable(const Track aTrack, const bool anEnable)
{
	if(m_volume > 0.02f && anEnable)
	{
		if(!myTracks[aTrack].IsPlaying())
			myTracks[aTrack].Play();
		myTracks[aTrack].SetVolume(m_volume);
	}
	else
	{
		if(myTracks[aTrack].IsPlaying())
			myTracks[aTrack].Stop();
	}
}

void MusicBox::Enable(const Track aTrack, const bool anEnable, const bool aVolumeEnable)
{
	Enable(aTrack, anEnable);
	if(anEnable)
		myTracks[aTrack].SetVolume(aVolumeEnable ? m_volume : 0.f);
}

}