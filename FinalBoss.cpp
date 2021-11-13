// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "FinalBoss.h"
#include "Enemy.h"

namespace model {

FinalBoss::FinalBoss(void) : BossBase(30.0f) {

}

FinalBoss::~FinalBoss(void) {

}

bool FinalBoss::IsAlive() {
	return m_enemies[0] != NULL;
}

float FinalBoss::GetLifeLeft() {
	return 1.0f;
}

void FinalBoss::DoInitiate() {
	m_enemies[0] = NULL;
	m_time = 0.0f;
}

void FinalBoss::DoUpdate(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty) {

	m_time += a_dt;

	m_enemies[0]->m_pos = a_playerPos;
	m_enemies[0]->m_pos.y -= 50.0f + sinf(m_time * 1.2f) * 10.0f;
	m_enemies[0]->m_hasShield = m_isAgressive;
}

bool FinalBoss::AddEnemy(Enemy *a_pEnemy) {
	if (m_enemies[0] == NULL) {
		m_enemies[0] = a_pEnemy;
		a_pEnemy->SetAlive(Enemy::T_Shock, 0.0f, 0.3f, -1, 0, false);
		a_pEnemy->m_life = 100;
		return true;
	} 
	return false;
}

}