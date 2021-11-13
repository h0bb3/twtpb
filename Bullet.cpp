// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include ".\bullet.h"

#include "IEventTarget.h"

namespace model {

Bullet::Bullet(void) {
	Kill();
}

Bullet::~Bullet(void) {
}

bool Bullet::IsAlive() const {
	return m_age >= 0.0f;
}

void Bullet::Kill() {
	m_age = -1.0f;
}

void Bullet::Spawn(const math::Vector2f &a_pos) {
	m_pos = a_pos;
	m_age = 0.0f;
}

void Bullet::Update(float a_dt) {
	m_age += a_dt;
	m_pos += m_speed * a_dt;
}

void Bullet::GetBox(math::AABoundingBox2D &a_box) {
	a_box.m_size.Set(g_bulletSize, g_bulletSize);
	a_box.m_pos = m_pos;
}

const math::Vector2f &Bullet::GetPos() const {
	return m_pos;
}

const float Bullet::GetAge() const {
	return m_age;
}

}