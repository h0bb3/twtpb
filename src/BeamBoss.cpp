// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "BeamBoss.h"

#include "Enemy.h"

namespace model {

BeamBoss::BeamBoss(void) : BossBase(60.0f * 5.0f) {
	Initiate();
}

BeamBoss::~BeamBoss(void) {
}

bool BeamBoss::IsAlive() {
	for (int eIx = 0; eIx < g_numEnemies; eIx++) {
		if (m_enemies[eIx] != NULL && m_enemies[eIx]->IsAlive()) {
			return true;
		}
	}
	return false;
}

void BeamBoss::DoUpdate(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty) {
	const float angleOffsets[g_numEnemies] = {0, 0, 0, 0, 0, 0, PI_HALF, PI_HALF, PI, PI, PI + PI_HALF, PI + PI_HALF, 0};
	const float size(85.0f);
	const float sizeSide(75.0f);
	const float hsize(25.0f);

	const math::Vector2f positions[g_numEnemies] = {
		math::Vector2f(-size, size), math::Vector2f(size, size),
		math::Vector2f(-size, -size), math::Vector2f(size, -size),

		math::Vector2f(-hsize, sizeSide), math::Vector2f(hsize, sizeSide),
		math::Vector2f(sizeSide, -hsize), math::Vector2f(sizeSide, hsize),
		math::Vector2f(-hsize, -sizeSide), math::Vector2f(hsize, -sizeSide),
		math::Vector2f(-sizeSide, -hsize), math::Vector2f(-sizeSide, hsize),

		math::Vector2f(0.0f, 0.0f)
	};

	m_angle += a_dt * 0.5f;
	m_beamTimer -= a_dt;

	const float cosa(cosf(m_angle));
	const float sina(sinf(m_angle));
	const math::Vector2f pos(g_width * 0.5f + cosf(-m_angle) * 100.0f, g_height * 0.25f + sinf(-m_angle) * 50.0f + 40.0f);
	bool didBeam(false);

	for (int eIx = 0; eIx < g_numEnemies; eIx++) {
		Enemy &e(*m_enemies[eIx]);


		e.m_pos.Rotate(positions[eIx], cosa, sina);
		e.m_pos += pos;
		e.m_angle = angleOffsets[eIx] - m_angle + PI_HALF;

		if (eIx < g_numCornerEnemies) {
			e.m_beam.m_angle = atan2f(pos.y - e.m_pos.y, pos.x - e.m_pos.x);// + PI_HALF;
			if (e.m_beam.IsAlive() != true && m_beamTimer < 0.0f) {
				e.m_beam.SetAlive(3.0f, 5.0f);
				didBeam = true;
			}
		}
		e.Update(a_dt, a_difficulty);
	}

	if (didBeam) {
		m_beamTimer = 5.0f;
	}
}

void BeamBoss::DoInitiate() {
	for (int eIx = 0; eIx < g_numEnemies; eIx++) {
		m_enemies[eIx] = NULL;
	}
	m_addedEnemies = 0;
	m_angle = 0.0f;
	m_beamTimer = 3.0f;
}

bool BeamBoss::AddEnemy(Enemy *a_pEnemy) {
	if (m_addedEnemies < g_numEnemies) {

		if (m_addedEnemies < g_numCornerEnemies) {
			a_pEnemy->SetAlive(Enemy::T_Shock, 0.0, 1.0f, -1, 0, true);
			a_pEnemy->m_life = g_maxCornerDamage;
			delete a_pEnemy->m_pGun;
			a_pEnemy->m_pGun = new NullGun();
		} else if (m_addedEnemies < g_numCornerEnemies + g_numSidesEnemies) {
			a_pEnemy->SetAlive(Enemy::T_Pill, 0.0, 1.5f, -1, 0, true);
			a_pEnemy->m_life = g_maxSideDamage;
		} else if (m_addedEnemies < g_numCornerEnemies + g_numSidesEnemies + g_numCenterEnemies) {
			a_pEnemy->SetAlive(Enemy::T_Shock, 0.0, 2.0f, -1, 0, true);
			a_pEnemy->m_life = g_maxCornerDamage;
			delete a_pEnemy->m_pGun;
			a_pEnemy->m_pGun = new CloudGun();//new NullGun();
		}

		m_enemies[m_addedEnemies] = a_pEnemy;	
		m_addedEnemies++;
		return true;
	}
	return false;
}

float BeamBoss::GetLifeLeft() {
	int damage(0);
	for (int eIx = 0; eIx < g_numEnemies; eIx++) {
		if (m_enemies[eIx]->m_life > 0) {
			damage += m_enemies[eIx]->m_life;
		}
	}

	return (float)damage / (float)g_maxDamage;
}

}
