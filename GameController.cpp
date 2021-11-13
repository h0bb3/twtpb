// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include ".\GameController.h"

#include "BarSprite.h"
#include "Strings.h"

#include <Scene/Sprite.h>

namespace view {

const cmn::WString GameController::g_communiques[model::Model::g_maxLevels] = {
	LEVEL_0, LEVEL_1, LEVEL_2, LEVEL_3, LEVEL_4, LEVEL_5, LEVEL_6, LEVEL_7, LEVEL_8, LEVEL_9
};


const cmn::WString GameController::g_abilityHeadlines[model::Player::A_Count - 1] = {
	ABILITY_RAPID_FIRE_HEADLINE, ABILITY_REGENERATION_HEADLINE, ABILITY_TIME_WARP_HEADLINE, ABILITY_MISSILES_HEADLINE
};

const cmn::WString GameController::g_abilityTexts[model::Player::A_Count - 1] = {
	ABILITY_RAPID_FIRE, ABILITY_REGENERATION, ABILITY_TIME_WARP, ABILITY_MISSILES
};

GameController::GameController(const model::Model &a_model, ViewCore &a_core) : View(a_model, a_core), m_playerBank(0.0f, 0.0f, 0.0f), m_gameView(a_core), m_menu(a_core) {
	m_showEnabledAbility = true;
	m_showHelp = false;
	StartGame(0, 1.0f);
	m_time = 60.0f;	// offset to avoid premature effects
	m_shipType = 0;
}

GameController::~GameController(void) {
}

GameController::ViewStatus GameController::Render(float a_dt, IEventTarget &a_target) {

	const model::Model::GameState state(m_model.GetState());

	math::Vector2f mpos;

	m_core.m_input.GetCursorPos(mpos);

#ifndef PUBLIC_RELEASE
	{	// cheats
		for (char c = '0'; c <= '9'; c++) {
			if (m_core.m_input.IsClicked(c)) {
				a_target.SetWeapon(c - '0');
				a_target.JumpToLevel(c - '0');
			}
		}

		if (m_core.m_input.IsClicked(VK_ADD)) {
			a_target.SetWeapon(m_model.m_player.m_weapon + 1);
		}
		if (m_core.m_input.IsClicked(VK_SUBTRACT)) {
			a_target.SetWeapon(m_model.m_player.m_weapon + 1);
		}
		if (m_core.m_input.IsClicked('B')) {
			a_target.JumpToBoss();
		}
		if (m_core.m_input.IsClicked('K')) {
			a_target.KillPlayer();
		}

	}
#endif

	float bossTimer(0.0f);
	if (m_model.m_isPaused) {
		bossTimer = 1.0f;
	} else if (m_model.m_pCurrentBoss != NULL) {
		bossTimer = m_model.GetBossScale();
		if (m_model.m_pCurrentBoss->GetTimeLeft() <= 0.5f) {	// 0.5 to make sure the bg is not scrolled if the boss times out
			bossTimer = 1.0f;
		}
	}

	m_core.UpdateAndDrawBackground(a_dt, bossTimer, m_model.m_player.m_pos);

	m_gameView.InitFrame();

	if (m_model.m_pCurrentBoss != NULL) {
		m_gameView.RenderRift(m_time, m_bossKilledTime, m_model.m_bossTimer, m_model.m_pCurrentBoss->IsAlive(), m_model.m_isAgressive);
	}

	m_gameView.RenderGlowLayer(m_model, m_time, m_enemyHitTimers, m_playerBulletHitTimers);

	math::Matrix44f playerWorld;
	{
		
		math::Matrix44f scale;
		math::Matrix44f rot;
		const float s(0.18f);

		scale.Scale(math::Vector3f(s, s, s));

		m_playerBank.x -= m_model.m_player.m_movement.y * 0.002f;
		if (m_playerBank.x < -PI_QUARTER * 0.25f) {
			m_playerBank.x = -PI_QUARTER * 0.25f;
		} else if (m_playerBank.x > PI_QUARTER * 0.25f) {
			m_playerBank.x = PI_QUARTER * 0.25f;
		}

		m_playerBank.y -= m_model.m_player.m_movement.x * 0.01f;
		if (m_playerBank.y < -PI_QUARTER) {
			m_playerBank.y = -PI_QUARTER;
		} else if (m_playerBank.y > PI_QUARTER) {
			m_playerBank.y = PI_QUARTER;
		}
		rot.Rotation(m_playerBank);

		m_playerBank.y -= m_playerBank.y * a_dt * 2.0f;
		m_playerBank.x -= m_playerBank.x * a_dt;

		if (m_model.m_player.IsActive(m_model.m_levelTime) != true) {
			m_gameView.RenderPlayerDeath(m_model.m_player.m_pos, m_time, m_playerHitTimer);

		} else if (m_model.m_player.m_life > 0 && state == model::Model::GS_Playing) {

#ifdef FOUR_PLAYER_VERSION
			if (m_core.m_input.IsPressedVKRaw('Y') == true) {
				m_shipType = 0;
			} else if (m_core.m_input.IsPressedVKRaw('U') == true) {
				m_shipType = 1;
			} else if (m_core.m_input.IsPressedVKRaw('I') == true) {
				m_shipType = 2;
			} else if (m_core.m_input.IsPressedVKRaw('O') == true) {
				m_shipType = 3;
			}
#endif

			for (int i = 0; i < model::Player::g_maxGuns; i++) {
				math::Vector3f gunPos(m_core.m_assets.m_playerData[m_shipType].m_gunMounts[i]);
				gunPos.TransformRot(rot);
				gunPos *= s;
				gunPos += math::Vector3f(0.0f, 0.2f, 0.0f);
				const math::Vector2f pos(m_gameView.Ship2World(gunPos));
				a_target.SetPlayerGunPosition(pos, i);
			}
				
			playerWorld.Translation(m_gameView.World2Ship(m_model.m_player.m_pos) + math::Vector3f(0.0f, 0.2f, 0.0f));
			playerWorld = rot * scale * playerWorld;

			float dt(m_time - m_playerHitTimer);
			if (m_model.m_levelTime - m_model.m_player.m_lifeLostTime > 0 && dt < 0.25f) {
				m_gameView.DrawPlayer(m_model.m_player, m_playerGunTimers, m_particles, m_time, rot, playerWorld, 1.0 - (dt * 4.0f), a_dt, m_shipType);
			} else {
				m_gameView.DrawPlayer(m_model.m_player, m_playerGunTimers, m_particles, m_time, rot, playerWorld, 0.0f, a_dt, m_shipType);
			}
		}
	}

	static float missileParticleTimer = 0.0f;
	missileParticleTimer -= a_dt;
	bool missileParticlesAdded(false);

	for (int mIx = 0; mIx < model::Model::g_maxMissiles; mIx++) {
		if (m_model.m_missiles[mIx].IsAlive() && missileParticleTimer < 0.0f) {
			BeamParticle *pPart(m_particles.FindFreeBeamParticle(m_time));
			if (pPart != NULL) {
				missileParticlesAdded = true;
				pPart->m_bornTime = m_time;
				pPart->m_speed.Set(0.0f, 0.0f, 0.0f);
				pPart->m_startPos = m_gameView.World2Ship(m_model.m_missiles[mIx].m_pos);
				pPart->m_relativeEndPos = m_gameView.World2Ship(m_particles.m_missileOldPositions[mIx]) - pPart->m_startPos;
				m_particles.m_missileOldPositions[mIx] = m_model.m_missiles[mIx].m_pos;
				pPart->m_startSize.Set(0.2f, 0.05f);
				pPart->m_endSize.Set(0.2f, 0.05f);
				pPart->m_ttl = 0.5f;
			}
		}
	}

	if (missileParticlesAdded) {
		// this value could be part of a details setting...
		if (m_core.m_assets.m_levelOfDetail == 0) {
			missileParticleTimer = 0.03f;
		} else if (m_core.m_assets.m_levelOfDetail == 1) {
			missileParticleTimer = 0.02f;
		} else {
			missileParticleTimer = 0.01f;
		}
	}

	m_gameView.RenderParticleEffects(m_particles, playerWorld, m_time);



	cmn::String status;
	model::Player::Ability playerAbility(m_model.m_player.m_ability);
	math::Vector2f dMouse(m_core.GetPlayerMovement(a_dt));


	bool doFire(m_core.DoFire()), doAbility(m_core.DoUseAbility());
	bool doSetAbility[model::Player::A_Count] = {false, false, false, false, false};
	m_core.DoSetAbility(doSetAbility);
	
	dMouse.x *= g_width;
	dMouse.y *= g_height;

	m_gameView.SetNormalRenderState();
	

	if (m_model.m_isPaused == true) {

		const Menu::MenuStatus menuStatus(m_menu.DoMenu(a_dt, m_model));

		switch (menuStatus) {
			case Menu::MS_Reset:
				m_gameView.EndFrame();
				return VS_Reset;
			break;
			case Menu::MS_StartGame:
				a_target.StartGame(m_menu.m_level, m_menu.m_difficulty);
				a_target.PlayerInput(dMouse, true, false, m_model.m_player.m_ability);
			break;
			case Menu::MS_QuitGame:
				a_target.EndGame();
			break;
			case Menu::MS_ContinueGame:
				a_target.SetPause(false);
			break;
			case Menu::MS_Quit:
				m_gameView.EndFrame();
				return VS_Quit;
			break;
			default:
			break;
		};

	} else {

		switch (state) {
			case model::Model::GS_WaitingForNextLevel:
			{
				if (ShowingBriefing()) {	// wait some time
					m_core.SetMouse(true);
					m_core.DrawBriefingBackground(m_time);

					if (m_model.m_levelIx < 3 && m_model.m_loopCount == 0 && m_showEnabledAbility == true) {
						cmn::WString str(g_abilityTexts[m_model.m_levelIx + 1]);

						str.Replace(L"%kf", m_menu.VKToString(m_core.m_assets.m_kbdFire));
						str.Replace(L"%ke", m_menu.VKToString(m_core.m_assets.m_kbdAbility));
						str.Replace(L"%k1", m_menu.VKToString(m_core.m_assets.m_kbdSetRapidFire));
						str.Replace(L"%k2", m_menu.VKToString(m_core.m_assets.m_kbdSetRegeneration));
						str.Replace(L"%k3", m_menu.VKToString(m_core.m_assets.m_kbdSetTimeWarp));
						str.Replace(L"%k4", m_menu.VKToString(m_core.m_assets.m_kbdSetMissiles));

						const cmn::String or(" or ");
						str.Replace(L"%pf", m_core.m_assets.GetPadButtonName(or, m_core.m_assets.m_padFire));
						str.Replace(L"%pe", m_core.m_assets.GetPadButtonName(or, m_core.m_assets.m_padAbility));
						str.Replace(L"%p1", m_core.m_assets.GetPadButtonName(or, m_core.m_assets.m_padSetRapidFire));
						str.Replace(L"%p2", m_core.m_assets.GetPadButtonName(or, m_core.m_assets.m_padSetRegeneration));
						str.Replace(L"%p3", m_core.m_assets.GetPadButtonName(or, m_core.m_assets.m_padSetTimeWarp));
						str.Replace(L"%p4", m_core.m_assets.GetPadButtonName(or, m_core.m_assets.m_padSetMissiles));

						static char lmbState = 0;
						if (m_core.IsButtonUpDownUp(m_core.m_input.IsPressedVKRaw(m_core.m_assets.m_mouseFire) || doFire, lmbState)) {
							m_showEnabledAbility = false;
							m_startLevelTimer = m_time;
						}
						m_core.DoHeadline(0, 150, g_width, g_height - 150, g_abilityHeadlines[m_model.m_levelIx + 1], DT_CENTER | DT_TOP);
						m_core.DoLabel(0, 200, g_width, g_height - 150, str, DT_CENTER | DT_TOP);

						math::Vector2f pos(g_width * 0.5f * m_core.m_scale.x - 64 * m_core.m_scale.x, 300 * m_core.m_scale.y);
						m_gameView.RenderBriefingAbilityIcon(pos, m_model.m_levelIx);

						break;
					} else if (m_showHelp != true) {
						const float dt(m_time - m_startLevelTimer);
						const int count((int)(dt / 0.025f));
						cmn::WString str(g_communiques[(m_model.m_levelIx + 1) % m_model.g_maxLevels]);

						str = str.substr(0, count);

						m_core.DoHeadline(0, 150, g_width, g_height - 150, MESSAGE_HEADLINE, DT_CENTER | DT_TOP);
						m_core.DoLabel(0, 0, g_width, g_height, str, DT_CENTER | DT_VCENTER);
					} else {
						//cmn::String str("XZ-75fh Imperial Fighter/Hunter Controls\n\n\nUse the mouse to control your ship.\n\nUse the Left Mouse Button to fire, you have an infinite amount of bullets.\n\nUse the Mouse Wheel or 'a' and 'd' keys to toggle your Active Ability.\n\nUse the Right Mouse Button to activate the Active Ability, this will consume energy as indicated by the blue energy bar\n\nKeep an eye on the status bar at the top of the screen, it will show your Active Ability, Shield Status and Score.\n\n\nKilling complete waves of enemies will release their energy in the form of an energy diamond,\ncollect the diamonds to increase your score and to replenish your energy.\n\nSome enemies release weapon upgrades in the form of a triangle, collect to boost your weapons.");
						//m_core.DoLabel(0, 0, g_width, g_height, str, DT_CENTER | DT_VCENTER);
					}

					{
						static char lmbState = 0;
						/*if (m_core.DoButton(g_width  - 170, g_height - 40, 140, 20, "Ship Controls", m_showHelp) == ViewCore::WS_Clicked) {
							m_showHelp = !m_showHelp;
							lmbState = 0;
						}*/
						if (m_showHelp == false && m_core.IsButtonUpDownUp(doFire, lmbState)) {
							const float dt(m_time - m_startLevelTimer);
							const int count((int)(dt / 0.025f));
							if (count < g_communiques[(m_model.m_levelIx + 1) % m_model.g_maxLevels].length()) {
								m_startLevelTimer = -100.0f;
								lmbState = 0;
							} else {
								a_target.NextLevel();
								m_showHelp = false;
								m_showEnabledAbility = true;
								lmbState = 0;
							}
						}
					}
				}
			}
			break;
			case model::Model::GS_Starting:
			{
				const Menu::MenuStatus menuStatus(m_menu.DoMenu(a_dt, m_model));

				switch (menuStatus) {
					case Menu::MS_Reset:
						m_gameView.EndFrame();
						return VS_Reset;
					break;
					case Menu::MS_StartGame:
						a_target.StartGame(m_menu.m_level, m_menu.m_difficulty);
						a_target.PlayerInput(dMouse, true, false, m_model.m_player.m_ability);
					break;
					case Menu::MS_QuitGame:
						a_target.EndGame();
					break;
					case Menu::MS_ContinueGame:
						a_target.SetPause(false);
					break;
					case Menu::MS_Quit:
						m_gameView.EndFrame();
						return VS_Quit;
					break;
					default:
					break;
				};

			}
			break;
			case model::Model::GS_Playing:
			{
				m_core.SetMouse(false);
				const int wheelStatus(m_core.GetMouseWheelStatus());

				int ability((int)playerAbility);

				if (m_core.m_input.IsPressedVK(m_core.m_assets.m_kbdPrev)) {
					ability = GetPreviousAbility(ability);
				}
				if (m_core.m_input.IsPressedVK(m_core.m_assets.m_kbdNext)) {
					ability = GetNextAbility(ability);
				}

				for (int aIx = 0; aIx < model::Player::A_Count; aIx++) {
					if (doSetAbility[aIx] && m_model.m_player.CanUseAbility((model::Player::Ability)aIx, m_model.m_levelIx + m_model.g_maxLevels * m_model.m_loopCount)) {
						ability = aIx;
					}
				}

				
				if (wheelStatus > 0) {
					ability = GetNextAbility(ability);
				} else if (wheelStatus < 0) {
					ability = GetPreviousAbility(ability);
				}
				
				if (ability != playerAbility) {
					if (ability < 1) {
						ability = 1;
					}
					if (ability >= (int)model::Player::A_Count) {
						ability = (int)model::Player::A_Count - 1;
					}
					m_abilityChangedTime = m_time;
					playerAbility = (model::Player::Ability)ability;
				}


				float healthPercent(((float)m_model.m_player.m_shield / (float)m_model.m_player.m_maxShield));
				if (m_model.m_player.IsActive(m_model.m_levelTime) != true) {
					healthPercent = 0;
				}

				m_gameView.AddPlayerHealth(healthPercent, m_health, m_playerHitTimer, m_time, a_dt);

				float energyPercent((float)m_model.m_player.m_energy / (float)m_model.m_player.m_maxEnergy);
				if (m_model.m_player.IsActive(m_model.m_levelTime) != true) {
					energyPercent = 0;
				}
				m_gameView.AddPlayerEnergy(energyPercent, m_energy, m_energyPickUpTime, m_time, a_dt);
				

				if (m_model.IsBossActive() && m_model.m_bossTimer < 0.0f && m_model.m_pCurrentBoss->IsAlive() && m_model.m_pCurrentBoss->GetTimeLeft() > 0.0f) {
					m_gameView.AddBossStatus(m_model.m_pCurrentBoss->GetLifeLeft(), m_model.m_pCurrentBoss->GetTimeLeft());
				}
				

				//status.Format("lives: %i | score: %i", m_model.m_player.m_life, m_model.m_player.m_score);
				//m_core.DoLabel(0, 32, g_width, 32, status, DT_CENTER | DT_TOP);

				if (m_model.m_pCurrentBoss != NULL && m_model.m_bossTimer < 5.0f && m_model.m_bossTimer > 1.0f) {
					m_gameView.RenderBossWarning(m_model.m_bossTimer, m_time);					
				}

				m_gameView.RenderHUD(m_model.m_player.m_multiplier, m_model.m_player.m_score, m_model.m_player.m_life);
				m_gameView.RenderAbilityIcons(m_model.m_player, m_time, m_abilityChangedTime, m_model.GetLoopedLevelIx());

				a_target.PlayerInput(dMouse, doFire, doAbility, playerAbility);

			}
			break;
			case model::Model::GS_WaitingForName:
			{
				m_core.DrawStaticBackground(m_time);

				if (m_model.TrialExpired()) {
					m_core.DoHeadline(0, 140, g_width, 40, DEMO_HEADLINE, DT_CENTER | DT_TOP);
					m_core.DoLabel(0, 180, g_width, g_height - 180, DEMO_TEXT, DT_CENTER | DT_TOP);
					if (m_menu.DoBuyButton( 430)) {
						a_target.EndGame();
						m_gameView.EndFrame();
						return View::VS_Reset;
					}
					if (m_model.m_levelTime - m_model.m_trialExpirationTimer - 10.0f < 10.0f) {
						cmn::WString str;
						str.Format(DEMO_NO_THANKS_TIME, 10 - (int)(m_model.m_levelTime - m_model.m_trialExpirationTimer - 10.0f));
						m_core.DoInactiveButton(g_width / 2 - 70, 500, 140, 25, str);
					} else {
						if (m_menu.DoHotMenuButton(g_width / 2 - 70, 500, 140, 25, DEMO_NO_THANKS, false) == ViewCore::WS_Clicked) {
							a_target.EndGame();
						}
					}
					m_core.SetMouse(true);
				} else {

					cmn::WString status;
					const cmn::WString name(m_playerName);
					//if (m_model.IsTrial()) {			
					//	status.Format(HIGHSCORE_NOT_REGISTERED, name.c_str());
					//} else {
						status.Format(HIGHSCORE, name.c_str());
					//}

					

					const float width(32.0f * m_core.m_scale.x), height(32.0f * m_core.m_scale.y);
					cmn::String score;
					score.ToString((int)m_model.m_player.m_score);
					math::Vector2f pos;
					pos.Set((float)(g_width) * m_core.m_scale.x * 0.5f - score.size() * width * 0.5f, (float)300.0f * m_core.m_scale.y - height * 2.0f);
					m_gameView.RenderScore(pos, score, width, height);

					m_core.DoLabel(10, 10, g_width - 10, g_height - 10, status, DT_CENTER | DT_VCENTER);

					if (m_core.m_input.IsClicked(VK_RETURN)) {
						m_menu.m_startScore = m_model.m_scores.GetPlace(m_model.m_player.m_score);
						a_target.EnterName(m_playerName);
						if (m_menu.m_startScore >= 0) {
							m_menu.m_mainMenu = Menu::MM_Highscores;
						}
					}
					if (m_core.m_input.IsPressedVK(VK_BACK) && m_playerName.size() > 0) {
						m_playerName.erase(m_playerName.size() - 1, 1);
					}

					if (m_playerName.size() < 32) {
						for (char c = 'a'; c <= 'z'; c++) {
							if (m_core.m_input.IsPressedASCII(c)) {
								m_playerName.append(1, c);
							}
						}

						for (char c = 'A'; c <= 'Z'; c++) {
							if (m_core.m_input.IsPressedASCII(c)) {
								m_playerName.append(1, c);
							}
						}

						for (char c = '0'; c <= '9'; c++) {
							if (m_core.m_input.IsPressedASCII(c)) {
								m_playerName.append(1, c);
							}
						}
					}
				}
			}
			break;
			case model::Model::GS_GameOver:
			{
				cmn::WString status;
				if (m_model.IsTrial()) {
					status.Format(GAME_OVER_NOT_REGISTERED, m_model.m_player.m_score);
				} else {
					status.Format(GAME_OVER, m_model.m_player.m_score);
				}

				static char lmbState(0);
				bool clicked(false);

				if (m_core.IsButtonUpDownUp(doFire, lmbState)) {
					clicked = true;
					lmbState = 0;
				}
				
				m_core.DoLabel(0, 0, g_width, g_height, status, DT_CENTER | DT_VCENTER);
				a_target.PlayerInput(dMouse, clicked, false, playerAbility);
			}
			break;
			case model::Model::GS_GameCompleted:
			{
				static float yScroll = 610.0f;
				static char lmbState = 0;

				m_core.DoHeadline(100.0f, yScroll, (float)g_width - 200, (float)g_height, EPILOGUE_HEADLINE, DT_CENTER | DT_TOP); 
				m_core.DoLabel(100.0f, yScroll + 30.0f, (float)g_width - 200, (float)g_height, EPILOGUE_TEXT, DT_CENTER | DT_TOP);
				m_menu.DrawCredits(yScroll + 200.0f, m_time);
				yScroll -= a_dt * 20.0f;
				if (yScroll < -600.0f) {
					yScroll = 610.0f;
				}

				if (m_core.IsButtonUpDownUp(doFire, lmbState)) {
					lmbState = 0;
					a_target.EndGame();
					yScroll = 0;
				}
			}
			break;
		};
	}
	m_time += a_dt;

	m_gameView.EndFrame();
	return VS_Ok;
}

int GameController::GetNextAbility(int a_ability) {
	for (int i = a_ability + 1; i < model::Player::A_Count; i++) {
		if (m_model.m_player.CanUseAbility((model::Player::Ability)i, m_model.GetLoopedLevelIx())) {
			return i;
		}
	}

	return a_ability;
}

int GameController::GetPreviousAbility(int a_ability) {
	for (int i = a_ability - 1; i > 0; i--) {
		if (m_model.m_player.CanUseAbility((model::Player::Ability)i, m_model.GetLoopedLevelIx())) {
			return i;
		}
	}

	return a_ability;
}

void GameController::HitEnemy(int a_enemyIx) {

	if (m_model.m_enemies[a_enemyIx].IsAlive() != true) {
		m_enemyHitTimers[a_enemyIx] = m_time;
		if (m_model.m_enemies[a_enemyIx].m_waveIx < 0) {
			m_core.m_assets.m_sfx.mySounds[SoundBox::S_PlayerExplode].Play(m_core.m_assets.m_sfx.m_volume);
		} else {
			m_core.m_assets.m_sfx.mySounds[SoundBox::S_EnemyExplode].Play(m_core.m_assets.m_sfx.m_volume);
		}
		
	} else if (m_model.m_enemies[a_enemyIx].m_hasShield != true) {

		m_core.m_assets.m_sfx.mySounds[SoundBox::S_EnemyDamage].Play(m_core.m_assets.m_sfx.m_volume);
	}
}

void GameController::PlayerBulletHit(int a_playerBulletIx) {
	m_playerBulletHitTimers[a_playerBulletIx] = m_time;
}

void GameController::ResetEnemy(int a_enemyIx) {
//	if (m_time - m_enemyHitTimers[a_enemyIx] < 2.0f) {
//		LOG_3("effect stolen!");
//	}
	m_enemyHitTimers[a_enemyIx] = 0.0f;
}

void GameController::HitPlayer() {
	m_playerHitTimer = m_time;
	if (m_model.m_player.IsActive(m_model.m_levelTime) != true) {
		m_core.m_assets.m_sfx.mySounds[SoundBox::S_PlayerExplode].Play(m_core.m_assets.m_sfx.m_volume);
	}
	else
	{
		m_core.m_assets.m_sfx.mySounds[SoundBox::S_PlayerHit].Play(m_core.m_assets.m_sfx.m_volume);
	}
}

void GameController::PlayerFireBullet(int a_gunIx, int a_bulletIx) {
	m_playerGunTimers[a_gunIx] = m_time;

	if (a_gunIx == 0) {
		m_core.m_assets.m_sfx.mySounds[SoundBox::S_PlayerShoot].Play(m_core.m_assets.m_sfx.m_volume);
	}

}

void GameController::EnergyPickup() {
	m_energyPickUpTime = m_time;
	m_core.m_assets.m_sfx.mySounds[SoundBox::S_EnergyPickup].Play(m_core.m_assets.m_sfx.m_volume);
}

void GameController::LifePickup() {
//	m_core.m_assets.m_sfx.mySounds[SoundBox::S_LifePickup].Play(m_core.m_assets.m_sfx.m_volume);
}

void GameController::WeaponPickup() {
	m_core.m_assets.m_sfx.mySounds[SoundBox::S_WeaponPickup].Play(m_core.m_assets.m_sfx.m_volume);
}

void GameController::StartGame(int a_level, float a_difficulty) {
	m_playerHitTimer = 0;
	if (a_level == 0) {
		m_energyPickUpTime = 0.0f;
		m_health.Reset();
		m_energy.Reset();
		for (int eIx = 0; eIx < model::Model::g_maxEnemies; eIx++) {
			m_enemyHitTimers[eIx] = 0.0f;
		}
		for (int bIx = 0; bIx < model::Model::g_maxPlayerBullets; bIx++) {
			m_playerBulletHitTimers[bIx] = 0.0f;
		}
		for (int gIx = 0; gIx < model::Player::g_maxGuns; gIx++) {
			m_playerGunTimers[gIx] = 0.0f;
		}
	}

	m_startLevelTimer = m_time;
}

void GameController::BossKilled() {
	m_bossKilledTime = m_time;
}

bool GameController::OnEsc(IEventTarget &a_target) {

	if (m_showHelp == true) {
		m_showHelp = false;
		return true;
	}

	if (m_menu.m_gfxOptions != Menu::GO_None) {
		m_menu.m_gfxOptions = Menu::GO_None;
		return true;
	}

	if (m_menu.m_inputOptions != Menu::IO_None) {
		if (m_menu.m_gamePadInput != Menu::GPI_None) {
			m_menu.m_gamePadInput = Menu::GPI_None;
			return true;
		}
		if (m_menu.m_kbdInput != Menu::KI_None) {
			m_menu.m_kbdInput = Menu::KI_None;
			return true;
		}
		m_menu.m_inputOptions = Menu::IO_None;
		return true;
	}

	if (m_menu.m_options != Menu::O_None) {
		m_menu.m_hotButton = m_menu.m_options;
		m_menu.m_options = Menu::O_None;
		return true;
	}

	if (m_menu.m_mainMenu == Menu::MM_Start) {
		m_menu.m_mainMenu = Menu::MM_Difficulty;
		return true;
	}

	if (m_menu.m_mainMenu != Menu::MM_None) {
		m_menu.m_hotButton = m_menu.m_mainMenu;
		m_menu.m_mainMenu = Menu::MM_None;
		return true;
	}

	a_target.SetPause(!m_model.m_isPaused);

	return true;
}

void GameController::PlayerFireMissile(int a_missileIx) {
	m_particles.m_missileOldPositions[a_missileIx] = m_model.m_missiles[a_missileIx].m_pos;
	//if (a_missileIx == 0) {	// only trigger sound on first call
		m_core.m_assets.m_sfx.mySounds[SoundBox::S_MissileFire].Play(m_core.m_assets.m_sfx.m_volume);
	//}
}

void GameController::MissileHitEnemy(int a_enemyIx, int a_missileIx) {
	m_core.m_assets.m_sfx.mySounds[SoundBox::S_MissileHit].Play(m_core.m_assets.m_sfx.m_volume);
}

bool GameController::ShowingBriefing() const {
	if (m_model.GetState() == model::Model::GS_WaitingForNextLevel) {
		const float limit(m_gameView.IsRenderingRiftExplosion() ? 5.5f : 2.5f);
		return m_time - m_bossKilledTime > limit;
	}
	return false;				
}

}



