// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include ".\gun.h"

#include "Enemy.h"
#include "Bullet.h"
#include "ieventtarget.h"


namespace model {

float RandomFloat() {
	return ((float)(rand() % 1001) / 1000.0f);
}

Gun::Gun(void) {
}

Gun::~Gun(void) {
}

void Gun::Update(float a_dt) {
	m_shootTimer -= a_dt;
}

void Gun::Reset() {
	m_shootTimer =  RandomFloat() * 1.0f + 0.5f;
}



bool StandardGun::DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy) {
	if (m_shootTimer < 0.0f) {
		m_shootTimer = RandomFloat() * 3.0f + 0.3f;

		a_bullet.Spawn(a_enemy.m_pos);
		a_bullet.m_speed = (a_playerPos - a_enemy.m_pos);
		a_bullet.m_speed.Normalize();
		a_bullet.m_speed *= 150.0f;

		return true;
	}

	return false;
}

void OneShotBombGun::Reset() {
	Gun::Reset();
	m_time = 0.1f + RandomFloat() * 0.4f;
}

bool OneShotBombGun::DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy) {
	if (a_enemy.m_t > m_time) {
		if (m_time < 0.5f) {
			m_time += 0.1f + RandomFloat() * 0.3f;
		} else {
			m_time = 2.0f;
		}

		a_bullet.Spawn(a_enemy.m_pos);
		a_bullet.m_speed = (a_playerPos - a_enemy.m_pos);
		a_bullet.m_speed.Set(0.0f, 1.0f);
		a_bullet.m_speed.Normalize();
		a_bullet.m_speed *= 250.0f;

		return true;
	}

	return false;
}


void Rotating4WayGun::Reset() {
	Gun::Reset();
	m_shootCounter = 0;
	m_shootTimer = 0.0f;
}

bool Rotating4WayGun::DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy) {
	if (m_shootTimer < 0.0f) {
		if (m_shootCounter >= 4) {
			m_shootTimer = 0.2f;
			m_shootCounter = 0;
			return false;
		} else {
			static const math::Vector2f speeds[4] = {math::Vector2f(0.0f, 250.0f), math::Vector2f(0.0f, -250.0f), math::Vector2f(250.0f, 0.0f), math::Vector2f(-250.0f, 0.0f)};
			a_bullet.Spawn(a_enemy.m_pos);
			//a_bullet.m_speed = speeds[m_shootCounter] + m_pos - m_oldPos;
			a_bullet.m_speed.x = cos(a_enemy.m_t * PI_DOUBLE * 2.0f + (float)m_shootCounter * PI_HALF) * 250.0f;
			a_bullet.m_speed.y = sin(a_enemy.m_t * PI_DOUBLE * 2.0f + (float)m_shootCounter * PI_HALF) * 250.0f;

			a_bullet.m_speed += a_enemy.m_pos - a_enemy.m_oldPos;

			m_shootCounter++;
			return true;
		}
	}

	return false;
}

SlowRotating4WayGun::SlowRotating4WayGun(float a_dir) : m_dir(a_dir) {
}

void SlowRotating4WayGun::Reset() {
	Gun::Reset();
	m_shootCounter = 0;
	m_shootTimer = 0.0f;
}

bool SlowRotating4WayGun::DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy) {
	if (m_shootTimer < 0.0f) {
		if (m_shootCounter >= 4) {
			m_shootTimer = 0.1f;
			m_shootCounter = 0;
			return false;
		} else {
			a_bullet.Spawn(a_enemy.m_pos);
			//a_bullet.m_speed = speeds[m_shootCounter] + m_pos - m_oldPos;
			a_bullet.m_speed.x = cos(a_enemy.m_t * PI_DOUBLE * m_dir + (float)m_shootCounter * PI_HALF) * 100.0f;
			a_bullet.m_speed.y = sin(a_enemy.m_t * PI_DOUBLE * m_dir + (float)m_shootCounter * PI_HALF) * 100.0f;

			a_bullet.m_speed += a_enemy.m_pos - a_enemy.m_oldPos;

			m_shootCounter++;
			return true;
		}
	}

	return false;
}

void FunkyRotating4WayGun::Reset() {
	Gun::Reset();
	m_shootCounter = 0;
	m_shootTimer = 0.0f;
	m_dirTime = 1.0f;
	m_dir = 0.3f;
	m_time = 0.0f;
}

void FunkyRotating4WayGun::Update(float a_dt) {
	Gun::Update(a_dt);
	m_dirTime -= a_dt;
	if (m_dirTime < 0.0f) {
		m_dir *= -1.0f;
		m_dirTime = 1.0f;
	}
	m_time += a_dt * m_dir;
}

bool FunkyRotating4WayGun::DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy) {
	if (m_shootTimer < 0.0f) {
		if (m_shootCounter >= 4) {
			m_shootTimer = 0.05f;
			m_shootCounter = 0;
			return false;
		} else {
			a_bullet.Spawn(a_enemy.m_pos);
			//a_bullet.m_speed = speeds[m_shootCounter] + m_pos - m_oldPos;
			a_bullet.m_speed.x = cos(m_time * PI_DOUBLE + (float)m_shootCounter * PI_HALF) * 150.0f;
			a_bullet.m_speed.y = sin(m_time * PI_DOUBLE + (float)m_shootCounter * PI_HALF) * 150.0f;

			a_bullet.m_speed += a_enemy.m_pos - a_enemy.m_oldPos;

			m_shootCounter++;
			return true;
		}
	}

	return false;
}

