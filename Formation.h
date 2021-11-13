// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "Enemy.h"

#include <Common/Pcomplex.h>

#include "FormationController.h"

namespace model {

	class Formation : public cmn::PComplex {
	public:
		Formation(void);
		Formation(const Formation &a_copy);
		Formation& operator = (const Formation &a_copy);
		~Formation(void);

		struct Enemy : public cmn::PComplex {
			Enemy();
			Enemy(const Enemy &a_copy);
			Enemy& operator = (const Enemy &a_copy);
			
			float m_timeOffset;
			::model::Enemy::Type m_enemyType;
			math::Vector2f m_offset;
			float m_scale;
			bool m_hasShield;

			void SetEnemyAlive(model::Enemy &a_enemy, int a_waveIx, int a_orderIx) const;

			private:
				void RegisterVariables();
		};

		typedef std::vector<Enemy> EnemyList;


		EnemyList m_enemies;
		int m_levelIx;
};

}
