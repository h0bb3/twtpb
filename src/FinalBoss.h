// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once
#include "BossBase.h"

namespace model {
class FinalBoss : public BossBase {
	public:
		FinalBoss(void);
		virtual ~FinalBoss(void);

		bool AddEnemy(Enemy *a_pEnemy);

		bool IsAlive();
		float GetLifeLeft();

	protected:
		virtual void DoInitiate();
		virtual void DoUpdate(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty);

	private:
		static const int g_numEnemies = 1;
		Enemy *m_enemies[g_numEnemies];
		float m_time;
};
}
