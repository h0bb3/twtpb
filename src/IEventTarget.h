// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

//#define PUBLIC_RELEASE
#define GAMERS_GATE_FULL_VERSION
#define FOUR_PLAYER_VERSION

#ifdef KOMPUTER_SWIAT_VERSION
	#define NO_INSTRUMENTATION	// no instrumentation data sent
	#define NO_UPDATE			// no version check/update
	const int g_version = 1029;	// should be increased for every iteration, script att game_scripts/GetGameVersion.php
#else 
	const int g_version = 32;	// should be increased for every iteration, script att game_scripts/GetGameVersion.php
#endif

#ifdef GAMERS_GATE_FULL_VERSION
	#define NO_INSTRUMENTATION	// no instrumentation data sent
	#define NO_UPDATE			// no version check/update
	#define NO_PROTECTION		// full version on build not to be protected by armadillo
#endif



const int g_width = 800;
const int g_height = 600;
const int g_playerWidth = 7;
const int g_playerHeight = 11;
const int g_bulletSize = 4;

class IEventTarget {
	public:
		IEventTarget(void);
		virtual ~IEventTarget(void);

		// game input
		virtual void EndGame() {};
		virtual void PlayerInput(math::Vector2f a_dPos, bool a_doFire, bool a_useEnergy, int a_ability) {};
		virtual void NextLevel() {};
		virtual void EnterName(cmn::String a_name) {};
		virtual void SetPause(bool a_doPause) {};
		virtual void StartGame(int a_level, float a_difficutly) {};
		virtual void SetPlayerGunPosition(const math::Vector2f a_pos, const int a_gunIx) {};
		

		// cheats
		virtual void JumpToLevel(int a_level) {};
		virtual void SetWeapon(int a_weapon) {};
		virtual void JumpToBoss() {};
		virtual void KillPlayer() {};

		// Instrumentation
		virtual void SendGPUDescription(const char * a_desc) {};

		// effects
		virtual void HitEnemy(int a_enemyIx) {};
		virtual void MissileHitEnemy(int a_enemyIx, int a_missileIx) {};
		virtual void PlayerBulletHit(int a_playerBulletIx) {};
		virtual void ResetEnemy(int a_enemyIx) {};
		virtual void HitPlayer() {};
		virtual void PlayerFireBullet(int a_gunIx, int a_bulletIx) {};
		virtual void PlayerFireMissile(int a_missileIx) {};
		virtual void EnergyPickup() {};
		virtual void LifePickup() {};
		virtual void WeaponPickup() {};
		virtual void BossKilled() {};

		// Formation editing
		virtual void AddEnemyToFormation(int a_formationIx, int a_enemyType, float a_time) {};
		virtual void UpdateEnemyInFormation(int a_formationIx, int a_enemyIx, int a_enemyType, float a_time, math::Vector2f a_offset, float a_scale, bool a_hasShield) {};
		virtual void DeleteEnemyFromFormation(int a_formationIx, int a_enemyIx) {};
		virtual void CopyFormation(int a_fromFormationIx, int a_toFormationIx) {};

		// Wave editing
		virtual void AddWave(int a_levelIx, float a_triggerTime, float a_speedMultiplier, int a_formationIx, int a_controller, const math::Vector2f &a_offset) {};
		virtual void SetWave(int a_waveIx, float a_triggerTime, float a_speedMultiplier, int a_controller, int a_formationIx, int a_dropWeapon, const math::Vector2f &a_offset) {};
		virtual void DeleteWave(int a_waveIx) {};
};
