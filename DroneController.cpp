// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include ".\dronecontroller.h"
#include "IEventTarget.h"

namespace model {

DroneController::DroneController(void) {
	m_stateTime = 0;
	m_doFlee = false;
}

DroneController::~DroneController(void) {
}


bool DroneController::Update(float a_dt) {
	bool aliveEnemy(false);
	EnemyList::iterator eIt;
	math::Vector2f center(g_width * 0.5f, g_height * 0.5f);

	if (m_stateTime < 2.0f) {
		float t(m_stateTime / 2.0f);
		math::Vector2f start(g_width * 0.5f, -200.0f);
		center = start + (center - start) * t;
	}

	m_stateTime += a_dt;
	m_time += a_dt;

	if (m_doFlee != true) {
		int i = 0;
		for (eIt = m_enemies.begin(); eIt != m_enemies.end(); eIt++) {
			Enemy& enemy(**eIt);

			if (enemy.IsAlive()) {
				enemy.Update(a_dt);
				math::Vector2f oldPos(enemy.m_pos);

				enemy.m_pos.x = cosf((float)i / m_enemies.size() * PI_DOUBLE + cosf(m_time * 0.1f) * PI_DOUBLE) * (50.0f * cosf(m_time) + 100.0f) + center.x;
				enemy.m_pos.y = sinf((float)i / m_enemies.size() * PI_DOUBLE + cosf(m_time * 0.1f) * PI_DOUBLE) * (50.0f * cosf(m_time) + 100.0f) + center.y;


				enemy.m_angle = atan2f(enemy.m_pos.x - oldPos.x, enemy.m_pos.y - oldPos.y);

				aliveEnemy = true;
			} else if (m_stateTime > 2.0f) {
				m_doFlee = true;
				m_fleeTimer = 0.0f;
			}
			i++;
		}
	} else {

		m_fleeTimer += a_dt;

		for (eIt = m_enemies.begin(); eIt != m_enemies.end(); eIt++) {
			Enemy& enemy(**eIt);

			if (enemy.IsAlive()) {
				enemy.Update(a_dt);
				math::Vector2f oldPos(enemy.m_pos);
				math::Vector2f toCenter(center - enemy.m_pos);

				toCenter.Normalize();
				enemy.m_pos -= toCenter * a_dt * 10.0f * m_fleeTimer;
				if (enemy.m_pos.x > g_width + 10 || enemy.m_pos.x < -10 ||
					enemy.m_pos.y > g_height + 10 || enemy.m_pos.y < -10) {
					enemy.Kill(false);
				}

				enemy.m_angle = atan2f(enemy.m_pos.x - oldPos.x, enemy.m_pos.y - oldPos.y);
				aliveEnemy = true;

			}
		}

	}


	if (aliveEnemy != true) {
		m_enemies.clear();
	}

	return aliveEnemy;

}

}
