// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "BossBase.h"

namespace model {
class TwinBoss : public BossBase {
	public:
		TwinBoss(void);
		virtual ~TwinBoss(void);

		bool IsAlive();
		bool AddEnemy(Enemy *a_pEnemy);
		float GetLifeLeft();

	protected:
		void DoInitiate();
		void DoUpdate(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty);

		static const int g_numBodyEnemies = 5;
		static const int g_numBodies = 2;
		static const int g_numEnemies = g_numBodies * g_numBodyEnemies;
		static const int g_maxLife = 800;

		float m_beamSpeeds[g_numBodies];
		Enemy *m_enemies[g_numEnemies];
		int m_addedEnemies;
		float m_time;

		struct BeamKey {
			const float m_time;
			const float m_ttl;
			const float m_speed;
			bool m_hasTriggered;
		};

		static const int g_numBeamKeys = 5;
		static BeamKey g_beamKeys[g_numBodies * g_numBeamKeys];
		float m_beamKeyTime;
		float m_maxBeamTime;

};
}
