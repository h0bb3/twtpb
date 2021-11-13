// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "DanMakuBoss.h"
#include "Enemy.h"
#include "IEventTarget.h"

namespace model {

DanMakuBoss::DanMakuBoss(void) : BossBase(60.0f  * 7.0f) {
	for (int eIx = 0; eIx < g_numEnemies; eIx++) {
		m_enemies[eIx] = NULL;
	}
}

DanMakuBoss::~DanMakuBoss(void) {
}

float DanMakuBoss::GetLifeLeft() {
	if (IsAlive()) {
		return (float)m_enemies[0]->m_life / (float)g_life;
	}
	return 0.0f;
}

bool DanMakuBoss::IsAlive() {
	if (m_enemies[0] != NULL && m_enemies[0]->IsAlive() != true) {
		int eIx = 0;
		for (; eIx < g_numBodyEnemies + g_numGunEnemies; eIx++) {
			m_enemies[eIx]->Kill(true);
			m_enemies[eIx] = NULL;
		}
		for (; eIx < g_numEnemies; eIx++) {
			m_enemies[eIx]->Kill(false);
			m_enemies[eIx] = NULL;
		}

		return false;
	}

	return m_enemies[0] != NULL;
}

void DanMakuBoss::DoUpdate(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty) {

	m_time += a_dt;

	// body
	if (m_enemies[0] != NULL) {
		if (m_enemies[0]->IsAlive()) {
			m_enemies[0]->m_pos.Set(g_width / 2, 200.0f);
			m_enemies[0]->Update(a_dt, a_difficulty);
		}
	}

	const float waveTimer(m_time - m_waveTime);
	if (waveTimer > 10.0f) {
		for (int eIx = g_numBodyEnemies + g_numGunEnemies; eIx < g_numEnemies; eIx++) {
			if (m_enemies[eIx]->IsAlive() != true) {
				m_enemies[eIx]->m_life = 5;
			}
		}
		m_waveTime = m_time;

		m_currentWave = (Wave)((m_currentWave + 1) % (int)W_Count);
		if (m_currentWave == W_DoubleSphere || m_currentWave == W_Lines) {
			m_gunPauseTime = m_time;
		}
	}

	// guns
	switch (m_currentWave) {
		case W_DoubleSphere:
		case W_LinesCCW:
			DoGuns(g_numGunEnemies / 2, a_dt, m_time * 0.5f, 0.25, 0.0f, 25.0f, a_difficulty);
		break;
		case W_Lines:
			DoGuns(g_numGunEnemies / 2 + 1, a_dt, m_time * 0.5f, 0.25, 0.0f, 25.0f, a_difficulty);
		break;
		default:
			DoGuns(g_numGunEnemies, a_dt, m_time * 3.0f, 0.25, 0.2f, 8.0f, a_difficulty);
		break;
	}

	float ccw = 1.0f;
	int eIx(g_bulletEnemyStartIx);
	switch (m_currentWave) {
		case W_DoubleSphere:
			for (; eIx < g_bulletEnemyStartIx + 16; eIx++) {
				const float angleOffset((eIx - g_numBodyEnemies - g_numGunEnemies) * PI_DOUBLE / 16);

				m_enemies[eIx]->m_pos.x = cosf(angleOffset) * waveTimer * 70.0f;
				m_enemies[eIx]->m_pos.y = sinf(angleOffset) * waveTimer * 70.0f;
				m_enemies[eIx]->m_pos += m_enemies[0]->m_pos;

			}

			if (waveTimer - 1.0f > 0.0f) {
				for (; eIx < g_bulletEnemyStartIx + 16 + 16; eIx++) {
					const float angleOffset((eIx - g_numBodyEnemies - g_numGunEnemies) * PI_DOUBLE / 16);

					m_enemies[eIx]->m_pos.x = cosf(angleOffset) * (waveTimer - 1.0f) * 80.0f;
					m_enemies[eIx]->m_pos.y = sinf(angleOffset) * (waveTimer - 1.0f) * 80.0f;
					m_enemies[eIx]->m_pos += m_enemies[0]->m_pos;
				}
			}
		break;

		case W_LinesCCW:
			ccw = -1;
		case W_Lines:
			for (int lIx = 0; lIx < 8; lIx++) {
				const float angleOffset((lIx) * PI_DOUBLE / 8);
				for (int i = 0; i < 8; eIx++, i++) {
					const float time(waveTimer - (float)(eIx - g_bulletEnemyStartIx) * 0.1f);
					if (time > 0.0f) {
						m_enemies[eIx]->m_pos.x = cosf((angleOffset + time + m_waveTime) * ccw) * time * 150.0f;
						m_enemies[eIx]->m_pos.y = sinf((angleOffset + time + m_waveTime) * ccw) * time * 150.0f;
						m_enemies[eIx]->m_pos += m_enemies[0]->m_pos;
					}
				}
			}
		break;

		case W_Sphere:
		default:
			for (; eIx < g_bulletEnemyStartIx + 16; eIx++) {
				const float angleOffset((eIx - g_numBodyEnemies - g_numGunEnemies) * PI_DOUBLE / 16);

				m_enemies[eIx]->m_pos.x = cosf(angleOffset) * waveTimer * 60.0f;
				m_enemies[eIx]->m_pos.y = sinf(angleOffset) * waveTimer * 60.0f;
				m_enemies[eIx]->m_pos += m_enemies[0]->m_pos;
			}
		break;
	}

	// remove unused bullet enemies
	for (; eIx < g_numEnemies; eIx++) {
		m_enemies[eIx]->m_pos.Set(-500, -500);
	}
}

void DanMakuBoss::DoGuns(int a_gunCount, float a_dt, float a_rotFactor, float a_rotSpeed, float a_pause, float a_rateOfFire, const float a_difficulty) {
	int eIx = g_numBodyEnemies;
	for (; eIx < g_numBodyEnemies + a_gunCount; eIx++) {
		if (m_enemies[eIx] != NULL) {
			if (m_enemies[eIx]->IsAlive() != true) {
				m_enemies[eIx]->m_life = 10;
			}

			m_angle += a_dt * cos(a_rotFactor) * a_rotSpeed;
			const float anlgeOffset((eIx - g_numBodyEnemies) * PI_DOUBLE / (float)a_gunCount);
	
			m_enemies[eIx]->m_pos.x = cos(m_angle + anlgeOffset) * 10.0f;
			m_enemies[eIx]->m_pos.y = sin(m_angle + anlgeOffset) * 10.0f;

			m_enemies[eIx]->m_angle = -(m_angle + anlgeOffset + PI_HALF + PI);
			m_enemies[eIx]->m_pos += m_enemies[0]->m_pos;

			if (m_time - m_gunPauseTime > 1.0f) {
				if (cos(m_time) < -a_pause || cos(m_time) > a_pause) {
					m_enemies[eIx]->Update(a_dt * a_rateOfFire, a_difficulty);
				}
			}
		}
	}

	for (; eIx < g_numBodyEnemies + g_numGunEnemies; eIx++) {
		m_enemies[eIx]->m_life = -1;
	}
}

void DanMakuBoss::DoInitiate() {
	for (int eIx = 0; eIx < g_numEnemies; eIx++) {
		m_enemies[eIx] = NULL;
	}
	m_addedEnemies = 0;
	m_time = 0.0f;
	m_gunPauseTime = 0.0f;
	m_angle = 0.0f;
	m_waveTime = 0.0f;

	m_currentWave = W_Sphere;
}

bool DanMakuBoss::AddEnemy(Enemy *a_pEnemy) {

	m_enemies[m_addedEnemies] = a_pEnemy;
	if (m_addedEnemies < g_numBodyEnemies) {
		a_pEnemy->SetAlive(Enemy::T_Shock, 0.0, 1.5f, -1, 0, false);
		delete a_pEnemy->m_pGun;
		a_pEnemy->m_pGun = new NullGun();
		a_pEnemy->m_life = g_life;
	} else if (m_addedEnemies < g_numBodyEnemies + g_numGunEnemies) {
		a_pEnemy->SetAlive(Enemy::T_Shock, 0.0, 0.25f, -1, 0, false);
		delete a_pEnemy->m_pGun;
		a_pEnemy->m_pGun = new DirGun();
	} else {
		a_pEnemy->SetAlive(Enemy::T_Shock, 0.0, 0.25f, -1, 0, m_addedEnemies % 4 == 0);
		a_pEnemy->m_life = 5;
		a_pEnemy->m_life = 0;
		delete a_pEnemy->m_pGun;
		a_pEnemy->m_pGun = new NullGun();
	}

	m_addedEnemies++;
	return m_addedEnemies < g_numEnemies;
}

}