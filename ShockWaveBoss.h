// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once
#include "Enemy.h"
#include "BossBase.h"

namespace model {

class ShockWaveBoss : public BossBase {
	public:
		ShockWaveBoss(void);
		~ShockWaveBoss(void);

		bool IsAlive();

		float GetLifeLeft();
		bool AddEnemy(Enemy *a_pEnemy);
	protected:
		void DoUpdate(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty);
		void DoInitiate();

	private:
		static const int g_numEnemies = 21;
		float m_time;
		int m_enemiesAdded; 
		Enemy *m_enemies[g_numEnemies];
		math::Vector2f m_swarmPositions[g_numEnemies];
		Enemy m_phony;
		int m_maxLife;


};

}