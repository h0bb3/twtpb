// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include <Common/Pcomplex.h>
#include "FormationController.h"

namespace model {
// represents a specific formation at a specific time, with a specific controller
class Wave : public cmn::PComplex{
	public:
		Wave(void);
		Wave(const Wave &a_copy);
		~Wave(void);

		int m_formationIx;
		FormationController::Formation m_controller;
		float m_triggerTime;
		float m_speedMultiplier;
		int m_levelIx;
		bool m_hasTriggered;
		int m_dropWeapon;		// the weapon ix to drop 0 is no weapon drop
		bool m_hasShield;		// turn on or off shield
		math::Vector2f m_offset;

	private:
		void RegisterVariables();
};

}
