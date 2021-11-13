// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "View.h"
#include "ViewCore.h"
#include "Model.h"
#include "GameParticles.h"
#include "GameView.h"
#include "Menu.h"



namespace view {

class GameController : public View {
	public:
		
		GameController(const model::Model &a_model, ViewCore &a_core);
		~GameController(void);

		ViewStatus Render(float a_dt, IEventTarget &a_target);

		void MissileHitEnemy(int a_enemyIx, int a_missileIx);
		void HitEnemy(int a_enemyIx);
		void PlayerBulletHit(int a_playerBulletIx);
		void ResetEnemy(int a_enemyIx);
		void HitPlayer();
		void PlayerFireBullet(int a_gunIx, int a_bulletIx);
		void PlayerFireMissile(int a_missileIx);
		void EnergyPickup();
		void LifePickup();
		void WeaponPickup();
		void StartGame(int a_level, float a_difficulty);
		bool OnEsc(IEventTarget &a_target);
		bool ShowingScores() const	{return m_menu.m_mainMenu == Menu::MM_Highscores;}
		bool ShowingCredits() const {return m_menu.m_mainMenu == Menu::MM_Credits;}
		bool ShowingBriefing() const;
		void BossKilled();

	private:
		int GetNextAbility(int a_ability);
		int GetPreviousAbility(int a_ability);
		
		float m_playerGunTimers[model::Player::g_maxGuns];
		float m_enemyHitTimers[model::Model::g_maxEnemies];					// used for visualizing enemy explosions
		float m_playerBulletHitTimers[model::Model::g_maxPlayerBullets];	// used for visualizing player bullet enemy hit animations
		
		float m_abilityChangedTime;		// the time an ability is changed
		float m_playerHitTimer;
		math::Vector3f m_playerBank;	// player ship rotation

		bool m_showEnabledAbility;	// show ability help at end of level
		bool m_showHelp;			// show game help
		GameView::HealthAnimationState m_health;
		GameView::EnergyAnimationState m_energy;
		float m_energyPickUpTime;	// time when an energy pellet was picked up

		GameParticles m_particles;

		static const cmn::WString g_communiques[model::Model::g_maxLevels];
		static const cmn::WString g_abilityHeadlines[model::Player::A_Count - 1];
		static const cmn::WString g_abilityTexts[model::Player::A_Count - 1];
		cmn::String m_playerName;
		float m_time;
		float m_startLevelTimer;
		float m_bossKilledTime;	// do funky rift collapse
		
		Menu m_menu;

		int m_shipType;

		GameView m_gameView;

};

}