// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "IEventTarget.h"
#include "ViewCore.h"
#include "Model.h"
#include "SpriteView.h"
#include "GameController.h"
#include "FormationView.h"
#include "LevelView.h"
#include "MixerView.h"
#include "Splash.h"

namespace view {
	class ViewManager : public IEventTarget {
		public:
			ViewManager(const model::Model &a_model, HINSTANCE a_appInstance);
			~ViewManager(void);
		
			cmn::String Initiate(IEventTarget &a_target);
			cmn::String Terminate();
			bool Render(float a_dt, IEventTarget &a_target);

			// IEventTarget realization
			void PlayerBulletHit(int a_playerBulletIx);
			void MissileHitEnemy(int a_enemyIx, int a_missileIx);
			void HitEnemy(int a_enemyIx);
			void ResetEnemy(int a_enemyIx);
			void HitPlayer();
			void PlayerFireBullet(int a_gunIx, int a_bulletIx);
			void PlayerFireMissile(int a_missileIx);
			void EnergyPickup();
			void LifePickup();
			void WeaponPickup();
			void StartGame(int a_level, float m_difficulty);
			void BossKilled();

		protected:

			HINSTANCE m_appInstance;
			const model::Model &m_model;
			ViewCore m_core;

			float m_time;
			bool m_doEditSprites;

			View *m_pCurrentView;

			GameController m_gameController;
			Splash m_splashScreen;
			FormationView m_formationEditor;
			SpriteView m_spriteEditor;
			LevelView m_levelEditor;
			MixerView m_mixer;
	};
}
