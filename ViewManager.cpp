// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include ".\viewmanager.h"

#include <DXRenderer/DX.h>
#include <Scene/Sprite.h>

#include "Bonus.h"


namespace view {

ViewManager::ViewManager(const model::Model &a_model, HINSTANCE a_appInstance)
:m_model(a_model)
,m_appInstance(a_appInstance)
,m_spriteEditor(a_model, m_core)
,m_formationEditor(a_model, m_core)
,m_gameController(a_model, m_core)
,m_levelEditor(a_model, m_core)
,m_mixer(a_model, m_core)
,m_splashScreen(a_model, m_core)
{
	m_time = 0.0f;
	m_doEditSprites = false;
	m_pCurrentView = &m_splashScreen;
}

ViewManager::~ViewManager(void) {
}

cmn::String ViewManager::Initiate(IEventTarget &a_target) {
	cmn::String error(m_core.Initiate(m_appInstance));
	cmn::String adapter(m_core.GetAdapterDescription());
	if (adapter.c_str()[0] != '\0') {
		a_target.SendGPUDescription(adapter.c_str());
	} else {
		LOG_3("No Adapter?!: DX Not Created?!!?");
	}

	return error;
}

cmn::String ViewManager::Terminate()
{

	m_core.Terminate();

	return "";
}

bool ViewManager::Render(float a_dt, IEventTarget &a_target) {
	m_core.m_input.Update(a_dt);
	m_core.m_time += a_dt;

	if (m_core.HasFocus() != true) {
		if (m_model.m_isPaused != true) {
			a_target.SetPause(true);
		}

		m_core.SetMouse(true);
		Sleep(200);
		return true;
	}
#ifndef PUBLIC_RELEASE
	if (m_core.m_input.IsClicked(VK_F1)) {
		m_core.m_assets.SaveMixer();
		m_pCurrentView = &m_gameController;
	}

	if (m_core.m_input.IsClicked(VK_F2)) {
		m_pCurrentView = &m_spriteEditor;
	}

	if (m_core.m_input.IsClicked(VK_F3)) {
		m_pCurrentView = &m_formationEditor;
	}

	if (m_core.m_input.IsClicked(VK_F4)) {
		m_pCurrentView = &m_levelEditor;
	}

	if (m_core.m_input.IsClicked(VK_F6)) {
		m_pCurrentView = &m_mixer;
	}
#endif

	if (m_core.m_input.IsClicked(VK_ESCAPE)) {
		if (m_pCurrentView != &m_gameController) {
			m_pCurrentView = &m_gameController;
		} else {
			const model::Model::GameState state(m_model.GetState());
			if (state == model::Model::GS_Playing) {
				m_gameController.OnEsc(a_target);
			} else if (state == model::Model::GS_WaitingForName && m_model.TrialExpired() != true) {
				a_target.EndGame();
			} else if (state == model::Model::GS_WaitingForNextLevel) {
				a_target.NextLevel();
			} else {
				if (m_gameController.OnEsc(a_target) != true) {
					// quit
					return false;
				}
			}
		}
	}

	m_core.StartFrame();
	
	const View::ViewStatus status(m_pCurrentView->Render(a_dt, a_target));

	switch (status) {
		case View::VS_Done:
		{
			m_pCurrentView = &m_gameController;
		}
		break;
		case View::VS_Reset:
		{
			m_core.EndFrame();
			Terminate();
			m_core.m_assets.m_music.Terminate();
			const cmn::String ret(Initiate(a_target));
			if (ret.size() > 0) {
				LOG_3_A1("Initialization failed: %s", ret.c_str());
				return false;
			} else {
				// reloading is not neccessary since assets.Initate is called again...
				//dxrnd::Resource::ReloadAll();
				return true;
			}
		}
		break;
		case View::VS_Quit:
			m_core.EndFrame();
			return false;
		break;

	}

#ifndef PUBLIC_RELEASE
	{
		cmn::String status;
		status.Format("level %i, wave: %i, time: %f\n bossTimer: %f", m_model.m_levelIx, m_model.m_waveIx, m_model.m_gameTime, m_model.m_bossTimer);
		if (m_model.m_isPaused) {
			status += "\nPaused";
		}
		//m_core.DoLabel(10, 30, g_width - 10, g_height - 10, status, DT_LEFT | DT_TOP);
	}
#endif

	m_core.EndFrame();

	m_core.m_assets.m_music.Update(m_model, m_gameController.ShowingScores(), m_gameController.ShowingCredits(), m_gameController.ShowingBriefing());

	

	return true;
}

void ViewManager::HitEnemy(int a_enemyIx) {
	m_gameController.HitEnemy(a_enemyIx);
}

void ViewManager::PlayerBulletHit(int a_playerBulletIx) {
	m_gameController.PlayerBulletHit(a_playerBulletIx);
}

void ViewManager::ResetEnemy(int a_enemyIx) {
	m_gameController.ResetEnemy(a_enemyIx);
}

void ViewManager::HitPlayer() {
	m_gameController.HitPlayer();
}

void ViewManager::PlayerFireBullet(int a_gunIx, int a_bulletIx) {
	m_gameController.PlayerFireBullet(a_gunIx, a_bulletIx);
}

void ViewManager::EnergyPickup()
{
	m_gameController.EnergyPickup();
}

void ViewManager::LifePickup()
{
	m_gameController.LifePickup();
}

void ViewManager::WeaponPickup()
{
	m_gameController.WeaponPickup();
}

void ViewManager::StartGame(int a_level, float a_difficulty) {
	m_gameController.StartGame(a_level, a_difficulty);
}

void ViewManager::BossKilled() {
	m_gameController.BossKilled();
}

void ViewManager::PlayerFireMissile(int a_missileIx) {
	m_gameController.PlayerFireMissile(a_missileIx);
}

void ViewManager::MissileHitEnemy(int a_enemyIx, int a_missileIx) {
	m_gameController.MissileHitEnemy(a_enemyIx, a_missileIx);
}

}
