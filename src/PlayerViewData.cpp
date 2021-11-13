// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "ViewAssets.h"

namespace view {

ViewAssets::PlayerViewData::PlayerViewData(void) : cmn::PComplex("playerViewData") {
	Register("leftEnginePos", m_lEnginePos, 0.0f, 0.0f, 0.0f);
	Register("rightEnginePos", m_rEnginePos, 0.0f, 0.0f, 0.0f);
	Register("engineParticleStartSize", m_engineParticleStartSize, 0.0f, 0.0f);
	Register("engineParticleEndSize", m_engineParticleEndSize, 0.0f, 0.0f);
	Register("engineParticleSpeed", m_engineParticleSpeed, 0.0f, 0.0f, 0.0f);

	for (int i = 0; i < model::Player::g_maxGuns; i++) {
		cmn::String str;
		str.Format("gunMount_%i", i);
		Register(str, m_gunMounts[i], 0.0f, 0.0f, 0.0f);
	}
}

ViewAssets::PlayerViewData::~PlayerViewData(void) {
}

}