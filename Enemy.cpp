// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include ".\enemy.h"

#include "IEventTarget.h"

namespace model {

const Enemy::TypeParams Enemy::g_params[T_Count] = {
	Enemy::TypeParams(math::Vector2f(14.5f, 14.5f), 4 * 1),
	Enemy::TypeParams(math::Vector2f(17.5f, 12.5f), 6 * 1),
	Enemy::TypeParams(math::Vector2f(27.5f, 17.5f), 12 * 1),
	Enemy::TypeParams(math::Vector2f(10.0f, 10.0f), 1 * 1),
	Enemy::TypeParams(math::Vector2f(8.0f, 11.0f), 2 * 1),
	Enemy::TypeParams(math::Vector2f(14.5f, 14.5f), 9 * 1),
	Enemy::TypeParams(math::Vector2f(10.5f, 10.5f), 4 * 1),
	Enemy::TypeParams(math::Vector2f(14.5f, 14.5f), 9 * 1),
	Enemy::TypeParams(math::Vector2f(27.5f, 17.5f), 24 * 1)
};

Gun* Enemy::ConstructGun() const {
	switch (m_type) {
		case T_Head:
			return  new Rotating4WayGun();
		case T_Ball:
			return  new DirGun();
		case T_BallBoss:
		case T_Segment:
			return  new StandardGun();
		case T_Drone:
			return new OneShotBombGun();
		case T_Shock2:
			return new AimOffsetGun();
		case T_Shock:
			return new InvShockWaveGun();
		case T_Spy:
			return new TriShotDirGun();
		case T_Pill:
			return new SpreadGun();
		default:
			return new NullGun();
	};
}

Enemy::Enemy(void) {
	m_life = 0;
	m_angle = 0.0f;
	m_type = (Type)(rand() % T_Count);
	m_pGun = ConstructGun();
	m_pGun->Reset();
	m_pos.Set(0.0f, 0.0f);
	m_oldPos = m_pos;
	m_doNotifyTarget = false;
	m_waveIx = -1;

	m_offset = m_pos;
	m_lastOffset = m_pos;
	m_hasShield = false;
	m_doDropEnergy = false;
}

Enemy::~Enemy(void) {
	delete m_pGun;
	m_pGun = NULL;
}

bool Enemy::IsAlive() const {
	return m_life > 0;
}

void Enemy::SetAlive(Enemy::Type a_type, float a_time, float a_scale, int a_waveIx, int a_orderIx, bool a_dropEnergy) {
	m_beam.Kill();

	m_waveIx = a_waveIx;
	m_orderIx = a_orderIx;
	m_startTime = fabsf(a_time);

	if (m_type != a_type) {
		m_type = a_type;
		delete m_pGun;
		m_pGun = ConstructGun();
	}
	
	m_hasShield = false;
	m_scale = a_scale;
	m_life = g_params[m_type].m_health;
	m_t = a_time;
	m_pGun->Reset();
	m_doNotifyTarget = false;
	m_doDropEnergy = a_dropEnergy;

	m_offset.Set(0.0f, 0.0f);
}
void Enemy::Kill(bool a_doNotifyTarget) {
	m_life = -1.0f;
	m_doNotifyTarget = a_doNotifyTarget;
}

void Enemy::Damage(int a_damage, bool a_doNotifyTarget) {
	if (m_hasShield != true) {
		m_life -= a_damage;
	}
}

void Enemy::Update(float a_dt, float a_difficulty) {
	m_t += a_dt * 0.1f;

	if (m_hasShield != true && m_t >= 0.0f) {
		m_pGun->Update(a_dt * a_difficulty);
	}
	m_beam.Update(a_dt);
}

bool Enemy::DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet) {
	if (m_hasShield != true) {
		return m_pGun->DoFire(a_playerPos, a_bullet, *this);
	}
	return false;
}

bool Enemy::DoBeam() const {
	return IsAlive() && m_beam.IsAlive() && m_hasShield != true;
}

void Enemy::GetBox(math::AABoundingBox2D &a_box) {
	a_box.m_size = g_params[m_type].m_size * m_scale;
	a_box.m_pos = m_pos;
}

}
