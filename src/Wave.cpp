// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "Wave.h"

namespace model {

Wave::Wave(void) : cmn::PComplex("w") {
	RegisterVariables();
}

Wave::Wave(const Wave &a_copy) : cmn::PComplex(a_copy) {
	RegisterVariables();
	*this = a_copy;
}

Wave::~Wave(void) {
}


void Wave::RegisterVariables() {
	m_hasTriggered = false;
	Register("f", m_formationIx, -1);
	Register("fc", (int&)m_controller, (int)FormationController::F_VerticalSine);
	Register("tt", m_triggerTime, 0.0f);
	Register("lIx", m_levelIx, -1);
	Register("sm", m_speedMultiplier, 1.0f);
	Register("wd", m_dropWeapon, 0);
	Register("off", m_offset, 0.0f, 0.0f);

}


}