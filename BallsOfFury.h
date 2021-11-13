// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once
#include "Enemy.h"
#include "BossBase.h"

namespace model {

class BallsOfFury  : public BossBase {
	public:
		BallsOfFury(void);
		virtual ~BallsOfFury(void);

		static const int g_ballCount = 16;

		bool IsAlive();

		float GetLifeLeft();
		bool AddEnemy(Enemy *a_pEnemy);
	protected:
		void DoUpdate(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty);
		void DoInitiate();
	private:
		int m_enemiesAdded; 
		Enemy *m_enemies[g_ballCount];
		math::Vector2f m_swarmPositions[g_ballCount];

		bool m_hasBeamed;
		float m_stateTime;
		float m_time;
		int m_maxLife;

		enum State {
			S_Ball,
			S_Swarm
		};

		State m_state;


	void DoBall();
	void SetCenterBallPosition();

	Enemy m_phony;
};

}