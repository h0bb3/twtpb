// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include ".\beam.h"
#include "IEventTarget.h"

namespace model {

const float Beam::g_powerUpTime = 0.5f;

Beam::Beam(void) {
	m_timer = -1.0f;
	m_timer = -1.0f;
}

Beam::~Beam(void) {
}

void Beam::Update(float a_dt) {
	m_timer -= a_dt;
}


bool Beam::IsAlive() const {
	return m_timer > 0.0f;
}

bool Beam::IsLethal() const {
	return IsAlive() && (m_time - m_timer) > g_powerUpTime;
}

void Beam::SetAlive(float a_timeToLive, float a_width) {
	m_timer = a_timeToLive + g_powerUpTime;
	m_time = a_timeToLive + g_powerUpTime;
	m_width = a_width;
}

void Beam::Kill() {
	m_timer = -1.0f;
	m_time = -1.0f;
}

float Beam::GetNormalizedPowerupTimer() const {
	const float time((m_time - m_timer) / g_powerUpTime);

	if (time > 1.0f) {
		return 1.0f;
	}
	return time;
}



}