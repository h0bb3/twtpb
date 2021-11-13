// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include ".\formation.h"

#include <Common/PContainer.h>

namespace model {

Formation::Formation(void) : cmn::PComplex("f"){
	Register("es", new cmn::PContainer<EnemyList>(m_enemies, "e"));
}

Formation::Formation(const Formation &a_copy) : PComplex(a_copy) {
	Register("es", new cmn::PContainer<EnemyList>(m_enemies, "e"));
	*this = a_copy;
}

Formation& Formation::operator = (const Formation &a_copy) {
	m_enemies = a_copy.m_enemies;

	return *this;
}

Formation::~Formation(void) {
}

Formation::Enemy::Enemy() : PComplex("e") {
	RegisterVariables();
}
Formation::Enemy::Enemy(const Enemy &a_copy) : PComplex(a_copy) {
	RegisterVariables();
	*this = a_copy;
}

Formation::Enemy& Formation::Enemy::operator = (const Enemy &a_copy) {
	m_timeOffset = a_copy.m_timeOffset;
	m_enemyType = a_copy.m_enemyType;
	m_offset = a_copy.m_offset;
	m_scale = a_copy.m_scale;
	m_hasShield = a_copy.m_hasShield;

	return *this;
}

void Formation::Enemy::RegisterVariables() {
	Register("type", (int&)m_enemyType, 0);
	Register("time", m_timeOffset, 0.0f);
	Register("space.x", m_offset.x, 0.0f);
	Register("space.y", m_offset.y, 0.0f);
	Register("scale", m_scale, 1.0f);
	Register("shi", m_hasShield, false);
}

void Formation::Enemy::SetEnemyAlive(model::Enemy &a_enemy, int a_waveIx, int a_orderIx) const {
	a_enemy.SetAlive(m_enemyType, m_timeOffset, m_scale, a_waveIx, a_orderIx, false);
	a_enemy.m_offset = m_offset;
	a_enemy.m_hasShield = m_hasShield;
}

}