// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "TwinBoss.h"

#include "Enemy.h"

namespace model {

TwinBoss::BeamKey TwinBoss::g_beamKeys[g_numBeamKeys * g_numBodies] = {
	{3.0, 6.0, -1, false},
	{16.0, 6.0, 1, false},
	{18.0, 2.0, -1, false},
	{20.0, 2.0, 1, false},
	{23.0, 5.5, -0.5, false},

	{9.0, 6.0, 1, false},
	{16.0, 6.0, -1, false},
	{18.0, 2.0, 1, false},
	{20.0, 2.0, -1, false},
	{23.0, 5.5, -1, false}
};

TwinBoss::TwinBoss(void) : BossBase(60.0f * 3.0f) {
	Initiate();
}

TwinBoss::~TwinBoss(void) {
}

bool TwinBoss::IsAlive() {
	for (int eIx = 0; eIx < g_numEnemies; eIx++) {
		if (m_enemies[eIx] != NULL && m_enemies[eIx]->IsAlive()) {
			return true;
		}
	}

	return false;
}

bool TwinBoss::AddEnemy(Enemy *a_pEnemy) {
	if (m_addedEnemies < g_numEnemies) {
		m_enemies[m_addedEnemies] = a_pEnemy;
		m_addedEnemies++;
		a_pEnemy->SetAlive(Enemy::T_Shock, 0.0f, 1.0f, -1, m_addedEnemies, true);
		a_pEnemy->m_life = g_maxLife / 2;
		a_pEnemy->m_angle = 0.0f;
		delete a_pEnemy->m_pGun;
		a_pEnemy->m_pGun = new NullGun();
		return true;
	}

	return false;
}

float TwinBoss::GetLifeLeft() {
	int life(0);

	for (int bIx = 0; bIx < g_numBodies; bIx++) {
		if (m_enemies[bIx * g_numBodyEnemies] != NULL && m_enemies[bIx * g_numBodyEnemies]->IsAlive()) {
			life += m_enemies[bIx * g_numBodyEnemies]->m_life;
		}
	}

	return (float)life / (float)g_maxLife;
}

void TwinBoss::DoInitiate() {
	for (int eIx = 0; eIx < g_numEnemies; eIx++) {
		m_enemies[eIx] = NULL;
	}
	m_maxBeamTime = 0.0f;
	for (int bkIx = 0; bkIx < g_numBeamKeys * g_numBodies; bkIx++) {
		g_beamKeys[bkIx].m_hasTriggered = false;
		if (g_beamKeys[bkIx].m_time + g_beamKeys[bkIx].m_ttl > m_maxBeamTime) {
			m_maxBeamTime = g_beamKeys[bkIx].m_time + g_beamKeys[bkIx].m_ttl;
		}
	}
	m_maxBeamTime += 1.0f;
	for (int sIx = 0; sIx < g_numBodies; sIx++) {
		m_beamSpeeds[sIx] = g_beamKeys[g_numBeamKeys * sIx].m_speed;
	}
	m_addedEnemies = 0;
	m_time = 0;
	m_beamKeyTime = 0;
}

void TwinBoss::DoUpdate(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty) {

	m_time += a_dt;
	m_beamKeyTime += a_dt;

	while (m_beamKeyTime > m_maxBeamTime) {
		m_beamKeyTime -= m_maxBeamTime;
		for (int bkIx = 0; bkIx < g_numBeamKeys * g_numBodies; bkIx++) {
			g_beamKeys[bkIx].m_hasTriggered = false;
		}
		for (int eIx = 0; eIx < g_numBodies; eIx++) {
			m_enemies[eIx * g_numBodyEnemies]->m_angle = 0.0f;
			m_beamSpeeds[eIx] = g_beamKeys[g_numBeamKeys * eIx].m_speed;
		}
	}

	for (int bIx = 0; bIx < g_numBodies; bIx++) {
		Enemy& be(*m_enemies[bIx * g_numBodyEnemies]);

		// find the possible beam key
		int beamKeyIx(-1);
		for (int bkIx = 0; bkIx < g_numBeamKeys; bkIx++) {
			BeamKey &bk(g_beamKeys[bkIx + bIx * g_numBeamKeys]);
			if (bk.m_time < m_beamKeyTime && bk.m_hasTriggered == false) {
				beamKeyIx = bkIx + bIx * g_numBeamKeys;
				bk.m_hasTriggered = true;
				m_beamSpeeds[bIx] = g_beamKeys[beamKeyIx].m_speed;
				break;
			}
		}

		// update body
		if (be.IsAlive()) {
			float xs[2] = {200, 600};
			float tms[2] = {1.0f, -1.0f};
			be.m_angle += a_dt * m_beamSpeeds[bIx];
			be.m_pos.Set(xs[bIx], 300);
		}


		// update contents
		for (int eIx = 1; eIx < g_numBodyEnemies; eIx++) {
			Enemy& e(*m_enemies[eIx + bIx * g_numBodyEnemies]);
			if (e.IsAlive()) {

				const float angle(((float)(eIx - 1) / (float)(g_numBodyEnemies - 1)) * PI_DOUBLE + be.m_angle);

				e.m_scale = 0.25;
				e.m_pos.Set(cosf(angle) * 15.0f, sinf(angle) * 15.0f);
				e.m_pos += be.m_pos;
				e.m_angle = angle;
				e.m_beam.m_angle = angle;
				if (beamKeyIx >= 0 && e.DoBeam() == false) {
					e.m_beam.SetAlive(g_beamKeys[beamKeyIx].m_ttl, 5);
				}
				e.Update(a_dt, a_difficulty);
				if (be.IsAlive() != true) {
					e.Kill(true);
				}
			}
		}
	}
}

}
	