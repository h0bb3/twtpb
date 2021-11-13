// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "Missile.h"

#include "Enemy.h"
#include "IEventTarget.h"

namespace model {

Missile::Missile(void) {
	m_time = -1.0f;
	m_pSelectedEnemy = NULL;
}

Missile::~Missile(void) {
}

void Missile::Fire(math::Vector2f a_pos) {
	m_pos = a_pos;
	m_pSelectedEnemy = NULL;
	m_time = 2.0f;
	float angle = (float)(rand() % 1000) / 999.0f * PI_DOUBLE;
	m_speed.Set(cosf(angle) * 200.0f, sinf(angle) * 200.0f);
}

bool Missile::IsAlive() const {
	return m_time > 0.0f;
}

void Missile::Kill() {
	m_time = -1.0f;
}

void Missile::Update(float a_dt, const Enemy *a_pEnemies, int a_numEnemies, const math::Vector2f &a_playerPos) {
	if (IsAlive()) {
		m_time -= a_dt;
		if (m_pSelectedEnemy == NULL || m_pSelectedEnemy->IsAlive() != true) {
			int selectables[256];
			int selectableCount(0);

			for (int eIx = 0; eIx < a_numEnemies; eIx++) {
				if (a_pEnemies[eIx].IsAlive()) {
					if (a_pEnemies[eIx].m_pos.x > -50 && a_pEnemies[eIx].m_pos.x < g_width + 50 &&
						a_pEnemies[eIx].m_pos.y > -50 && a_pEnemies[eIx].m_pos.y < g_height + 50) {
						selectables[selectableCount] = eIx;
						selectableCount++;
					}
				}
			}
			if (selectableCount > 0) {
				m_pSelectedEnemy = &a_pEnemies[selectables[rand() % selectableCount]];
			} else {
				m_pSelectedEnemy = NULL;
			}
		}

		m_oldPos = m_pos;
		const math::Vector2f goal(m_pSelectedEnemy != NULL ? m_pSelectedEnemy->m_pos : a_playerPos);
		for (int i = 0; i < 11; i++) {
			math::Vector2f force(0.0f, 0.0f);
			math::Vector2f dir(goal - m_pos);
			force = dir;
			force.Normalize();
			force *= 150.0f;


			m_speed += force * 1.0f * a_dt;
			if (m_speed.Length2() > 100.0f * 100.0f) {
				m_speed.Normalize();
				m_speed *= 100.0f;
			}
			m_pos += m_speed * a_dt;
		}
	}
}

void Missile::GetBox(math::AABoundingBox2D &a_box) {
	a_box.m_pos = m_pos;
	a_box.m_size.Set(5.0f, 6.0f);
}

}
