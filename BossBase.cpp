// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "BossBase.h"

namespace model {

BossBase::BossBase(float a_timeToLive) : m_timeToLive(a_timeToLive) {
}

BossBase::~BossBase(void) {
}

float BossBase::GetTimeLeft() {
	if (m_time > 0.0f) {
		return m_time / m_timeToLive;
	}
	return 0.0f;
}

void BossBase::Initiate() {
	m_time = m_timeToLive;
	DoInitiate();
}

void BossBase::Update(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty) {
	if (m_isAgressive != true) {
		m_time -= a_dt * 3.0f;
	} else {
		m_time -= a_dt;
	}
	
	DoUpdate(a_dt, a_playerPos, m_isAgressive, a_difficulty);
}

}