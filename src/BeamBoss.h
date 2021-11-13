// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "BossBase.h"

namespace model {

class Enemy;

class BeamBoss : public BossBase {
	public:
		BeamBoss(void);
		virtual ~BeamBoss(void);

		bool IsAlive();
		bool AddEnemy(Enemy *a_pEnemy);
		float GetLifeLeft();
	protected:
		void DoUpdate(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty);
		void DoInitiate();
	private:

		const static int g_maxSideDamage = 100;
		const static int g_maxCornerDamage = 200;
		const static int g_maxCenterEnemyDamage = 30;
		const static int g_numCornerEnemies = 4;
		const static int g_numSidesEnemies = 8;
		const static int g_numCenterEnemies = 1;
		const static int g_numEnemies = g_numCornerEnemies + g_numSidesEnemies + g_numCenterEnemies;
		const static int g_maxDamage = g_maxSideDamage * g_numSidesEnemies + g_maxCornerDamage * g_numCornerEnemies + g_numCenterEnemies * g_maxCenterEnemyDamage;
		int m_addedEnemies;
		Enemy *m_enemies[g_numEnemies];

		float m_beamTimer;
		float m_angle;
};

}