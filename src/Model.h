// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "IEventTarget.h"
#include "Enemy.h"
#include "Player.h"
#include "Bullet.h"
#include "HighScores.h"
#include "BallsOfFury.h"
#include "ShockWaveBoss.h"
#include "ProjectXBoss.h"
#include "YellyBoss.h"
#include "DanMakuBoss.h"
#include "FormationController.h"
#include "Bonus.h"
#include "Formation.h"
#include "Wave.h"
#include "Missile.h"
#include "BeamBoss.h"
#include "SnakeBoss.h"
#include "TwinBoss.h"
#include "SquidBoss.h"
#include "FinalBoss.h"
#include "InstrumentationRequest.h"


namespace model {

	class Model : public IEventTarget {
		public:
			Model(IEventTarget &a_target, const cmn::String &a_uid);
			~Model(void);

			typedef enum {
				GS_Starting,
				GS_Playing,
				GS_WaitingForNextLevel,
				GS_GameOver,
				GS_WaitingForName,
				GS_GameCompleted
			} GameState;

			bool Initiate();

			bool Update(float a_dt);

			void SetPause(bool a_doPause);

			GameState GetState() const;
			bool IsTimeForBoss();
			bool IsBossActive() const	{return m_pCurrentBoss != NULL;}
			float GetBossScale() const;

			int GetLoopedLevelIx() const;

			// IEventTarget Realization
			void EndGame();
			void StartGame(int a_level, float a_difficutly);
			void PlayerInput(math::Vector2f a_dPos, bool a_doFire, bool a_useEnergy, int a_ability);
			void NextLevel();
			void EnterName(cmn::String a_name);
			void AddEnemyToFormation(int a_formationIx, int a_enemyType, float a_time);
			void UpdateEnemyInFormation(int a_formationIx, int a_enemyIx, int a_enemyType, float a_time, math::Vector2f a_offset, float a_scale, bool a_hasShield);
			void DeleteEnemyFromFormation(int a_formationIx, int a_enemyIx);
			void CopyFormation(int a_fromFormationIx, int a_toFormationIx);
			void AddWave(int a_levelIx, float a_triggerTime, float a_speedMultiplier, int a_formationIx, int a_controller, const math::Vector2f &a_offset);
			void SetWave(int a_waveIx, float a_triggerTime, float a_speedMultiplier, int a_controller, int a_formationIx, int a_dropWeapon, const math::Vector2f &a_offset);
			void DeleteWave(int a_waveIx);
			void SetPlayerGunPosition(const math::Vector2f a_pos, const int a_gunIx);

			void SendGPUDescription(const char * a_desc);

			// IEventTarget Cheats
			void JumpToLevel(int a_level);
			void SetWeapon(int a_weapon);
			void JumpToBoss();
			void KillPlayer();

			const bool TrialExpired() const;
			const bool IsTrial() const;

			IEventTarget &m_target;

			static const int g_bossKilledBonus = 250000;
			static const int g_maxEnemies = 512;
			static const int g_maxEnemyBullets = 1024;
			static const int g_maxPlayerBullets = 256;
			static const int g_maxBonuses = 16;
			static const int g_maxFormations = 128;
			static const int g_maxWaves = 512;
			static const int g_maxControllers = 32;
			static const int g_maxLevels = 10;
			static const int g_maxMissiles = 64;
			static const int g_maxBosses = 10;
					
			const float m_bossWaitTime;

			FormationController m_controllers[g_maxControllers];
			Player m_player;
			
			IBoss *m_pCurrentBoss; // one of the objects below...
			float m_bossTimer;
			ShockWaveBoss m_shockwave;
			BallsOfFury m_fury;
			ProjectXBoss m_pxboss;
			YellyBoss m_yelly;
			DanMakuBoss m_danMaku;
			BeamBoss m_beamBoss;
			SnakeBoss m_snakeBoss;
			TwinBoss m_twinBoss;
			SquidBoss m_squidBoss;
			FinalBoss m_finalBoss;

			Enemy m_enemies[g_maxEnemies];
			Bullet m_enemyBullets[g_maxEnemyBullets];
			Bullet m_playerBullets[g_maxPlayerBullets];
			Bonus m_bonuses[g_maxBonuses];
			Formation m_formations[g_maxFormations];
			Wave m_waves[g_maxWaves];
			Missile m_missiles[g_maxMissiles];
			int m_waveIx;
			bool m_isPaused;

			HighScores m_scores;

			float m_levelTime;	// is reset when a new level is started and not updated while paused
			float m_gameTime;	// the total time played in a gaming session
			float m_totalTime;	// total time the game has run
			int m_levelIx;
			int m_loopCount;	// how many times the game has looped
			float m_slowmoTimer;
			float m_trialExpirationTimer; // when missiles was fired used for trial

			int m_continues;		// how many times the game can continue on an unlocked level
			int m_unlockedLevels;	// the unlocked levels
			bool m_isAgressive;		// shows if the player has begun the agression
			int m_serverVersion;	// -1 == error, 0 == working, > 0 version number
			float m_difficulty;		// 0.5f, 1.0f, 1.5f

		private:
			
			HttpRequest m_versionRequest;
			InstrumentationRequest m_instrumentationRequest;

			bool HasEnemyInFormation() const;
			bool IsNextLevel() const;
			bool m_hasFormationChanged;	// for knowing if to save the formations
			bool m_hasWavesChanged;		// for knowing if to save the waves
			int GetNextFreeEnemy();
			int GetNextFreePlayerBullet();
			bool UpdateEnemies(float a_dt);
			void UpdateBonuses(float a_dt);
			void UpdateBullets(float a_dt);
			bool UpdateBullet(Bullet &a_bullet, float a_dt);
			void UpdateMissiles(float a_dt);
			void InitiateWave();
			bool IsLooping() const;		// For knowing if we are to loop back to the first enemy when playing in the background

			void HitEnemy(int a_enemyIx, int a_damage);
			void HitPlayer();
			void UpdateVersionRequest();

	};
}
