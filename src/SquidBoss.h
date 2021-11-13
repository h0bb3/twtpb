// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "BossBase.h"
#include "Gun.h"

namespace model {

class SquidBoss : public BossBase {
	public:
		SquidBoss(void);
		~SquidBoss(void);

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
		static const int g_numTentacles = 10;
		static const int g_numTentacleEnemies = 20;
		static const int g_numBodyEnemies = 5;
		static const int g_numEnemies = g_numBodyEnemies + (g_numTentacleEnemies * g_numTentacles);

		class SquidGun : public Gun {
			public:
				SquidGun();
				virtual ~SquidGun();


				enum GunType {
					GT_NULL = 0,
					GT_Slow4Way,
					GT_Funky4Way,
					GT_4Way,
					GT_TriShot,
					GT_Cloud,
					GT_Center,
					GT_Count
				};

				void SetGun(GunType a_gun);

				virtual void Reset();
				virtual void Update(float a_dt);
				virtual bool DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy);

			private:

				NullGun m_nullGun;
				SlowRotating4WayGun m_slowRotating4WayGun;
				FunkyRotating4WayGun m_funky4WayGun;
				Rotating4WayGun m_rotating4WayGun;
				TriShotDirGun m_trishot;
				CloudGun m_cloud;
				InvShockWaveGun m_center;
				Gun *m_guns[GT_Count];	// interface pointers to the above guns

				Gun *m_pCurrentGun;	// interface pointer to one of the above
		};


		struct Frame {
			math::Vector2f m_pos;
			float m_time;
			SquidGun::GunType m_gun;
			float m_shootSpeed;
		};

		static const int g_numFrames = 32;
		static const Frame g_frames[g_numFrames];
		int m_frameIx;
		float m_frameTime;


		Enemy *m_enemies[g_numEnemies];
		math::Vector2f m_speeds[g_numTentacleEnemies * g_numTentacles];
		int m_damage;
		float m_radius;
		const static EnemySettings g_settings[g_numBodyEnemies];
		SquidGun *m_pSquidGun;


};

}
