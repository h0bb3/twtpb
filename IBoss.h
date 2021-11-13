// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "IEventTarget.h"

namespace model {

class Enemy;

class IBoss {
	public:
		IBoss(void);
		~IBoss(void);

		virtual bool IsAlive() = 0;
		virtual void Update(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty) = 0;
		virtual void Initiate() = 0;
		virtual bool AddEnemy(Enemy *a_pEnemy) = 0;
		virtual float GetLifeLeft() = 0;
		virtual float GetTimeLeft() = 0;
};
}
