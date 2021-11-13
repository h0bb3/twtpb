// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include ".\formationcontroller.h"
#include "IEventTarget.h"


namespace model {

	FormationController::FormationController(void) : m_enemies(m_enemiesI) {
	m_speedMultiplier = 1.0f;
	m_currentOffset.Set(0.0f, 0.0f);
}

FormationController::~FormationController(void) {
}

void FormationController::AddEnemy(Enemy *a_pEnemy) {
	m_enemiesI.push_back(a_pEnemy);
	m_originalEnemiesSize = m_enemies.size();
}


float FormationController::ComputeArea(float a_t) {
	const float power(24.0f);
	const float limit(power + 10.0f);
	const float lower(-cos(PI) * power + PI * limit);
	const float upper(-cos(PI_DOUBLE) * power + PI_DOUBLE * limit);
	const float maxArea(upper - lower);
	
	const float angle(a_t * PI + PI);

	return (angle * limit - cos(angle) * power - lower) / maxArea;
}
bool FormationController::Update(float a_dt, const float a_difficulty) {
	EnemyList::iterator eIt;

	for (eIt = m_enemiesI.begin(); eIt != m_enemiesI.end();) {

		if ((*eIt)->IsAlive() && (*eIt)->m_t >= 0.0f && (*eIt)->m_t <= 1.0f ) {
			Enemy& enemy(**eIt);
			enemy.Update(a_dt * m_speedMultiplier, a_difficulty);
			if (enemy.m_t > 1.0f) {
				enemy.Kill(false);
				eIt = m_enemiesI.erase(eIt);
				m_enemyEscaped = true;
				break;
			}
			enemy.m_pos -= enemy.m_lastOffset;
			math::Vector2f oldPos(enemy.m_pos);
			switch (m_currentFormation) {
				case F_VerticalSine:
					{
					const float t(ComputeArea(enemy.m_t));
					enemy.m_pos.x = cosf(t * PI_DOUBLE * 3.0f) * 75.0f + g_width * 0.5f;
					enemy.m_pos.y = t * 700.0f - 50;
					}
				break;
				case F_HorizontalSineLR:
					{
					const float t(ComputeArea(enemy.m_t));
					enemy.m_pos.x = t * 900 - 50;
					enemy.m_pos.y = cosf(t * PI_DOUBLE * 3.0f) * 50.0f + 150.0f;
					}
				break;
				case F_EllipticLR:
					{
					const float t(ComputeArea(enemy.m_t));
					const float radius(cosf(enemy.m_t * 27.0f) * 25.0f);
					enemy.m_pos.x = cosf(-t * PI - PI) * (325.0f + radius) + g_width * 0.5f;
					enemy.m_pos.y = sinf(-t * PI - PI) * (225.0f + radius) - 50.0f;
					}
				break;
				case F_HorizontalSineRL:
					{
					const float t(ComputeArea(enemy.m_t));
					enemy.m_pos.x =  (1.0f - t) * 900 - 50.0f;
					enemy.m_pos.y = cosf((1.0f - t) * PI_DOUBLE * 3.0f + PI) * 50.0f + 150.0f;
					}
				break;
				case F_EllipticRL:
					{
					

					const float t(ComputeArea(enemy.m_t));
					const float radius(cosf(enemy.m_t * 27.0f) * 25.0f);
					enemy.m_pos.x = cosf(-(1.0f - t) * PI + PI) * (325.0f + radius) + g_width * 0.5f;
					enemy.m_pos.y = sinf(-(1.0f - t) * PI + PI) * (220.0f + radius) - 50.0f;
					}
				break;
				case F_VerticalSineLeft:
					{
					const float t(ComputeArea(enemy.m_t));
					enemy.m_pos.x = cosf(t * PI_DOUBLE * 3.0f) * 85.0f + g_width * 0.5f - 200.0f;
					enemy.m_pos.y = t * 700.0f - 50;
					}
				break;
				case F_VerticalSineRight:
					{
					const float t(ComputeArea(enemy.m_t));
					enemy.m_pos.x = cosf(t * PI_DOUBLE * 3.0f + PI) * 85.0f + g_width * 0.5f + 200.0f;
					enemy.m_pos.y = t * 700.0f - 50;
					}
				break;
				case F_VerticalSineOffset:
					{
					const float t(ComputeArea(enemy.m_t));
					enemy.m_pos.x = cosf(t * PI_DOUBLE * 3.0f + PI) * 75.0f + g_width * 0.5f;
					enemy.m_pos.y = t * 700.0f - 50;
					}
				break;

				case F_CircleLRBottom:
					{
					const float t(ComputeArea(enemy.m_t));
					enemy.m_pos.x = cosf(t * PI - PI) * 350.0f + g_width * 0.5f;
					enemy.m_pos.y = sinf(t * PI - PI) * 500.0f + g_height + 50.0f;
					}
				break;
				case F_CircleRLBottom:
					{
					const float t(ComputeArea(enemy.m_t));
					enemy.m_pos.x = cosf((1.0f - t) * PI - PI) * 350.0f + g_width * 0.5f;
					enemy.m_pos.y = sinf(t * PI - PI) * 500.0f + g_height + 50.0f;
					}
				break;
				case F_SQUARE_CW_1:
					if (enemy.m_t < 0.25) {
						enemy.m_pos.x = 50.0f;
						enemy.m_pos.y = (g_height + 50) - g_height * enemy.m_t / 0.25f;
					} else if (enemy.m_t < 0.75) {

						enemy.m_pos.x = 50.0f + (g_width - 100.0f) * (enemy.m_t - 0.25f) / 0.50f;
						enemy.m_pos.y = 50.0f;

					} else {
						enemy.m_pos.x = (g_width - 50.0f);
						enemy.m_pos.y = 50.0f + g_height * (enemy.m_t - 0.75f) / 0.25f;
					}
				break;

				case F_SQUARE_CW_2:
					if (enemy.m_t < 0.25) {
						enemy.m_pos.x = 100.0f;
						enemy.m_pos.y = (g_height + 50) - (g_height -50.0f) * enemy.m_t / 0.25f;
					} else if (enemy.m_t < 0.75) {

						enemy.m_pos.x = 100.0f + (g_width - 200.0f) * (enemy.m_t - 0.25f) / 0.50f;
						enemy.m_pos.y = 100.0f;

					} else {
						enemy.m_pos.x = (g_width - 100.0f);
						enemy.m_pos.y = 100.0f + (g_height - 50.0f) * (enemy.m_t - 0.75f) / 0.25f;
					}
				break;

				case F_Slash_Back:
					enemy.m_pos.x = g_width * 0.5f - enemy.m_startTime * 850.0f;
					enemy.m_pos.y = -110.0f + (g_height + 160.0f) * enemy.m_t + cosf(enemy.m_t * 20.0f) * 60.0f;
				break;

				case F_Slash_Front:
					enemy.m_pos.x = g_width * 0.5f + enemy.m_startTime * 850.0f;
					enemy.m_pos.y = -110.0f + (g_height + 160.0f) * enemy.m_t + cosf(enemy.m_t * 20.0f) * 60.0f;
				break;

				case F_Pause:
					enemy.m_pos.x = 0.0f;
					enemy.m_pos.y = 0.0f;
				break;

				case F_FunkLeft:
				{
					const float t(enemy.m_t * 1.14f - 0.07f);
					//enemy.m_pos.x = 120 + cos(t * 9) * (75 + enemy.m_pos.y * 0.1f) * sin(t * 17);
					enemy.m_pos.y = (t * 700.0f - 50) + cos(t * 17) * 100;

					enemy.m_pos.x = 120 + cos(t * 9 + PI) * (75 + enemy.m_pos.y * 0.1f) * sin(t * 17 + PI);
				}
				break;

				case F_FunkRight:
				{
					const float t(enemy.m_t * 1.14f - 0.07f);
					enemy.m_pos.y = (t * 700.0f - 50) + cos(t * 17) * 100;
					enemy.m_pos.x = g_width - 120 - cos(t * 9) * (75 + enemy.m_pos.y * 0.1f) * sin(t * 17);
					
				}
				break;

				case F_FunkMiddleLeft:
				{
					const float t(enemy.m_t * 1.14f - 0.07f);
					enemy.m_pos.y = (t * 700.0f - 50) + cos(t * 17) * 100;
					enemy.m_pos.x = g_width * 0.5f + cos(t * 9 + PI) * (125 + enemy.m_pos.y * 0.1f) * sin(t * 23 + PI);
					
				}
				break;

				case F_FunkMiddleRight:
				{
					const float t(enemy.m_t * 1.14f - 0.07f);
					enemy.m_pos.y = (t * 700.0f - 50) + cos(t * 17) * 100;
					enemy.m_pos.x = g_width * 0.5f - cos(t * 9 + PI) * (125 + enemy.m_pos.y * 0.1f) * sin(t * 23 + PI);
					
				}
				break;

				case F_VerticalCatterpillar:
					{
							const float t(ComputeArea(enemy.m_t));
							enemy.m_pos.x = (float)g_width * 0.5f;
							enemy.m_pos.y = t * 700.0f - 50;
					}
				break;

				case F_HorizontalCatterpillarLR:
					{
						const float t(ComputeArea(enemy.m_t));
						enemy.m_pos.x = t * 900.0f - 50.0f;
						enemy.m_pos.y = (float)g_height * 0.5f;
					}
				break;

				case F_HorizontalCatterpillarRL:
					{
						const float t(ComputeArea(enemy.m_t));
						enemy.m_pos.x = 850.0f - t * 900.0f;
						enemy.m_pos.y = (float)g_height * 0.5f;
					}
				break;

				case F_VerticalFlower:
					{
						enemy.m_doDropEnergy = true;
						float t;
						if (enemy.m_t < 0.25f) {
							t = ComputeArea(enemy.m_t / 0.25f);
							enemy.m_hasShield = true;
						} else {
							enemy.m_hasShield = false;
							t = 1.0f;
						}

						enemy.m_pos.x = (float)g_width * 0.5f;
						enemy.m_pos.y = t * 200.0f - 50;
						if (enemy.m_t > 0.25f) {
							const float nt(ComputeArea((enemy.m_t - 0.25f) / 0.75f));
							const float angle((float)enemy.m_orderIx / (float)m_originalEnemiesSize * PI_DOUBLE);
							
							enemy.m_pos.x += cosf(angle) * nt * 800.0f;
							enemy.m_pos.y += sinf(angle) * nt * 600.0f;
						}
					}
				break;

			}

			enemy.m_pos += m_currentOffset;

			enemy.m_angle = atan2f(enemy.m_pos.x - oldPos.x, enemy.m_pos.y - oldPos.y);

			const float c(cosf(enemy.m_angle));
			const float s(sinf(enemy.m_angle));

			enemy.m_lastOffset.y = c * enemy.m_offset.x - s * enemy.m_offset.y;
			enemy.m_lastOffset.x = s * enemy.m_offset.x + c * enemy.m_offset.y;

			enemy.m_pos += enemy.m_lastOffset;

			eIt++;
		} else if ((*eIt)->IsAlive() != true) {
			eIt = m_enemiesI.erase(eIt);
		} else {
			(*eIt)->Update(a_dt * m_speedMultiplier, a_difficulty);
			(*eIt)->m_pos.Set(-1000.0f, -1000.0f);
			eIt++;
		}
	}

	return m_enemies.size() > 0;
}

void FormationController::RemoveDeadEnemies() {
	EnemyList::iterator it;
	for (it = m_enemiesI.begin(); it != m_enemiesI.end();) {
		if ((*it)->IsAlive() != true) {
			it = m_enemiesI.erase(it);
		} else {
			it++;
		}
	}
}

}