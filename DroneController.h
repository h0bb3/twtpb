// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "Enemy.h"

namespace model {

class DroneController {
	public:
		DroneController(void);
		~DroneController(void);

		bool Update(float a_dt);

		typedef std::list<Enemy*> EnemyList;
		EnemyList m_enemies;
		float m_time;
		float m_stateTime;
		float m_fleeTimer;
		bool m_doFlee;
};

}
