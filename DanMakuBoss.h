// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once
#include "BossBase.h"

namespace model {

class DanMakuBoss : public BossBase {
	public:
		DanMakuBoss(void);
		virtual ~DanMakuBoss(void);

		// IBoss Realization
		bool IsAlive();
		bool AddEnemy(Enemy *a_pEnemy);
		float GetLifeLeft();
	protected:
		void DoUpdate(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty);
		void DoInitiate();

	private:
		void DoGuns(int a_gunCount, float a_dt, float a_rotFactor, float a_rotSpeed, float a_pause, float a_rateOfFire, const float a_difficulty);

		enum Wave {
			W_Sphere = 0,
			W_DoubleSphere,
			W_LinesCCW,
			W_Lines,
			W_Count
		};

		static const int g_numBodyEnemies = 1;
		static const int g_numGunEnemies = 7;
		static const int g_bulletEnemyStartIx = g_numBodyEnemies + g_numGunEnemies;
		static const int g_numBulletEnemies = 64;
		static const int g_numEnemies = g_numBodyEnemies + g_numGunEnemies + g_numBulletEnemies;
		static const int g_life = 2000;

		float m_time;
		float m_waveTime;
		float m_angle;
		int m_addedEnemies;
		float m_gunPauseTime;

		Wave m_currentWave;
		
		Enemy *m_enemies[g_numEnemies];
};

}
