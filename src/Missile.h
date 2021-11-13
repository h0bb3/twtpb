// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include <math/AABoundingBox.h>

namespace model {
	class Enemy;

	class Missile {
		public:
			Missile(void);
			virtual ~Missile(void);
			void Fire(math::Vector2f a_pos);
			bool IsAlive() const;
			void Kill();
			void GetBox(math::AABoundingBox2D &a_box);

			void Update(float a_dt, const Enemy *a_pEnemies, int a_numEnemies, const math::Vector2f &a_playerPos);
			math::Vector2f m_pos;
			math::Vector2f m_oldPos;
		private:

			math::Vector2f m_speed;
			const Enemy *m_pSelectedEnemy;
			float m_time;
	};
}
