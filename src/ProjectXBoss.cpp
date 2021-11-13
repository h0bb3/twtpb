// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "ProjectXBoss.h"
#include "Gun.h"

namespace model {
ProjectXBoss::ProjectXBoss(void) : BossBase(60.0f * 4.0f) {
	Initiate();
}

ProjectXBoss::~ProjectXBoss(void) {
}

bool ProjectXBoss::IsAlive() {
	for (int eIx = 0; eIx < g_numEnemies; eIx++) {
		if (m_enemies[eIx] != NULL && m_enemies[eIx]->IsAlive()) {
			return true;
		}
	}
	return false;
}
void ProjectXBoss::DoUpdate(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty) {
	m_time += a_dt;
	if (m_enemies[0] != NULL) {
		m_enemies[0]->Update(a_dt, a_difficulty);

		
		const float angle(cosf(m_enemies[0]->m_t * 0.5f) * 10.0f + PI);
		const float radius(cosf(m_enemies[0]->m_t  * 40.0f) * 100.0f + 400.0f - fabsf(sinf(angle)) * 100.0f) ;

		const float baseScale(1.0f + sinf(m_enemies[0]->m_t * 25.0f) * sinf(m_enemies[0]->m_t * 10.0f) * 0.5f);
		m_enemies[0]->m_scale = baseScale;
		const math::Vector2f basePos(cosf(angle) * radius + g_width / 2, sinf(angle) * radius + g_height / 2);
		m_enemies[0]->m_oldPos = m_enemies[0]->m_pos;
		m_enemies[0]->m_pos = basePos;

		bool liveTentacle(false);
		for (int tIx = 0; tIx < g_numTentacles; tIx++) {
			for (int eIx = 0; eIx < g_numTentacleEnemies; eIx++) {
				Enemy *pEnemy(m_enemies[g_numBodyEnemies + tIx * g_numTentacleEnemies + eIx]);
				if (pEnemy != NULL && pEnemy->IsAlive()) {
					liveTentacle = true;
					
					const float angle(tIx * PI_DOUBLE / g_numTentacles + m_time * 0.5f + sinf(m_time * 1.5f) * eIx * 0.25f);
					pEnemy->m_pos.x = cos(angle) * ((eIx + 1) * 10 + 25);
					pEnemy->m_pos.y = sin(angle) * ((eIx + 1) * 10 + 25);

					pEnemy->m_pos *= baseScale;
					pEnemy->m_pos += basePos;

					pEnemy->m_scale = 0.2f + eIx * 0.05;
				}
			}
		}

		// shoot from the tentacles
		if (liveTentacle) {
			for (int tIx = 0; tIx < g_numTentacleEnemies; tIx++) {
				Enemy *pPrevEnemy(m_enemies[0]);
				int enemyCount(0);
				for (int eIx(0); eIx < g_numTentacleEnemies; eIx++) {
					Enemy *pEnemy(m_enemies[g_numBodyEnemies + tIx * g_numTentacleEnemies + eIx]);
					if (pEnemy != NULL && pEnemy->IsAlive()) {

						pEnemy->m_angle = atan2f(pEnemy->m_pos.x - pPrevEnemy->m_pos.x, pEnemy->m_pos.y - pPrevEnemy->m_pos.y);
						pPrevEnemy = pEnemy;
						enemyCount++;
						//pEnemy->Update(a_dt);
					}
				}

				if (pPrevEnemy != NULL && pPrevEnemy != m_enemies[0]) {
					pPrevEnemy->Update(a_dt * (g_numTentacleEnemies - enemyCount), a_difficulty);
				}
			}
		}

		// keep the body alive while there are living tentacles
		if (liveTentacle && m_enemies[0]->m_life < Enemy::g_params[m_enemies[0]->m_type].m_health) {
			m_enemies[0]->m_life = Enemy::g_params[m_enemies[0]->m_type].m_health;
		} else if (liveTentacle != true) {
			m_enemies[0]->Update(a_dt, a_difficulty);
		}

	} else {
		m_enemies[0] = NULL;
	}
}

float ProjectXBoss::GetLifeLeft() {
	if (IsAlive()) {
		int life(0);
		for (int eIx = 0; eIx < g_numEnemies; eIx++) {
			life += m_enemies[eIx]->m_life;
		}

		return (float)life / (float)m_maxLife;
	}
	return 0.0f;
}

void ProjectXBoss::DoInitiate() {
	m_addedEnemies = 0;
	for (int eIx = 0; eIx < g_numEnemies; eIx++) {
		m_enemies[eIx] = NULL;
	}
	m_time = 0.0f;
	m_maxLife = 0;
}

bool ProjectXBoss::AddEnemy(Enemy *a_pEnemy) {
	if (m_addedEnemies < g_numEnemies) {
		m_enemies[m_addedEnemies] = a_pEnemy;
		a_pEnemy->SetAlive(model::Enemy::T_Shock, 0.0f, 2.0f, -1, 0, m_addedEnemies % 3 == 0);

		if (m_addedEnemies > g_numBodyEnemies) {
			a_pEnemy->m_life = 6;
			delete a_pEnemy->m_pGun;
			a_pEnemy->m_pGun = new DirGun();
		} else {
			delete a_pEnemy->m_pGun;
			a_pEnemy->m_pGun = new InvShockWaveGun();
		}
		a_pEnemy->m_pGun->Reset();

		m_maxLife += a_pEnemy->m_life;

		m_addedEnemies++;
		
		return true;
	}
	return false;
}

}