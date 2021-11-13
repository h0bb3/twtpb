// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "BossBase.h"
#include "Enemy.h"

namespace model {
class ProjectXBoss : public BossBase {
	public:
		ProjectXBoss(void);
		~ProjectXBoss(void);

		// IBoss Realization
		virtual bool IsAlive();
		virtual bool AddEnemy(Enemy *a_pEnemy);
		float GetLifeLeft();

	protected:
		virtual void DoUpdate(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty);
		virtual void DoInitiate();

	private:
		const static int g_numTentacleEnemies = 5;
		const static int g_numTentacles = 5;
		const static int g_numBodyEnemies = 1;
		const static int g_numEnemies = g_numBodyEnemies + g_numTentacleEnemies * g_numTentacles;
		Enemy *m_enemies[g_numEnemies];
		int m_addedEnemies;
		float m_time;
		int m_maxLife;
};

}