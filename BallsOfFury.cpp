// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include ".\ballsoffury.h"
#include "IEventTarget.h"

namespace model {

BallsOfFury::BallsOfFury(void) : BossBase(60.0f * 3.0f) {

	for (int i = 0; i < g_ballCount; i++) {
		m_enemies[i] = &m_phony;
	}
	m_state = S_Ball;
	m_hasBeamed = false;
	m_phony.m_life = 0;

}

BallsOfFury::~BallsOfFury(void) {
}

float BallsOfFury::GetLifeLeft() {
	if (IsAlive()) {
		int life(0);
		for (int eIx = 0; eIx < g_ballCount; eIx++) {
			life += m_enemies[eIx]->m_life;
		}

		return (float)life / (float)m_maxLife;
	}
	return 0.0f;
}

bool BallsOfFury::IsAlive() {
	for (int i = 0; i < g_ballCount; i++) {
		if (m_enemies[i]->IsAlive() == true) {
			return true;
		} else {
			m_enemies[i] = &m_phony;
		}
	}

	return false;
}

void BallsOfFury::DoUpdate(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty) {
	m_stateTime += a_dt;
	m_time += a_dt;

	if (m_stateTime < 1.0f) {
		SetCenterBallPosition();
		m_enemies[0]->m_pos.x += (400.0f - m_enemies[0]->m_pos.x) * (1.0f - m_stateTime);
		m_enemies[0]->m_pos.y -= (1.0f - m_stateTime) * 250.0f;
		DoBall();
	} else if (m_state == S_Ball) {

		SetCenterBallPosition();
		DoBall();
		if (m_stateTime > 3.0f && m_hasBeamed == false) {
			m_hasBeamed = true;
			m_enemies[0]->m_beam.SetAlive(1.5f, 20.0f);
			m_enemies[0]->m_beam.m_angle = -PI_HALF;
		}
		if (m_stateTime > 5.0f) {
			m_state = S_Swarm;
			for (int i = 0; i < g_ballCount; i++) {
				m_swarmPositions[i].x = rand() % (g_width - 100) + 50;
				m_swarmPositions[i].y = rand() % (g_height - 50) * 0.5f + 25;
			}
		}
	}

	if (m_state == S_Swarm) {
		float swarmTime = m_stateTime - 5.0f;


		if (swarmTime < 1.0f) {
			SetCenterBallPosition();
			DoBall();
			
			for (int i = 0; i < g_ballCount; i++) {
				
				if (m_enemies[i]->IsAlive()) {
					math::Vector2f goalPos(m_swarmPositions[i]);

					const float angle(m_time + (float)i * PI_DOUBLE / (g_ballCount - 1));
					goalPos.x += cos(angle * 8.0f) * 40;
					goalPos.y += sin(angle * 8.0f) * 20;

					m_enemies[i]->m_pos += (goalPos - m_enemies[i]->m_pos) * swarmTime;
				}
			}

		} else if (swarmTime < 4.0f) {

			for (int i = 0; i < g_ballCount; i++) {
				if (m_enemies[i]->IsAlive()) {
					m_enemies[i]->m_pos = m_swarmPositions[i];

					const float angle(m_time + (float)i * PI_DOUBLE / (g_ballCount - 1));
					m_enemies[i]->m_pos.x += cos(angle * 8.0f) * 40;
					m_enemies[i]->m_pos.y += sin(angle * 8.0f) * 20;

					m_enemies[i]->m_angle = atan2(a_playerPos.x - m_enemies[i]->m_pos.x, a_playerPos.y - m_enemies[i]->m_pos.y);
				}
			}
		} else {
			swarmTime -= 4.0f;
			swarmTime = 1.0f - swarmTime;
			if (swarmTime < 0.0f) {
				swarmTime = 0.0f;
			}
			SetCenterBallPosition();
			DoBall();
			
			for (int i = 0; i < g_ballCount; i++) {
				if (m_enemies[i]->IsAlive()) {
					math::Vector2f goalPos(m_swarmPositions[i]);

					const float angle(m_time + (float)i * PI_DOUBLE / (g_ballCount - 1));
					goalPos.x += cos(angle * 8.0f) * 40;
					goalPos.y += sin(angle * 8.0f) * 20;

					m_enemies[i]->m_pos += (goalPos - m_enemies[i]->m_pos) * swarmTime;
				}
			}

		}
		if (m_stateTime > 10.0f) {
			m_state = S_Ball;
			m_hasBeamed = false;
			m_stateTime = 1.0f;
		}
	}

	bool hasDefences(false);
	for (int i = 1; i < g_ballCount; i++) {
		m_enemies[i]->Update(a_dt, a_difficulty);
		m_enemies[i]->m_scale = 1.0f;
		if (m_enemies[i]->IsAlive() != true) {
			m_enemies[i] = &m_phony;
		} else {
			hasDefences = true;
			m_enemies[i]->m_hasShield = m_state == S_Ball;
		}
	}

	if (hasDefences) {
		m_enemies[0]->m_life = Enemy::g_params[Enemy::T_BallBoss].m_health;
	}

	m_enemies[0]->Update(a_dt, a_difficulty);
	if (m_enemies[0]->IsAlive() != true) {
		m_enemies[0] = &m_phony;
	} else {
		m_enemies[0]->m_scale = 1.0f;
		m_enemies[0]->m_hasShield = m_state != S_Ball;
	}


}

void BallsOfFury::DoInitiate() {
	m_hasBeamed = false;
	m_state = BallsOfFury::S_Ball;
	m_stateTime = 0.0f;
	m_time = 0.0f;
	m_enemiesAdded = 0;
	for (int eIx = 0; eIx < g_ballCount; eIx++) {
		m_enemies[eIx] = &m_phony;
	}
	m_maxLife = 0; 
}

bool BallsOfFury::AddEnemy(Enemy *a_pEnemy) {

	m_enemies[m_enemiesAdded] = a_pEnemy;

	if (m_enemiesAdded == 0) {
		m_enemies[m_enemiesAdded]->SetAlive(Enemy::T_BallBoss, 0.0f, 1.0f, -1, m_enemiesAdded, true);
		m_enemies[m_enemiesAdded]->m_life * 10;
	} else {
		m_enemies[m_enemiesAdded]->SetAlive(Enemy::T_Ball, 0.0f, 1.0f, -1, m_enemiesAdded, m_enemiesAdded % 4 == 0);
		m_enemies[m_enemiesAdded]->m_life * 7;
	}

	m_maxLife += a_pEnemy->m_life;

	m_enemiesAdded++;

	if (m_enemiesAdded == g_ballCount) { 
		//end adding
		return false;
	}
	return true;
}


void BallsOfFury::DoBall() {
	for (int i = 1; i < g_ballCount; i++) {
		if (m_enemies[i]->IsAlive()) {
			const float angle(m_time + (float)i * PI_DOUBLE / (g_ballCount - 1));
			m_enemies[i]->m_pos.x = cos(angle) * 65 + m_enemies[0]->m_pos.x;
			m_enemies[i]->m_pos.y = sin(angle) * 65 + m_enemies[0]->m_pos.y;

			m_enemies[i]->m_angle = atan2(m_enemies[0]->m_pos.x - m_enemies[i]->m_pos.x, m_enemies[0]->m_pos.y - m_enemies[i]->m_pos.y);

		}
	}
}

void BallsOfFury::SetCenterBallPosition() {
	m_enemies[0]->m_pos.x = cos(m_time) * (g_width - 100) * 0.5f + g_width * 0.5f;
	m_enemies[0]->m_pos.y = sin(m_time * PI_DOUBLE) * 100.0f + 150.0f;
	m_enemies[0]->m_angle = PI;
}

}