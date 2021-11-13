// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "YellyBoss.h"

#include "Enemy.h"

namespace model {

const YellyBoss::EnemySettings YellyBoss::g_settings[g_numBodyEnemies] = {	EnemySettings(math::Vector2f(0, -8), 1.4f),
														EnemySettings(math::Vector2f(-25, 6.5), 0.8f),
														EnemySettings(math::Vector2f(25, 6.5), 0.8f),
														EnemySettings(math::Vector2f(-40, 15), 0.5f),
														EnemySettings(math::Vector2f(40, 15), 0.5f),
														EnemySettings(math::Vector2f(-50, 20), 0.3f),
														EnemySettings(math::Vector2f(50, 20), 0.3f),
														EnemySettings(math::Vector2f(-54, 26), 0.2f),
														EnemySettings(math::Vector2f(54, 26), 0.2f)};

YellyBoss::YellyBoss() : BossBase(60.0f * 4.0f) {
	Initiate();
}

YellyBoss::~YellyBoss(void) {
}

float YellyBoss::GetLifeLeft() {
	if (IsAlive()) {
		int life(m_damage);
		for (int eIx = g_numBodyEnemies; eIx < g_numEnemies; eIx++) {
			if (m_enemies[eIx] != NULL) {
				if (m_enemies[eIx]->m_life > 0) {
					life += m_enemies[eIx]->m_life;
				}
			}
		}

		return (float)life / (float)(GetMaxDamage() + m_maxDamage);
	}
	return 0.0f;
}

bool YellyBoss::IsAlive() {
	if (m_addedEnemies <= 0) {
		return false;
	}
	if (m_damage > 0) {
		return true;
	}

	for (int eIx = 0; eIx < g_numEnemies; eIx++) {
		if (m_enemies[eIx] != NULL && m_enemies[eIx]->IsAlive() == true) {
			return true;
		}
	}
	return false;
}

void YellyBoss::DoUpdate(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty) {
	const State state(GetState());
	static float deathTimer = 0.0f;
	deathTimer -= a_dt;

	const float maxScale(0.9);
	const float scale((m_t * 0.3f > 1.0f ? 1.0f : m_t * 0.3f) * maxScale);
	bool hasLiveTentacle(false);
	m_t += a_dt;

	if (state == S_Dying) {
		if (deathTimer < 0) {
			deathTimer = 0.1f;
			int killedEnemies(0);
			for (int eIx = 0; eIx < g_numBodyEnemies && killedEnemies < 3; eIx++) {
				if (m_enemies[eIx] != NULL) {
					m_enemies[eIx]->Kill(true);
					m_enemies[eIx] = NULL;
					killedEnemies++;
				}
			}
		}
		return;
	} else {
		const float yScale(fabsf(cosf(m_t * 0.5f) * 0.5f) + 1.5f);
		const float xScale(fabsf(cosf(m_t * 1.5f) * 0.5f) + 1.5f);

		math::Vector2f pos;
		pos.y = g_height * 0.5f * (1.0f - m_radius) + ((cosf(m_t * 0.1f + PI) * 150.0f + 250) + cos(m_t * 3) * 100.0f) * m_radius;
		pos.x = g_width * 0.5f - (cos(m_t * 0.5f) * (500 - pos.y) * sin(m_t * 0.3)) * m_radius;

		for (int i = 0; i < g_numBodyEnemies; i++) {
			Enemy *pE(m_enemies[i]);
			if (pE != NULL) {

				pE->m_pos = pos;

				pE->m_pos.y += g_settings[i].m_pos.y * yScale * scale;
				pE->m_pos.x += g_settings[i].m_pos.x * xScale * scale;
				pE->m_scale = g_settings[i].m_scale * yScale * scale;
			}
		}

		if (state == S_Mad) {
			if (scale >= maxScale && m_enemies[0] != NULL) {
				m_enemies[0]->Update(a_dt, 1.0f);
				
			}
		} else if (state == S_Mader) {
			m_radius -= a_dt;
			if (m_radius < 0.0f) {
				m_radius = 0.0f;
			
				if (scale >= maxScale) {
					if (m_enemies[g_numBodyEnemies - 1] != NULL) {
						m_enemies[g_numBodyEnemies - 1]->Update(a_dt, 1.0f);
					}
					if (m_enemies[g_numBodyEnemies - 2] != NULL) {
						m_enemies[g_numBodyEnemies - 2]->Update(a_dt, 1.0f);
					}
				}
			}
		}

		for (int tIx = 0; tIx < g_numTentacles; tIx++) {
			int lastIx = g_numTentacleEnemies - 1;
			
			for (; lastIx >= 0; lastIx--) {
				const int ix(lastIx + g_numBodyEnemies + g_numTentacleEnemies * tIx);
				if (m_enemies[ix] != NULL && m_enemies[ix]->IsAlive()) {
					break;
				}
			}

			if (lastIx > 0) {
				int prevIx(lastIx - 1);
				for (; prevIx >= 0; prevIx--) {
					const int ix(prevIx + g_numBodyEnemies + g_numTentacleEnemies * tIx);

					if (m_enemies[ix] != NULL && m_enemies[ix]->IsAlive()) {
						break;
					}					
				}

				if (prevIx > -1) {
					Enemy &lastEnemy(*m_enemies[lastIx + g_numBodyEnemies + g_numTentacleEnemies * tIx]);
					Enemy &prevEnemy(*m_enemies[prevIx + g_numBodyEnemies + g_numTentacleEnemies * tIx]);

					lastEnemy.m_angle = atan2f(lastEnemy.m_pos.x - prevEnemy.m_pos.x, lastEnemy.m_pos.y - prevEnemy.m_pos.y);
					lastEnemy.Update(a_dt * 4.0f, a_difficulty);
					hasLiveTentacle = true;
				}
			}
		}
	}

	static float dt = 0.0f;
	dt += a_dt;
	a_dt = 0.005f;
	while (dt >= a_dt) {
		SimulateTentacles(a_dt, scale, a_playerPos);
		dt -= a_dt;
	}

	// take care of damage
	for (int eIx = 0; eIx < g_numBodyEnemies; eIx++) {
		Enemy *pE(m_enemies[eIx]);
		if (pE != NULL) {
			if (pE->m_life < 10) {
				pE->m_life = 10;
				if (hasLiveTentacle != true) {
					m_damage--;
				}
			}
		}
	}
}

void YellyBoss::SimulateTentacles(float a_dt, float a_scale, const math::Vector2f &a_playerPos) {
	static const float xOffset[g_numTentacles] = {-60, -30, 0, 30, 60};
	for (int tIx = 0; tIx < g_numTentacles; tIx++) {

		if (m_enemies[g_numBodyEnemies + tIx * g_numTentacleEnemies] == NULL) {
			continue;
		}

		math::Vector2f basePos;
		m_enemies[g_numBodyEnemies + g_numTentacleEnemies * tIx]->m_pos = m_enemies[0]->m_pos;
		m_enemies[g_numBodyEnemies + g_numTentacleEnemies * tIx]->m_pos.x += xOffset[tIx];
		m_enemies[g_numBodyEnemies + g_numTentacleEnemies * tIx]->m_pos.x += cosf(m_t * 3.0f) * xOffset[tIx] * 0.5f;
		m_enemies[g_numBodyEnemies + g_numTentacleEnemies * tIx]->m_pos.y += 50.0f;
		basePos = m_enemies[g_numBodyEnemies + g_numTentacleEnemies * tIx]->m_pos;

		for (int i = 0; i < g_numTentacleEnemies - 1;) {
			if (m_enemies[i + g_numBodyEnemies + g_numTentacleEnemies * tIx]->IsAlive() == true) {
				basePos = m_enemies[i + g_numBodyEnemies + g_numTentacleEnemies * tIx]->m_pos;
			}

			int nextIx = i + 1;
			for (; nextIx < g_numTentacleEnemies; nextIx++) {
				if (m_enemies[nextIx + g_numBodyEnemies + g_numTentacleEnemies * tIx]->IsAlive()) {
					break;
				}
			}
			if (nextIx == g_numTentacleEnemies) {
				break;
			}
			
			math::Vector2f dir((m_enemies[nextIx + g_numBodyEnemies + g_numTentacleEnemies * tIx]->m_pos - basePos));
			const float len(dir.Length());

			math::Vector2f force(0.0f, 0.0f);
			if (len != 0.0f) {
				force -= (dir / len) * (len - 10) * (7000.0f + cosf(m_t * 3.0f + (float)tIx) * 2000.0f);
			}

			dir = (a_playerPos - m_enemies[nextIx + g_numBodyEnemies + g_numTentacleEnemies * tIx]->m_pos);
			dir.Normalize();
			dir *= 5000 + 9500 + (sinf(m_t * 3.0f + (float)tIx) * 5000.0f);

			force += dir;
			
			m_speeds[nextIx + g_numTentacleEnemies * tIx] = force * a_dt;
			


			i = nextIx;
		}

		for (int i = g_numBodyEnemies; i < g_numBodyEnemies + g_numTentacleEnemies; i++) {
			Enemy &enemy(*m_enemies[i + g_numTentacleEnemies * tIx]);
			
			if (enemy.IsAlive()) {
				enemy.m_oldPos = enemy.m_pos;
				const int ix(i - g_numBodyEnemies + g_numTentacleEnemies * tIx);

				m_speeds[ix] -= m_speeds[ix] * 0.1f * a_dt;
				if (m_speeds[ix].Length2() > 800.0f * 800.0f) {
					m_speeds[ix].Normalize();
					m_speeds[ix] *= 800.0f;
				}

				enemy.m_pos += m_speeds[ix] * a_dt;
				enemy.m_pos.y += 20 * a_dt;
				enemy.m_scale = 0.2f + cosf(m_t * 3.0f + (float)tIx + i * 0.02f) * 0.1f;
				enemy.m_scale *= a_scale;
			}
		}
	}
}

int YellyBoss::GetMaxDamage() const {
	return g_numBodyEnemies * 20;
}

bool YellyBoss::HasLiveTentacle() {
	for (int i = g_numBodyEnemies; i < g_numTentacleEnemies * g_numTentacles; i++) {
		if (m_enemies[i] != NULL && m_enemies[i]->IsAlive()) {
			return true;
		}
	}
	return false;
}

void YellyBoss::DoInitiate() {
	for (int i = 0; i < g_numEnemies; i++) {
		m_enemies[i] = NULL;
	}
	for (int i = 0; i < g_numTentacleEnemies * g_numTentacles; i++) {
		m_speeds[i].Set(0.0f, 0.0f);
	}
	m_t = 0.0f;
	m_damage = GetMaxDamage();
	m_addedEnemies = 0;
	m_radius = 1.0f;
	m_maxDamage = 0;
}

YellyBoss::State YellyBoss::GetState() {
	if (m_damage <= 0) {
		return S_Dying;
	}
	if (m_damage < (GetMaxDamage()) / 3) {
		return S_Mader;
	}

	if (m_damage < ((GetMaxDamage()) / 4) * 3) {
		return S_Mad;
	}

	return S_Normal;
}

bool YellyBoss::AddEnemy(Enemy *a_pEnemy) {
	if (m_addedEnemies < g_numEnemies) {
		m_enemies[m_addedEnemies] = a_pEnemy;
		m_enemies[m_addedEnemies]->SetAlive(Enemy::T_Shock, 0.0f, 1.0f, -1, m_addedEnemies, false);
		m_enemies[m_addedEnemies]->m_life = 10;


		if (m_addedEnemies < g_numBodyEnemies) {
			delete m_enemies[m_addedEnemies]->m_pGun;
			if (m_addedEnemies == 0) {
				m_enemies[m_addedEnemies]->m_pGun = new SlowRotating4WayGun();
			} else {
				m_enemies[m_addedEnemies]->m_pGun = new Rotating4WayGun();
			}
			m_enemies[m_addedEnemies]->m_pGun->Reset();
		} else {
			delete m_enemies[m_addedEnemies]->m_pGun;
			m_enemies[m_addedEnemies]->m_pGun = new DirGun();
			m_enemies[m_addedEnemies]->m_pGun->Reset();
			m_enemies[m_addedEnemies]->m_life = 2;
			m_enemies[m_addedEnemies]->m_doDropEnergy = m_addedEnemies % 4 == 0;

			m_maxDamage += a_pEnemy->m_life;
		}

			m_addedEnemies++;

		return true;
	}

	// this is to initiate the tentacles to good values
	DoUpdate(0.1f, math::Vector2f(0.0f, 0.0f), false, 1.0f);
	for (int i = 0; i < 100; i++) {
		SimulateTentacles(0.2f, 0.0f, math::Vector2f(0.0f, 0.0f));
	}


	return false;
}

}
