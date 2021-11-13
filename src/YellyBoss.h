// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "BossBase.h"

namespace model {

class YellyBoss : public BossBase {
	public:
		YellyBoss(void);
		~YellyBoss(void);

		bool IsAlive();
		float GetLifeLeft();
		bool AddEnemy(Enemy *a_pEnemy);
	protected:
		void DoUpdate(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty);
		void DoInitiate();

	private:
		void SimulateTentacles(float a_dt, float a_scale, const math::Vector2f &a_playerPos);

		enum State {
			S_Normal,
			S_Mad,
			S_Mader,
			S_Dying
		};

		State GetState();
		int GetMaxDamage() const;
		bool HasLiveTentacle();

		class EnemySettings {
			public:
				EnemySettings(const math::Vector2f &a_pos, const float a_scale) : m_pos(a_pos), m_scale(a_scale) {};
				const math::Vector2f m_pos;
				const float m_scale;
		};

		int m_addedEnemies;
		float m_t;
		static const int g_numTentacles = 5;
		static const int g_numTentacleEnemies = 20;
		static const int g_numBodyEnemies = 9;
		static const int g_numEnemies = g_numBodyEnemies + (g_numTentacleEnemies * g_numTentacles);

		Enemy *m_enemies[g_numEnemies];
		math::Vector2f m_speeds[g_numTentacleEnemies * g_numTentacles];
		int m_maxDamage;
		int m_damage;
		float m_radius;
		const static EnemySettings g_settings[g_numBodyEnemies];


};

}
