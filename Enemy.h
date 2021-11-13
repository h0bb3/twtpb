// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "Beam.h"
#include "Bullet.h"
#include "Gun.h"

#include <math/AABoundingBox.h>

namespace model {

class Enemy {
	public:

		struct TypeParams {
			TypeParams(const math::Vector2 &a_size, int a_health) : m_size(a_size), m_health(a_health) {};
			math::Vector2 m_size;
			int m_health;
		};

		enum Type {
			T_Head = 0,
			T_Ball,
			T_BallBoss,
			T_Drone,
			T_Segment,
			T_Shock,
			T_Spy,
			T_Shock2,
			T_Pill,
			T_Count
		};

		const static TypeParams g_params[T_Count];

		Enemy(void);
		virtual ~Enemy(void);

		bool IsAlive() const;
		void SetAlive(Type a_type, float a_time, float a_scale, int a_waveIx, int a_orderIx, bool a_dropEnergy);
		void Kill(bool a_doNotifyTarget);
		void Damage(int a_damage, bool a_doNotifyTarget);

		void Update(float a_dt, float a_difficulty);
		bool DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet);
		bool DoBeam() const;

		math::Vector2f m_pos;
		math::Vector2f m_oldPos;
		math::Vector2f m_lastOffset;
		math::Vector2f m_offset;
		float m_scale;
		int m_waveIx;	// the wave that spawned this enemy
		int m_orderIx;	// the order of the enemy in the wave
		float m_startTime;	// the starting time of the enemy

		Beam m_beam;
		Gun* m_pGun;

		float m_t;
		
		Type m_type;
		float m_angle;
		int m_life;				// > 0 enemy is alive
		bool m_hasShield;		// true if the enemy cannot be hurt
		bool m_doNotifyTarget;	// true if the eventtarget should be notified that the enemy has died
		bool m_doDropEnergy;	// force the enemy to drop an energy pellet

		void GetBox(math::AABoundingBox2D &a_box);

	private:
		Gun* ConstructGun() const;
		
		
		
};

}