bool NullGun::DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy) {
	return false;
}

void ShockWaveGun::Reset() {
	m_shootTimer = 1.0f;
}

bool ShockWaveGun::DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy) {
	if (m_shootTimer < 0.0f) {
		m_shootTimer = 0.2f;
		a_bullet.Spawn(a_enemy.m_pos);

		math::Vector2f dir = a_enemy.m_pos - math::Vector2f(g_width / 2, g_height / 2);
		dir.Normalize();

		a_bullet.m_speed = dir * 300.0f;
		return true;
	}
	return false;
}

void InvShockWaveGun::Reset() {
	m_shootTimer = 1.0f;
}

bool InvShockWaveGun::DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy) {
	if (m_shootTimer < 0.0f) {
		m_shootTimer = 0.2f;
		a_bullet.Spawn(a_enemy.m_pos);

		math::Vector2f dir = math::Vector2f(g_width / 2, g_height / 2) - a_enemy.m_pos;
		dir.Normalize();

		a_bullet.m_speed = dir * 300.0f;
		return true;
	}
	return false;
}

void DirGun::Reset() {
	m_shootTimer = 1.0f;
}

bool DirGun::DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy) {
	if (m_shootTimer < 0.0f) {

		a_bullet.Spawn(a_enemy.m_pos);

		float angle(a_enemy.m_angle + PI_HALF);

		math::Vector2f dir(cosf(angle + PI), sinf(angle));
		a_bullet.m_speed = dir * 300.0f;
		m_shootTimer = 0.75f;
		return true;
	}
	return false;
}

void TriShotDirGun::Reset() {
	m_shootTimer = 1.0f;
	m_shootCounter = 0;
}

bool TriShotDirGun::DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy) {
	if (m_shootTimer < 0.0f) {
		if (m_shootCounter < 2) {
			m_shootTimer = 0.2f;
			m_shootCounter++;
		} else {
			m_shootTimer = 1.0f;
			m_shootCounter = 0;
		}

		a_bullet.Spawn(a_enemy.m_pos);
		a_bullet.m_speed = (a_playerPos - a_enemy.m_pos);
		a_bullet.m_speed.Normalize();
		a_bullet.m_speed *= 350.0f;

		return true;
	}
	return false;
}

void AimOffsetGun::Reset() {
	m_shootTimer = -10.0f;
}

bool AimOffsetGun::DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy) {
	// first time reset
	if (m_shootTimer <= -10.0f) {
		m_shootTimer = 0.5f + a_enemy.m_startTime * 10.0f;
		return false;
	}
	if (m_shootTimer < 0.0f) {
		m_shootTimer = 2.5f;
		a_bullet.Spawn(a_enemy.m_pos);
		a_bullet.m_speed = (a_playerPos - a_enemy.m_pos);
		a_bullet.m_speed.Normalize();
		a_bullet.m_speed *= 350.0f;

		return true;
	}
	return false;
}

void SpreadGun::Reset() {
	m_shootTimer = 1.0f;
	m_firedBullets = 0;
	m_side = 0;
}

bool SpreadGun::DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy) {

	if (m_shootTimer < 0.0f) {
		static const float angles[4] = {- 0.5f, - 0.4f, 0.4f, 0.5f};

		const float angle(atan2(-a_enemy.m_pos.y + a_playerPos.y, -a_enemy.m_pos.x + a_playerPos.x));
		const int side((m_side % 24) < 12 ? 0 : 2);

		a_bullet.Spawn(a_enemy.m_pos);
		a_bullet.m_speed.x = cosf(angle + angles[m_firedBullets + side]) * 800.0f;
		a_bullet.m_speed.y = sinf(angle + angles[m_firedBullets + side]) * 800.0f;

		
		if (m_firedBullets < 2) {
			m_firedBullets++;
			return true;
		} else {
			m_side++;
			m_shootTimer = 0.15f;
			m_firedBullets = 0;
		}
	}

	return false;
}

void CloudGun::Reset() {
	m_shootTimer = 7.0f;
	m_firedBullets = 0;
}

bool CloudGun::DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy) {

	if (m_shootTimer < 0.0f) {

		const float angle(atan2(-a_enemy.m_pos.y + a_playerPos.y, -a_enemy.m_pos.x + a_playerPos.x));
		const float speed(100.0f + (float)(rand() % 50));
		const float angleOffset(((float)(rand() % 1001) / 1000.0f - 0.5f) * PI_QUARTER);

		a_bullet.Spawn(a_enemy.m_pos);
		a_bullet.m_speed.x = cosf(angle + angleOffset) * speed;
		a_bullet.m_speed.y = sinf(angle + angleOffset) * speed;

		if (m_firedBullets < 37) {
			m_firedBullets++;
			m_shootTimer = (float)(rand() % 1001) / 1000.0f * 0.2;
		} else {
			Reset();
		}
		return true;
	}

	return false;
}



}


