// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include ".\player.h"

#include "IEventTarget.h"

namespace model {

const Player::AbilityInfo Player::g_abilities[A_Count] = {AbilityInfo(0, 0), AbilityInfo(3, 0), AbilityInfo(2, 1), AbilityInfo(2, 2), AbilityInfo(2, 3)};
/*const Player::Gun Player::g_guns[g_maxGuns] = {	Gun(math::Vector2f(-6.0, -18), math::Vector2f(0, -1)), Gun(math::Vector2f(6.0, -18), math::Vector2f(0, -1)),
												Gun(math::Vector2f(0, -20), math::Vector2f(0, -1)),
												Gun(math::Vector2f(-11, 0), math::Vector2f(-175, -900)), Gun(math::Vector2f(11, 0), math::Vector2f(175, -900)),
												Gun(math::Vector2f(0, 19), math::Vector2f(0, 1)),
												Gun(math::Vector2f(-20, 0), math::Vector2f(-1, 0)), Gun(math::Vector2f(20, 0), math::Vector2f(1, 0))};*/

Player::Player(void) :	m_pos(0.0f, 0.0f), m_dPos(0.0f, 0.0f), m_movement(m_dPos), m_doFire(false), m_score(0), m_life(0), m_weapon(0),
						m_fireTime(0.2f), m_maxEnergy(g_defaultEnergy), m_energy(g_defaultEnergy), m_useEnergy(false),
						m_ability(Player::A_RapidFire), m_maxShield(g_defaultEnergy), m_shield(g_defaultEnergy)
{

	m_guns[0] = Gun(math::Vector2f(-6.0, -18), math::Vector2f(0, -1));
	m_guns[1] = Gun(math::Vector2f(6.0, -18), math::Vector2f(0, -1));
	m_guns[2] = Gun(math::Vector2f(0, -20), math::Vector2f(0, -1));
	m_guns[3] = Gun(math::Vector2f(-11, 0), math::Vector2f(-175, -900));
	m_guns[4] = Gun(math::Vector2f(11, 0), math::Vector2f(175, -900));
	m_guns[5] = Gun(math::Vector2f(0, 19), math::Vector2f(0, 1));
	m_guns[6] = Gun(math::Vector2f(-20, 0), math::Vector2f(-1, 0));
	m_guns[7] = Gun(math::Vector2f(20, 0), math::Vector2f(1, 0));
}

Player::~Player(void) {
}

bool Player::Damage(int a_amount, float a_gameTime) {
	//return true;
	m_shield -= a_amount;
	m_multiplier = 1;
	if (m_shield < 0) {
		m_lifeLostTime = a_gameTime;
		m_useEnergy = false;
		m_doFire = false;
		m_life--;
		return false;
	}

	return true;
}

void Player::NextLevel() {
	m_shield = m_maxShield;
	m_energy = m_maxEnergy;
	m_pos.x = g_width * 0.5f;
	m_pos.y = g_height - g_playerHeight * 2;
	m_multiplier = 1;
}

void Player::RestartLevel() {
	m_shield = m_maxShield;
	m_energy = m_maxEnergy;
	//m_weapon--;
	if (m_weapon < m_baseWeapon) {
		m_weapon = m_baseWeapon;
	}
	m_pos.x = g_width * 0.5f;
	m_pos.y = g_height - g_playerHeight * 2;
}

bool Player::IsActive(float a_gameTime) const {
	return m_life > 0 && a_gameTime - m_lifeLostTime < 0;
}

void Player::Init(int a_startingWeapon, int a_lives) {
	m_pos.x = g_width * 0.5f;
	m_pos.y = g_height - g_playerHeight * 2;
	m_dPos.Set(0.0f, 0.0f);
	m_doFire = false;
	m_life = a_lives;
	m_energy = Player::g_defaultEnergy;
	m_maxEnergy = Player::g_defaultEnergy;
	m_shield = Player::g_defaultShield;
	m_maxEnergy = Player::g_defaultShield;
	m_weapon = a_startingWeapon;
	m_ability = Player::A_RapidFire;
	m_lifeLostTime = 60 * 60 * 24.0f;	// 24h game :P
	m_regenerationTimer = 0.0f;
	m_multiplier = 1;
}

void Player::Update(float a_dt) {
	m_fireTimer -= a_dt;
	m_pos += m_dPos;
	m_movement = m_dPos;
	m_dPos.Set(0.0f, 0.0f);

	if (m_useEnergy && m_ability == A_Regeneration) {
		m_regenerationTimer -= a_dt;
		if (m_regenerationTimer < 0.0f) {
			if (m_energy > g_abilities[A_Regeneration].m_cost && m_shield < m_maxShield) {
				m_energy -= g_abilities[A_Regeneration].m_cost;
				m_shield++;
			}
			m_regenerationTimer = 0.1f;
		}
	}
}

bool Player::DoFire(int a_level) {
	if (m_doFire) {
		m_doFire = false;
		if (m_fireTimer < 0.0f) {
			
			if (IsUsingAbility(A_RapidFire, a_level)) {
				m_energy -= g_abilities[A_Regeneration].m_cost;
				if (m_energy < 0) {
					m_energy = 0;
				}
				m_fireTimer = m_fireTime * 0.25f;
				m_useEnergy = false;
			} else {
				m_fireTimer = m_fireTime; 
			}
			return true;
		}
		
		return false;
	}
	return false;
}
bool Player::CanUseAbility(Ability a_ability, int a_levelIx) const {
	return a_levelIx >= g_abilities[a_ability].m_level;
}

bool Player::IsUsingAbility(Ability a_ability, int a_levelIx) const {
	return m_useEnergy && a_ability == m_ability && m_energy > 0 && a_levelIx >= g_abilities[a_ability].m_level;
}

int Player::DoFireMissiles(int a_level) {
	if (IsUsingAbility(A_Missiles, a_level)) {
		float count((float)(m_energy / g_abilities[A_Regeneration].m_cost));
		const float maxCount((float)(g_defaultEnergy / g_abilities[A_Regeneration].m_cost));
		count /= maxCount;
		count = count * count;
		count *= maxCount;
		if (count > 0) {
			m_energy = 0;
		}
		
		return (int)count + 1;
	}
	return 0;
}

void Player::GetBox(math::AABoundingBox2D &a_box) const {
	a_box.m_size.x = (float)g_playerWidth;
	a_box.m_size.y = (float)g_playerWidth;
	a_box.m_pos = m_pos;
}

Player::Gun::Gun(const math::Vector2f &a_pos, const math::Vector2f &a_dir) : m_pos(a_pos), m_dir(a_dir) {
	m_dir.Normalize();
}

}