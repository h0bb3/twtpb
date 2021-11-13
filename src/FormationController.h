// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "Enemy.h"

namespace model {
class FormationController {
	public:
		FormationController(void);
		virtual ~FormationController(void);

		enum Formation {
			F_VerticalSine = 0,
			F_HorizontalSineLR,
			F_EllipticLR,
			F_HorizontalSineRL,
			F_EllipticRL,
			F_VerticalSineLeft,
			F_VerticalSineRight,
			F_VerticalSineOffset,
			F_CircleLRBottom,
			F_CircleRLBottom,
			F_SQUARE_CW_1,
			F_SQUARE_CW_2,
			F_Slash_Back,
			F_Slash_Front,
			F_Pause,
			F_FunkLeft,
			F_FunkRight,
			F_FunkMiddleLeft,
			F_FunkMiddleRight,
			F_VerticalCatterpillar,
			F_HorizontalCatterpillarLR,
			F_VerticalFlower,
			F_HorizontalCatterpillarRL,
			F_Count
		};

		bool Update(float a_dt, const float a_difficulty);
		void RemoveDeadEnemies();

		typedef std::list<Enemy*> EnemyList;


		float m_speedMultiplier;
		const EnemyList &m_enemies;	// const ref to internal list
		void AddEnemy(Enemy *a_pEnemy);

		bool m_enemyEscaped; // set to true if an enemy has slipped passed the player

		Formation m_currentFormation;
		math::Vector2f m_currentOffset;
	private:
		EnemyList m_enemiesI;
		size_t m_originalEnemiesSize;
		float ComputeArea(float a_t);
};

}