// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "BossBase.h"

namespace model {

class SnakeBoss : public BossBase {
	public:
		SnakeBoss(void);
		~SnakeBoss(void);

		virtual bool IsAlive();
		virtual bool AddEnemy(Enemy *a_pEnemy);
		virtual float GetLifeLeft();
	protected:
		void DoUpdate(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty);
		void DoInitiate();

	private:
		static const int g_numEnemies = 99;
		int GetDamageForSnakeSegment(int a_numSnakeSegments) const;

		int m_addedEnemies;	
		model::Enemy *m_enemies[g_numEnemies];
		math::Vector2f m_speeds[g_numEnemies];
		float m_time;
};

}
