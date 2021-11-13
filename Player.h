// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include <math/AABoundingBox.h>
#include "RandomTable.h"

namespace model {
class Player {
	public:
		Player(void);
		virtual ~Player(void);

		void Update(float a_dt);
		bool DoFire(int a_level);
		int DoFireMissiles(int a_level);
		bool Damage(int a_amount, float a_gameTime);
		void Init(int a_startingWeapon, int a_lives);
		bool IsActive(float a_gameTime) const;
		void NextLevel();
		void RestartLevel();

		void GetBox(math::AABoundingBox2D &a_box) const;

		

		enum Ability {
			A_None = 0,
			A_RapidFire,
			A_Regeneration,
			A_SlowMotion,
			A_Missiles,
			A_Count
		};
		struct AbilityInfo {
			AbilityInfo(int a_cost, int a_level) : m_cost(a_cost), m_level(a_level) {};
			const int m_cost;
			const int m_level;	// from which level should the ability be present
		};
		const static AbilityInfo g_abilities[A_Count];

		bool CanUseAbility(Ability a_ability, int a_levelIx) const;
		bool IsUsingAbility(Ability a_ability, int a_levelIx) const;

		math::Vector2f m_pos;
		math::Vector2f m_movement;

		const static int g_defaultEnergy = 100;
		const static int g_defaultShield = 100;
		const static int g_maxGuns = 8;

		class Gun {
			public:
				Gun() {};
				Gun(const math::Vector2f &a_position, const math::Vector2f &a_direction);

				math::Vector2f m_pos;
				math::Vector2f m_dir;	// normalized
		};
		Gun m_guns[g_maxGuns];

		class ProtectedInt {
			public:
				ProtectedInt() {
					m_pValue = new int();
					m_offset = rand() % g_randomTableCount;
					SetChecksum(*m_pValue);
				};
				ProtectedInt(const int &a_value) {
					m_pValue = new int(a_value);
					m_offset = rand() % g_randomTableCount;
					SetChecksum(a_value);
				};
				ProtectedInt(const ProtectedInt &a_pi);
				~ProtectedInt() { delete m_pValue; m_pValue = NULL; };

				bool Check() const {
					return m_checksum == CalcChecksum(*m_pValue);
				}

				int operator ++ () {
					SetChecksum(*m_pValue + 1);
					(*m_pValue)++;
					return *m_pValue;
				};
				int operator += (const int &a_int) {
					SetChecksum(*m_pValue + a_int);
					*m_pValue += a_int;
					return *m_pValue;
				};
				int operator + (const int &a_int) {
					SetChecksum(*m_pValue + a_int);
					*m_pValue += a_int;
					return *m_pValue;
				};
				const ProtectedInt& operator = (const int &a_int) {
					SetChecksum(a_int);
					*m_pValue = a_int;
					if (m_checksum != CalcChecksum(*m_pValue)) {
						LOG_3("dsad");
					}
					return *this;
				};
				operator int () const { return *m_pValue; };

				int GetChecksum() const { return m_checksum; };
				int GetOffset() const { return m_offset; };


			protected:
				int CalcChecksum(const int a_value) const {
					const int ix(abs(a_value + m_offset) % g_randomTableCount);
					return g_randomTable[ix];
				}
				void SetChecksum(const int a_value) {
					m_offset = rand() % g_randomTableCount;
					m_checksum = CalcChecksum(a_value);
				};

				int *m_pValue;
				int m_checksum;
				int m_offset;
		};


		math::Vector2f m_dPos;	// cashed input reset on input
		bool m_doFire;			// cashed input
		bool m_useEnergy;		// cashed input

		Ability m_ability;

		ProtectedInt m_score;
		ProtectedInt m_multiplier;

		int m_life;
		int m_shield;
		int m_maxShield;
		int m_weapon;
		int m_baseWeapon;	// the weapon to reset to
		int m_energy;
		int m_maxEnergy;

		float m_regenerationTimer;	// timer for regeneration ability
		float m_lifeLostTime;	// when in game time a life was lost

		float m_fireTime;
		float m_fireTimer;

	private:

};
}
