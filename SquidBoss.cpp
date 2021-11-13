// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "SquidBoss.h"

#include "Enemy.h"

namespace model {

const SquidBoss::EnemySettings SquidBoss::g_settings[g_numBodyEnemies] = {	EnemySettings(math::Vector2f(0, -10), 1.6f),
														EnemySettings(math::Vector2f(0, -30), 1.3f),
														EnemySettings(math::Vector2f(0, -50), 1.0f),
														EnemySettings(math::Vector2f(0, -65), 0.7f),
														EnemySettings(math::Vector2f(0, -77), 0.4f)};

const SquidBoss::Frame SquidBoss::g_frames[SquidBoss::g_numFrames] = {
	{math::Vector2f(400,250),	1.0f, SquidGun::GT_NULL,		1.0f},
	{math::Vector2f(400,250),	4.0f, SquidGun::GT_Funky4Way,	1.0f},
	{math::Vector2f(200,100),	2.0f, SquidGun::GT_Cloud,		3.0f},
	{math::Vector2f(200,100),	4.0f, SquidGun::GT_TriShot,		7.0f},
	{math::Vector2f(200,500),	5.0f, SquidGun::GT_Cloud,		3.0f},
	{math::Vector2f(200,500),	2.0f, SquidGun::GT_Cloud,		3.0f},
	{math::Vector2f(200,100),	1.0f, SquidGun::GT_Cloud,		3.0f},
	{math::Vector2f(200,100),	4.0f, SquidGun::GT_Slow4Way,	4.0f},
	{math::Vector2f(400,500),	5.0f, SquidGun::GT_Cloud,		3.0f},
	{math::Vector2f(400,100),	1.0f, SquidGun::GT_Cloud,		3.0f},
	{math::Vector2f(400,100),	4.0f, SquidGun::GT_Slow4Way,	4.0f},
	{math::Vector2f(700,500),	5.0f, SquidGun::GT_Cloud,		3.0f},
	{math::Vector2f(700,100),	1.0f, SquidGun::GT_Cloud,		3.0f},
	{math::Vector2f(700,100),	4.0f, SquidGun::GT_Slow4Way,	4.0f},
	{math::Vector2f(700,100),	4.0f, SquidGun::GT_TriShot,		7.0f},
	{math::Vector2f(750,50),	2.0f, SquidGun::GT_Center,		2.0f},
	{math::Vector2f(750,550),	2.0f, SquidGun::GT_Center,		2.0f},
	{math::Vector2f(50,550),	2.0f, SquidGun::GT_Center,		2.0f},
	{math::Vector2f(50,50),		2.0f, SquidGun::GT_Center,		2.0f},
	{math::Vector2f(750,50),	2.0f, SquidGun::GT_Center,		4.0f},
	{math::Vector2f(50,50),		1.0f, SquidGun::GT_Center,		8.0f},
	{math::Vector2f(750,50),	1.0f, SquidGun::GT_Center,		8.0f},
	{math::Vector2f(50,50),		1.0f, SquidGun::GT_Center,		8.0f},
	{math::Vector2f(50,550),	1.0f, SquidGun::GT_Center,		8.0f},
	{math::Vector2f(50,50),		1.0f, SquidGun::GT_Center,		8.0f},
	{math::Vector2f(50,550),	1.0f, SquidGun::GT_Center,		8.0f},
	{math::Vector2f(750,550),	1.0f, SquidGun::GT_Center,		8.0f},
	{math::Vector2f(50,550),	1.0f, SquidGun::GT_Center,		8.0f},
	{math::Vector2f(750,550),	1.0f, SquidGun::GT_Center,		8.0f},
	{math::Vector2f(750,50),	1.0f, SquidGun::GT_Center,		8.0f},
	{math::Vector2f(750,550),	1.0f, SquidGun::GT_Center,		8.0f},
	{math::Vector2f(750,50),	1.0f, SquidGun::GT_Center,		8.0f}

};

SquidBoss::SquidBoss() : BossBase(60.0f * 10.0f) {
	Initiate();
}

SquidBoss::~SquidBoss(void) {

}

float SquidBoss::GetLifeLeft() {
	if (IsAlive()) {
		return (float)m_damage / (float)(GetMaxDamage());
	}
	return 0.0f;
}

bool SquidBoss::IsAlive() {
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

void SquidBoss::DoUpdate(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty) {
	const State state(GetState());
	static float deathTimer = 0.0f;
	deathTimer -= a_dt;

	const float maxScale(1.0);
	const float scale(/*(m_t * 0.3f > 1.0f ? 1.0f : m_t * 0.3f) * */maxScale);
	bool hasLiveTentacle(false);
	m_t += a_dt;

	//m_radius = 0.0f;

	if (state == S_Dying) {
		if (deathTimer < 0) {
			deathTimer = 0.1f;
			if (m_enemies[0] != NULL) {
				// kill the body
				for (int eIx = 0; eIx < g_numBodyEnemies; eIx++) {
					if (m_enemies[eIx] != NULL) {
						m_enemies[eIx]->Kill(true);
						m_enemies[eIx] = NULL;
					}
				}
			} else {
				// kill the tentacles from the inside out
				int offset(0);
				for (int eIx = g_numBodyEnemies; eIx < g_numEnemies; eIx++) {
					if (m_enemies[eIx] == NULL) {
						offset++;
					} else {
						break;
					}
				}

				for (int tIx = 0; tIx < g_numTentacles; tIx++) {
					int eIx = g_numBodyEnemies + tIx * g_numTentacleEnemies + offset;
					if (m_enemies[eIx] != NULL) {
						m_enemies[eIx]->Kill(true);
						m_enemies[eIx] = NULL;
					}
				}
			}
		}
		return;
	} else {
		const float yScale(cosf(m_t * 3.0f) * 0.2f);
		const float xScale(cosf(m_t * 3.0f) * 0.2f);

		math::Vector2f pos;

		m_frameTime += a_dt;
		while (m_frameTime >= g_frames[m_frameIx].m_time) {
			m_frameTime -= g_frames[m_frameIx].m_time;
			m_frameIx = (m_frameIx + 1) % g_numFrames;
		}

		math::Vector2f from;
		if (m_frameIx - 1 < 0) {
			from = g_frames[g_numFrames - 1].m_pos;
		} else {
			from = g_frames[m_frameIx - 1].m_pos;
		}

		pos = from + (g_frames[m_frameIx].m_pos - from) * sinf((m_frameTime / g_frames[m_frameIx].m_time) * PI_HALF);

		m_pSquidGun->SetGun(g_frames[m_frameIx].m_gun);

		//pos.y = g_height * 0.5f * (1.0f - m_radius) + ((cosf(m_t * 0.01f + PI) * 150.0f + 250) + cosf(m_t * 0.3f) * 100.0f) * m_radius;
		//pos.x = g_width * 0.5f - (cosf(m_t * 0.05f) * (500 - pos.y) * sinf(m_t * 0.03f)) * m_radius;

		for (int i = 0; i < g_numBodyEnemies; i++) {
			Enemy *pE(m_enemies[i]);
			if (pE != NULL) {
				//pE->Update(a_dt);

				pE->m_pos = pos;

				pE->m_pos.y += g_settings[i].m_pos.y * scale + yScale;
				pE->m_pos.x += g_settings[i].m_pos.x * scale + xScale;
				pE->m_scale = g_settings[i].m_scale * scale + yScale;
			}
		}

		m_enemies[0]->Update(a_dt * g_frames[m_frameIx].m_shootSpeed, a_difficulty);


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
	for (int eIx = 0; eIx < g_numEnemies; eIx++) {
		Enemy *pE(m_enemies[eIx]);
		if (pE != NULL) {
			if (pE->m_life < 10) {
				pE->m_life = 10;
				m_damage--;
			}
		}
	}
}

void SquidBoss::SimulateTentacles(float a_dt, float a_scale, const math::Vector2f &a_playerPos) {
	static const math::Vector2f offsets[g_numTentacles] = {math::Vector2f(-43, -50), math::Vector2f(-40, -37), math::Vector2f(-35, -22), math::Vector2f(-25, -15), math::Vector2f(-15, -7),
														  math::Vector2f(43, -50), math::Vector2f(40, -37), math::Vector2f(35, -22), math::Vector2f(25, -15), math::Vector2f(15, -7)};

	static const math::Vector2f targets[g_numTentacles] = {math::Vector2f(-300, -150), math::Vector2f(-350, 0), math::Vector2f(-290, 140), math::Vector2f(-215, 215), math::Vector2f(-120, 270),
														  math::Vector2f(300, -150), math::Vector2f(350, 0), math::Vector2f(290, 140), math::Vector2f(215, 215), math::Vector2f(120, 270)};

	for (int tIx = 0; tIx < g_numTentacles; tIx++) {

		if (m_enemies[g_numBodyEnemies + tIx * g_numTentacleEnemies] == NULL) {
			continue;
		}

		math::Vector2f basePos;
		m_enemies[g_numBodyEnemies + g_numTentacleEnemies * tIx]->m_pos = m_enemies[0]->m_pos;
		//m_enemies[g_numBodyEnemies + g_numTentacleEnemies * tIx]->m_pos += xOffset[tIx];
		//m_enemies[g_numBodyEnemies + g_numTentacleEnemies * tIx]->m_pos.x += cosf(m_t * 3.0f) * xOffset[tIx] * 0.5f;
		m_enemies[g_numBodyEnemies + g_numTentacleEnemies * tIx]->m_pos.y += 50.0f;

		m_enemies[g_numBodyEnemies + g_numTentacleEnemies * tIx]->m_pos += offsets[tIx];

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

			//dir = (a_playerPos - m_enemies[nextIx + g_numBodyEnemies + g_numTentacleEnemies * tIx]->m_pos);
			dir = (targets[tIx] + m_enemies[0]->m_pos - m_enemies[nextIx + g_numBodyEnemies + g_numTentacleEnemies * tIx]->m_pos);
			dir.Normalize();
			dir *= 10000 + 9500 + (sinf(m_t * 3.0f + (float)tIx) * 5000.0f);

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
				enemy.m_pos.y -= 20 * a_dt;
				enemy.m_scale = 0.2f + cosf(m_t * 3.0f + (g_numBodyEnemies + g_numTentacleEnemies - i - 1) * 0.2f) * 0.1f;
				enemy.m_scale *= a_scale;
			}
		}
	}
}

int SquidBoss::GetMaxDamage() const {
	return g_numEnemies * 1;
}

bool SquidBoss::HasLiveTentacle() {
	for (int i = g_numBodyEnemies; i < g_numTentacleEnemies * g_numTentacles; i++) {
		if (m_enemies[i] != NULL && m_enemies[i]->IsAlive()) {
			return true;
		}
	}
	return false;
}

void SquidBoss::DoInitiate() {
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
	m_frameTime = 0.0f;
	m_frameIx = 0;
}

SquidBoss::State SquidBoss::GetState() {
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

bool SquidBoss::AddEnemy(Enemy *a_pEnemy) {
	if (m_addedEnemies < g_numEnemies) {
		m_enemies[m_addedEnemies] = a_pEnemy;
		m_enemies[m_addedEnemies]->SetAlive(Enemy::T_Shock, 0.0f, 1.0f, -1, m_addedEnemies, false);
		m_enemies[m_addedEnemies]->m_life = 10;
			
		if (m_addedEnemies == 0) {
			delete m_enemies[0]->m_pGun;
			m_pSquidGun = new SquidGun();
			m_enemies[0]->m_pGun = m_pSquidGun;
		} else {
			delete m_enemies[m_addedEnemies]->m_pGun;
			m_enemies[m_addedEnemies]->m_pGun = new NullGun();//new DirGun();
			m_enemies[m_addedEnemies]->m_pGun->Reset();
			m_enemies[m_addedEnemies]->m_life = 10;
			m_enemies[m_addedEnemies]->m_doDropEnergy = m_addedEnemies % 8 == 0;
			if (m_addedEnemies >= g_numBodyEnemies) {
				m_enemies[m_addedEnemies]->m_hasShield = true;
			}
		}

		m_addedEnemies++;

		return true;
	}

	// this is to initiate the tentacles to good values
	//DoUpdate(0.1f, math::Vector2f(0.0f, 0.0f));
	m_enemies[0]->m_pos = g_frames[0].m_pos;
	for (int i = 0; i < 100; i++) {
		SimulateTentacles(0.2f, 0.0f, math::Vector2f(0.0f, 0.0f));
	}


	return false;
}



SquidBoss::SquidGun::SquidGun() {
	m_pCurrentGun = &m_nullGun;

	m_guns[GT_NULL] = &m_nullGun;
	m_guns[GT_Slow4Way] = &m_slowRotating4WayGun;
	m_guns[GT_Funky4Way] = &m_funky4WayGun;
	m_guns[GT_4Way] = &m_rotating4WayGun;
	m_guns[GT_TriShot] = &m_trishot;
	m_guns[GT_Cloud] = &m_cloud;
	m_guns[GT_Center] = &m_center;

	for (int i = 0; i < GT_Count; i++) {
		m_guns[i]->Reset();
	}

	m_cloud.m_shootTimer = 0.0f;
}

SquidBoss::SquidGun::~SquidGun() {

}

void SquidBoss::SquidGun::SetGun(GunType a_gun) {
	m_pCurrentGun = m_guns[a_gun];
}

void SquidBoss::SquidGun::Reset() {
	m_pCurrentGun->Reset();
}

void SquidBoss::SquidGun::Update(float a_dt) {
	//for (int i = 0; i < GT_Count; i++) {
		m_pCurrentGun->Update(a_dt);
	//}
}

bool SquidBoss::SquidGun::DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy) {
	return m_pCurrentGun->DoFire(a_playerPos, a_bullet, a_enemy);
}

}