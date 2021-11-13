// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "ShockWaveBoss.h"
#include "ieventtarget.h"

namespace model {

ShockWaveBoss::ShockWaveBoss(void) : BossBase(60.0f * 3.0f) {
	m_phony.m_life = 0;
	Initiate();
}

ShockWaveBoss::~ShockWaveBoss(void) {
}

bool ShockWaveBoss::IsAlive() {
	for (int i = 0; i < g_numEnemies; i++) {
		if (m_enemies[i]->IsAlive() == true) {
			return true;
		} else {
			//m_enemies[i] = &m_phony;
		}
	}

	return false;
}



void ShockWaveBoss::DoInitiate() {
	m_enemiesAdded = 0;
	m_time = 0.0f;
	for (int eIx = 0; eIx < g_numEnemies; eIx++) {
		m_enemies[eIx] = &m_phony;
	}
	m_maxLife = 0;
}

float ShockWaveBoss::GetLifeLeft() {
	if (IsAlive()) {
		int life(0);
		for (int eIx = 0; eIx < g_numEnemies; eIx++) {
			life += m_enemies[eIx]->m_life;
		}

		return (float)life / (float)m_maxLife;
	}
	return 0.0f;
}

void ShockWaveBoss::DoUpdate(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty) {
	math::Vector2 center;

	center.x = g_width / 2.0f;
	if (m_time > 1.0f) {
		center.y = g_height / 2.0f;
	} else {
		center.y = -250 + m_time * (250 + g_height / 2.0f);
	}

	for (int i = 0; i < g_numEnemies; i++) {
		m_enemies[i]->m_scale = 1.0f;
		if (m_enemies[i]->IsAlive() == false) {
			m_enemies[i] = &m_phony;
		}
	}

	m_time += a_dt;
	for (int i = 0; i < g_numEnemies; i++) {
		if (m_enemies[i]->IsAlive()) {
			const float offset((float)i * PI_DOUBLE / ((float)g_numEnemies));
			const float angle(sinf(m_time) * 3.14f + offset);
			m_enemies[i]->m_pos.x = cos(m_time * 0.7f + offset) * 50 + cos(m_time * 0.7f + offset) * (sinf(m_time * 0.75f + offset * 3) * 100.0f + 75) + center.x;
			m_enemies[i]->m_pos.y = sin(m_time * 0.7f + offset) * 50 + sin(m_time * 0.7f + offset) * (sinf(m_time * 0.75f + offset * 3) * 100.0f + 75) + center.y;
			m_enemies[i]->m_angle = angle;
			m_enemies[i]->Update(a_dt, a_difficulty);			
		}
	}
}



bool ShockWaveBoss::AddEnemy(Enemy *a_pEnemy) {

	m_enemies[m_enemiesAdded] = a_pEnemy;

	
	m_enemies[m_enemiesAdded]->SetAlive(Enemy::T_Shock, 0.0f, 1.0f, -1, m_enemiesAdded, m_enemiesAdded % 4 == 0);
	delete m_enemies[m_enemiesAdded]->m_pGun;
	m_enemies[m_enemiesAdded]->m_pGun = new ShockWaveGun();

	
	m_enemies[m_enemiesAdded]->m_scale = 0.75f;
	m_enemies[m_enemiesAdded]->m_pos.x = (float)m_enemiesAdded * ((float)g_width / (float)g_numEnemies);
	m_enemies[m_enemiesAdded]->m_pos.y = -100;
	m_swarmPositions[m_enemiesAdded] = m_enemies[m_enemiesAdded]->m_pos;
	
	m_maxLife += a_pEnemy->m_life;

	m_enemiesAdded++;

	if (m_enemiesAdded == g_numEnemies) { 
		//end adding
		return false;
	}
	return true;
}


}