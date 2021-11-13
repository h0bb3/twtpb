// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "SnakeBoss.h"
#include "Enemy.h"

namespace model {

SnakeBoss::SnakeBoss(void) : BossBase(60.0f * 8.0f) {
	Initiate();
}

SnakeBoss::~SnakeBoss(void) {
}

bool SnakeBoss::IsAlive() {
	for (int eIx = 0; eIx < g_numEnemies; eIx++) {
		if (m_enemies[eIx] != NULL && m_enemies[eIx]->IsAlive()) {
			return true;
		}
	}
	return false;
}

void SnakeBoss::DoUpdate(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty) {

	std::vector<int> heads;
	m_time += a_dt;

	bool doHead(true);
	float scale(0.5f);
	int segmentCount;

	// get the head indices for easy access
	for (int eIx = 0; eIx < g_numEnemies; eIx++) {
		if (m_enemies[eIx] != NULL) {
			Enemy &e(*m_enemies[eIx]);
			if (e.IsAlive()) {
				if (doHead) {
					heads.push_back(eIx);
					doHead = false;
				}
			} else {
				doHead = true;
			}
		}
	}

	doHead = true;
	for (int eIx = 0; eIx < g_numEnemies; eIx++) {
		if (m_enemies[eIx] != NULL) {
			Enemy &e(*m_enemies[eIx]);
			if (e.IsAlive()) {
				e.m_hasShield = false;
				if (doHead) {
					//e.m_pos.Set(g_width * 0.5f, g_height * 0.5f);

					e.m_doDropEnergy = false;
					scale = 0.0f;
					segmentCount = 0;
					for (int eIx2 = eIx; eIx2 < g_numEnemies; eIx2++) {
						if (m_enemies[eIx2] != NULL && m_enemies[eIx2]->IsAlive()) {
							scale += 0.01f;
							segmentCount++;
							if (eIx2 > eIx) {
								e.m_hasShield = true;
							}
						} else {
							break;
						}
					}

					if (scale < 0.2) {
						scale = 0.2f;
					}

					e.m_scale = scale;
					if (e.m_life > GetDamageForSnakeSegment(segmentCount)) {
						e.m_life = GetDamageForSnakeSegment(segmentCount);
					}
					

					const math::Vector2f goal(a_playerPos);
					const float speedscale(1.0f);


					for (int i = 0; i < 10; i++) {
						math::Vector2f force(0.0f, 0.0f);
						math::Vector2f dir(goal - e.m_pos);
						force = dir;
						force.Normalize();
						force *= 75.0f;

						// flocking force
						for (int hIx = 0; hIx < heads.size(); hIx++) {
							if (heads[hIx] != eIx) {
								math::Vector2f dir(e.m_pos - m_enemies[heads[hIx]]->m_pos);
								if (dir.Length2() < 81.0f * 81.0f) {
									const float length(dir.Length());
									dir /= length;
									
									dir *=  (81.0f) / length;
									
									force += dir * 32.0f;
								}
								/*if (dir.Length2() > 95.0f * 95.0f) {
									const float length(dir.Length());
									dir /= length;
									
									dir *=  (95.0f) / length;
									
									force -= dir * 375.0f;
								}*/
							}
						}

						m_speeds[eIx] += force / (7.0f * scale) * a_dt;
						if (m_speeds[eIx].Length2() > 100.0f * 100.0f * speedscale * speedscale) {
							m_speeds[eIx].Normalize();
							m_speeds[eIx] *= 100.0f * speedscale;
						}
						e.m_pos += m_speeds[eIx] * a_dt * 0.5f;
					}

					doHead = false;

				} else {
					// set pos to previous enemy position...
					math::Vector2f dir(e.m_pos - m_enemies[eIx - 1]->m_pos);
					dir.Normalize();
					e.m_pos = m_enemies[eIx - 1]->m_pos + dir * 13.0f;
					
					if (e.m_life > GetDamageForSnakeSegment(segmentCount)) {
						e.m_life = GetDamageForSnakeSegment(segmentCount);
					}
					if (segmentCount > 3) {
						e.m_doDropEnergy = true;
					} else {
						e.m_doDropEnergy = false;
					}
					e.m_scale = scale * ((float)(e.m_life) / (float)GetDamageForSnakeSegment(segmentCount));
					if (e.m_scale < 0.1f) {
						e.m_scale = 0.1f;
					}
				}
			} else {
				doHead = true;
			}
		}
	}
}

void SnakeBoss::DoInitiate() {
	for (int eIx = 0; eIx < g_numEnemies; eIx++) {
		m_enemies[eIx] = NULL;
		m_speeds[eIx].Set(0.0f, 0.0f);
	}
	m_addedEnemies = 0;
	m_time = 0.0f;
}

bool SnakeBoss::AddEnemy(Enemy *a_pEnemy) {
	if (m_addedEnemies < g_numEnemies) {
		m_enemies[m_addedEnemies] = a_pEnemy;

		a_pEnemy->SetAlive(Enemy::T_Shock2, 0.0f, 0.5f, -1, m_addedEnemies, true);
		a_pEnemy->m_life = g_numEnemies * 1000;	// this is recalculated in update but needs to be really big

		m_addedEnemies++;
		return true;
	}

	for (int i = 0; i < 1000; i++) {
		DoUpdate(0.1f, math::Vector2f(g_width * 0.5f, g_height * 0.5f), false, 1.0f);
	}

	return false;
}

float SnakeBoss::GetLifeLeft() {
	int life(0);
	for (int eIx = 0; eIx < g_numEnemies; eIx++) {
		if (m_enemies[eIx] != NULL && m_enemies[eIx]->IsAlive()) {
			life += m_enemies[eIx]->m_life;
		}
	}

	return (float)life / (float)(g_numEnemies * GetDamageForSnakeSegment(g_numEnemies));
}

int SnakeBoss::GetDamageForSnakeSegment(int a_numSnakeSegments) const {


	return a_numSnakeSegments / 3 + 1;
}

}
