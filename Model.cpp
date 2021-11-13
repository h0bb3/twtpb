#include "StdAfx.h"
#include ".\model.h"

#include <math/AABoundingBox.h>
#include <Common/PComplex.h>

#include "SecuredSections.h"
#include "RandomTable.h"

namespace model {

//#define BULLET_SPEED 300.f
#define BULLET_SPEED 1000.f	//johno's opinion :)
#define STARTING_WEAPON 0

Model::Model(IEventTarget &a_target, const cmn::String &a_uid)
:m_target(a_target)
,m_scores(a_uid)
,m_isPaused(false)
,m_waveIx(0)
,m_levelTime(0.f)
,m_levelIx(-1)
,m_loopCount(0)
,m_pCurrentBoss(NULL)
,m_slowmoTimer(0.f)
, m_bossWaitTime(5.0f)
,m_serverVersion(0)
,m_instrumentationRequest(a_uid)
,m_totalTime(0.0f)
,m_gameTime(0.0f)
{
	m_unlockedLevels = 0;
	m_continues = 5;

	m_instrumentationRequest.StartGame();

	UpdateVersionRequest();	// this promts firewalls
}

Model::~Model(void) {
	m_instrumentationRequest.EndGame(m_levelIx, m_totalTime);
	m_instrumentationRequest.Purge();

	if (m_hasFormationChanged) {
		cmn::PComplex formations("formations");

		formations.Register("_", m_formations, g_maxFormations);
		formations.Save("data/formations.xml");
	}
	if (m_hasWavesChanged) {
		cmn::PComplex waves("waves");
		
		waves.Register("w_", m_waves, g_maxWaves);
		waves.Save("data/waves.xml");
	}

	cmn::PComplex progress("progress");
	int lvlCheck(g_randomTable[17 + m_unlockedLevels]), cntCheck(g_randomTable[117 + m_continues]);

	progress.Register("level", m_unlockedLevels, m_unlockedLevels);
	progress.Register("continues", m_continues, m_continues);
	progress.Register("lvlCheck", lvlCheck, lvlCheck);
	progress.Register("cntCheck", cntCheck, cntCheck);
	progress.Save("userdata/progress.xml");
}

bool Model::Initiate() {
	cmn::PComplex formations("formations");

	formations.Register("_", m_formations, g_maxFormations);
	formations.Load("data/formations.xml");
	m_hasFormationChanged = false;

	cmn::PComplex waves("waves");		
	waves.Register("w_", m_waves, g_maxWaves);
	waves.Load("data/waves.xml");
	m_hasWavesChanged = false;

	cmn::PComplex progress("progress");
	int lvlCheck, cntCheck;

	progress.Register("level", m_unlockedLevels, 0);
	progress.Register("continues", m_continues, 0);
	progress.Register("lvlCheck", lvlCheck, 0);
	progress.Register("cntCheck", cntCheck, 0);
	progress.Load("userdata/progress.xml");

	if (m_unlockedLevels > g_maxLevels || lvlCheck != g_randomTable[17 + m_unlockedLevels]) {
		m_unlockedLevels = 0;
	}
	if (m_continues >= g_maxLevels || cntCheck != g_randomTable[117 + m_continues]) {
		m_continues = 0;
	}

//#ifndef PUBLIC_RELEASE
//	m_unlockedLevels = g_maxLevels - 1;
//#endif

	return true;
}

int Model::GetLoopedLevelIx() const {
	return m_levelIx + g_maxLevels * m_loopCount;
}

const bool Model::TrialExpired() const {
	if (IsTrial()) {
		if (m_trialExpirationTimer > 0.0f && m_levelTime - m_trialExpirationTimer > 10.0f) {
			return true;
		}
	}

	return false;
}

bool Model::Update(float a_dt) {
	GameState state(GetState());

	m_scores.Update();
	UpdateVersionRequest();
	m_instrumentationRequest.Update();

	m_totalTime += a_dt;

	/*if (m_player.m_score.Check() != true ||	m_player.m_multiplier.Check() != true) {
		LOG_3("Cheat!");
	}*/

	if (m_isPaused != true) {

		if (TrialExpired()) {
			m_player.m_life = 0;
		}

		m_slowmoTimer -= a_dt;
		if (m_player.IsUsingAbility(Player::A_SlowMotion, m_levelIx + g_maxLevels * m_loopCount) && m_slowmoTimer <= 0.0f) {
			m_slowmoTimer = 0.25f;
			m_player.m_energy -= Player::g_abilities[m_player.m_ability].m_cost;
		}
		if (m_slowmoTimer > 0.0f) {
			a_dt *= 0.25f;
		}

		m_levelTime += a_dt;
		m_gameTime += a_dt;
		m_bossTimer -= a_dt;
		m_player.Update(a_dt);

		if (m_levelTime - m_player.m_lifeLostTime > 3.0f) {
			m_levelTime = 0.0f;
			m_player.m_lifeLostTime = 60 * 60 * 24.0f;

			m_player.RestartLevel();

			// restart the level
			if (m_pCurrentBoss != NULL) {
				// death in bossfight
				// but there is the odd chance that the player and the boss is dead
				// so we only reinit the boss if it is alive
				if (m_pCurrentBoss->IsAlive()) {
					for (int eIx = 0; eIx < g_maxEnemies; eIx++) {
						m_enemies[eIx].Kill(false);
					}
					m_pCurrentBoss->Initiate();
					m_bossTimer = 5.0f;
				}
			} else {
				// death in level
				m_pCurrentBoss = NULL;
				for (int wIx = 0; wIx < g_maxWaves; wIx++) {
					m_waves[wIx].m_hasTriggered = false;
				}
				for (int eIx = 0; eIx < g_maxEnemies; eIx++) {
					m_enemies[eIx].Kill(false);
				}
			}
			for (int bIx = 0; bIx < g_maxEnemyBullets; bIx++) {
				m_enemyBullets[bIx].Kill();
			}
			for (int bIx = 0; bIx < g_maxPlayerBullets; bIx++) {
				m_playerBullets[bIx].Kill();
			}
			for (int bIx = 0; bIx < g_maxBonuses; bIx++) {
				m_bonuses[bIx].m_isAlive = false;
			}
		}

		if (m_player.DoFire(m_levelIx + g_maxLevels * m_loopCount)) {

			// level 0
			int gunIx(0);

			for (int i = 0; i < 2; i++) {
				const int bIx(GetNextFreePlayerBullet());
				if (bIx >= 0) {
					m_playerBullets[bIx].m_speed = m_player.m_guns[gunIx].m_dir * BULLET_SPEED;
					m_playerBullets[bIx].Spawn(m_player.m_pos + m_player.m_guns[gunIx].m_pos);
					m_target.PlayerFireBullet(gunIx, bIx);
					gunIx++;
				}
			}

			// level 1
			if (m_player.m_weapon > 0) {
				const int bIx(GetNextFreePlayerBullet());
				if (bIx >= 0) {
					
					m_playerBullets[bIx].m_speed = m_player.m_guns[gunIx].m_dir * BULLET_SPEED;
					m_playerBullets[bIx].Spawn(m_player.m_pos + m_player.m_guns[gunIx].m_pos);
					m_target.PlayerFireBullet(gunIx, bIx);
					gunIx++;
				}
			}

			// level 2
			if (m_player.m_weapon > 1) {
				const int speed[2] = {-150, 150};
				for (int i = 0; i < 2; i++) {
					const int bIx(GetNextFreePlayerBullet());
					if (bIx >= 0) {
						m_playerBullets[bIx].m_speed = m_player.m_guns[gunIx].m_dir * BULLET_SPEED;
						m_playerBullets[bIx].Spawn(m_player.m_pos + m_player.m_guns[gunIx].m_pos);

						m_target.PlayerFireBullet(gunIx, bIx);
						gunIx++;
					}
				}
			}

			//level 3
			if (m_player.m_weapon > 2) {
				const int bIx(GetNextFreePlayerBullet());
				if (bIx >= 0) {

					m_playerBullets[bIx].m_speed = m_player.m_guns[gunIx].m_dir * BULLET_SPEED;
					m_playerBullets[bIx].Spawn(m_player.m_pos + m_player.m_guns[gunIx].m_pos);
					m_target.PlayerFireBullet(gunIx, bIx);
					gunIx++;
				}
			}

			// level 4
			if (m_player.m_weapon > 3) {
				const int speed[2] = {-BULLET_SPEED, BULLET_SPEED};
				for (int i = 0; i < 2; i++) {
					const int bIx(GetNextFreePlayerBullet());
					if (bIx >= 0) {
						m_playerBullets[bIx].m_speed = m_player.m_guns[gunIx].m_dir * BULLET_SPEED;
						m_playerBullets[bIx].Spawn(m_player.m_pos + m_player.m_guns[gunIx].m_pos);
						m_target.PlayerFireBullet(gunIx, bIx);
						gunIx++;
					}
				}
			}
		}

		{
			const int missileCount(m_player.DoFireMissiles(m_levelIx + g_maxLevels * m_loopCount));
			int addedMissiles(0);
			if ((missileCount > 0 || m_levelIx >= 4) && m_trialExpirationTimer < 0.0f) {
				m_trialExpirationTimer = m_levelTime;
			}
			for (int mIx = 0; mIx < g_maxMissiles && addedMissiles < missileCount; mIx++) {
				if (m_missiles[mIx].IsAlive() != true) {
					m_missiles[mIx].Fire(m_player.m_pos);
					m_target.PlayerFireMissile(mIx);
					addedMissiles++;
				}
			}
		}

		if (m_player.m_pos.x < g_playerWidth) {
			m_player.m_pos.x = g_playerWidth;
		} else if (m_player.m_pos.x > g_width - g_playerWidth) {
			m_player.m_pos.x = g_width - g_playerWidth;
		}
		if (m_player.m_pos.y < g_playerHeight) {
			m_player.m_pos.y = g_playerHeight;
		} else if (m_player.m_pos.y > g_height - g_playerHeight) {
			m_player.m_pos.y = g_height - g_playerHeight;
		}

		const bool bossAlive(m_pCurrentBoss != NULL && m_pCurrentBoss->IsAlive() &&  m_pCurrentBoss->GetTimeLeft() > 0.0f);

		UpdateBullets(a_dt);
		UpdateMissiles(a_dt);

		// now we can check if the boss died

		UpdateBonuses(a_dt);

		if (UpdateEnemies(a_dt * ((float)m_loopCount + 1.0f)) != true) {
			if (IsLooping()) {
				m_waveIx = 0;
				m_levelTime = 0.0f;
				for (int wIx = 0; wIx < g_maxWaves; wIx++) {
					m_waves[wIx].m_hasTriggered = false;
				}
			} else {
				InitiateWave();
			}
		}

		// notify target if boss died in battle or time ran out
		if (bossAlive && (m_pCurrentBoss->IsAlive() != true || m_pCurrentBoss->GetTimeLeft() <= 0.0f)) {
			if (m_pCurrentBoss->GetTimeLeft() > 0.0f) {
				m_player.m_score += g_bossKilledBonus * (m_levelIx + 1) * m_player.m_multiplier * m_difficulty;
				m_player.m_score += g_bossKilledBonus * (g_maxLevels * m_loopCount) * m_player.m_multiplier * m_difficulty;
			}
			m_target.BossKilled();
		}

		if (state == GS_Playing && IsNextLevel()) {
			m_target.StartGame((m_levelIx + 1) % g_maxLevels, m_difficulty);
			// this is to fix so that you don't have to wait between levels when looping the game...
			if (m_loopCount > 0 || (m_levelIx + 1) >= g_maxLevels) {
				NextLevel();
			}
		}


		// notify target for enemy changes
		for (int eIx = 0; eIx < g_maxEnemies; eIx++) {
			if (m_enemies[eIx].m_doNotifyTarget) {
				m_target.HitEnemy(eIx);
				m_enemies[eIx].m_doNotifyTarget = false;
			}
		}

		// check if an enemy slipped by the player
		for (int cIx = 0; cIx < g_maxControllers; cIx++) {
			if (m_controllers[cIx].m_enemyEscaped == true) {
				m_controllers[cIx].m_enemyEscaped = false;
				m_player.m_multiplier = 1;
			}
		}
	}

	return true;
}

void Model::StartGame(int a_level, float a_difficulty) {
	if (a_level <= m_unlockedLevels) {

/*		if (a_level > 0 && m_continues <= 0) {
			return;
		}*/

		// Go to waiting for next level
		for (int eIx = 0; eIx < g_maxEnemies; eIx++) {
			m_enemies[eIx].Kill(false);
		}
		for (int bIx = 0; bIx < g_maxEnemyBullets; bIx++) {
			m_enemyBullets[bIx].Kill();
		}
		for (int bIx = 0; bIx < g_maxPlayerBullets; bIx++) {
			m_playerBullets[bIx].Kill();
		}
		for (int wIx = 0; wIx < g_maxWaves; wIx++) {
			m_waves[wIx].m_hasTriggered = false;
		}
		for (int bIx = 0; bIx < g_maxBonuses; bIx++) {
			m_bonuses[bIx].m_isAlive = false;
		}

		m_levelIx = a_level - 1;
		if (a_level > 0) {
			m_continues -= 2;	// need to use -2 and not -1 since you get a continue for the level you start on
		} else {
			m_continues--;	// the first level does not const anyting
		}
		m_levelTime = 0.0f;
		m_gameTime = 0.0f;
		m_player.Init(STARTING_WEAPON + a_level, IsTrial() ? 5 : 5);
		m_waveIx = 0;
		m_isPaused = false;
		m_loopCount = 0;
		m_pCurrentBoss = NULL;
		m_slowmoTimer = 0.0f;
		m_bossTimer = -100.0f;
		m_isAgressive = false;
		m_trialExpirationTimer = -1.0f;
		m_difficulty = a_difficulty;

		m_pCurrentBoss = &m_pxboss;
		m_pCurrentBoss->Initiate();

		m_instrumentationRequest.StartGameAtLevel(a_level, m_difficulty);
		m_target.StartGame(m_levelIx, m_difficulty);
	}
}

void Model::PlayerInput(math::Vector2f a_dPos, bool a_doFire, bool a_useEnergy, int a_ability) {
	const GameState state(GetState());

	switch (state) {
		case GS_Playing:
			if (m_player.IsActive(m_levelTime)) {
				m_player.m_dPos = a_dPos;
				m_player.m_ability = (Player::Ability)a_ability;
				if (a_doFire) {
					if (m_player.m_doFire != true) {
						m_player.m_doFire = true;
					}
				}
				m_player.m_useEnergy = a_useEnergy;
			}
		break;
		case GS_GameOver:
			if (a_doFire) {
				m_isPaused = false;
				// Go to staring
				m_player.m_score = 0;
			}
		break;
	};
}

void Model::EnterName(cmn::String a_name) {
	const GameState state(GetState());

	if (state == GS_WaitingForName) {
		if (a_name.size() > 0) {
			m_scores.AddScore(a_name, m_player.m_score, m_player.m_score.GetOffset(), m_player.m_score.GetChecksum(), m_gameTime, m_levelIx);
		}
		m_player.m_score = 0;
	}
}

void Model::NextLevel() {
	if (GetState() == GS_WaitingForNextLevel) {
		m_pCurrentBoss = NULL;
		for (int eIx = 0; eIx < g_maxEnemies; eIx++) {
			m_enemies[eIx].Kill(false);
		}

		m_levelTime = 0.0f;
		m_levelIx++;

		// earn a continue and unlock a level
		m_continues++;
		if (IsTrial() && m_continues > 3) {
			m_continues = 3;
		}
		if (m_unlockedLevels < m_levelIx) {
			m_unlockedLevels = m_levelIx;
		}
		
		m_player.NextLevel();

		// hard coded start over...
		if (m_levelIx >= g_maxLevels) {
			//m_pCurrentBoss = NULL;
			m_loopCount++;
			m_levelIx = 0;
			m_levelTime = 0.0f;

			for (int wIx = 0; wIx < g_maxWaves; wIx++) {
				m_waves[wIx].m_hasTriggered = false;
			}
		}
	}
}

bool Model::UpdateEnemies(float a_dt) {
	bool aliveEnemy(false);
	const bool bossInit(GetBossScale() < 1.0f);

	math::AABoundingBox2D playerBox;
	math::AABoundingBox2D enemyBox;
	m_player.GetBox(playerBox);
	
	if (m_isAgressive == true && bossInit != true) {
		for (int eIx = 0; eIx < g_maxEnemies; eIx++) {
			if (m_enemies[eIx].IsAlive() == true) {
				m_enemies[eIx].m_oldPos = m_enemies[eIx].m_pos;
				Bullet bullet;
				while (m_enemies[eIx].DoFire(m_player.m_pos, bullet)) {
					int bIx(0);
					for (; bIx < g_maxEnemyBullets; bIx++) {
						if (m_enemyBullets[bIx].IsAlive() != true) {
							m_enemyBullets[bIx] = bullet;
							break;
						}
					}
					if (bIx >= g_maxEnemyBullets) {
						break;
					}
				}

				if (m_player.IsActive(m_levelTime)) {
					m_enemies[eIx].GetBox(enemyBox);
					if (playerBox.Intersects(enemyBox)) {
						HitEnemy(eIx, 1);
						HitPlayer();
						m_target.HitEnemy(eIx);
						m_target.HitPlayer();
					}
				}
			}
		}
	}

	// if it is time for the boss let all formations die
	if (IsTimeForBoss() || IsLooping()) {
		for (int cIx = 0; cIx < g_maxControllers; cIx++) {
			if (m_controllers[cIx].Update(a_dt, m_difficulty) == true) {
				aliveEnemy = true;
			}
		}
	} else {
		aliveEnemy = true;
		for (int cIx = 0; cIx < g_maxControllers; cIx++) {
			if (m_controllers[cIx].Update(a_dt, m_difficulty) != true ) {
				aliveEnemy = false;
			}
		}
	}

	if (m_pCurrentBoss != NULL && m_pCurrentBoss->IsAlive() && m_pCurrentBoss->GetTimeLeft() > 0.0f) {
		m_pCurrentBoss->Update(a_dt, m_player.m_pos, m_isAgressive, m_difficulty);
		aliveEnemy = true;
	}

	return aliveEnemy;
}

bool Model::IsLooping() const {
	return m_player.m_life == 0 && m_player.m_score == 0 && m_waveIx > 17;
}

bool Model::IsTimeForBoss() {

	
	//Cheat so boss is always thrown in
	//if (m_pCurrentBoss == NULL) {
	//	m_pCurrentBoss = &m_pxboss;
	//	m_bossTimer = 15.0f;
	//	return true;
	//}


	if (m_pCurrentBoss != NULL) {
		if (m_bossTimer < 0.0) {
			return false;
		}
		return true;
	}

	// no boss if the player is dead
	if (m_player.m_shield <= 0.0f) {
		return false;
	}

	// if there are no more formations that should be spawned for this level...
	for (size_t wIx = 0; wIx < g_maxWaves; wIx++) {
		if (m_waves[wIx].m_levelIx == m_levelIx &&
			m_waves[wIx].m_hasTriggered == false) {
				return false;
		}
	}
	// and the formations are empty
	for (int cIx = 0; cIx < g_maxControllers; cIx++) {
		if (m_controllers[cIx].m_enemies.size() > 0) {
			return false;
		}
	}

	// and there are no live enemy bullets
	for (int bIx = 0; bIx < g_maxEnemyBullets; bIx++) {
		if (m_enemyBullets[bIx].IsAlive()) {
			return false;
		}
	}

	const int bossIx(m_levelIx % g_maxBosses);
	switch (bossIx) {
		case 0:
			m_pCurrentBoss = &m_pxboss;
		break;
		case 1:
			m_pCurrentBoss = &m_fury;
		break;
		case 2:
			m_pCurrentBoss = &m_shockwave;
		break;
		case 3:
			m_pCurrentBoss = &m_yelly;
		break;
		case 4:
			m_pCurrentBoss = &m_danMaku;
		break;
		case 5:
			m_pCurrentBoss = &m_beamBoss;
		break;
		case 6:
			m_pCurrentBoss = &m_snakeBoss;
		break;
		case 7:
			m_pCurrentBoss = &m_twinBoss;
		break;
		case 8:
			m_pCurrentBoss = &m_squidBoss;	
		break;
		case 9:
			m_pCurrentBoss = &m_finalBoss;
		break;
	};
	m_pCurrentBoss->Initiate();

	m_bossTimer = m_bossWaitTime;

	return true;
}

void Model::InitiateWave() {
	const GameState state(GetState());

	if (state == GS_WaitingForNextLevel || state == GS_GameCompleted) {
		return;
	}

	if (IsTimeForBoss() && m_bossTimer < 1.0f && m_pCurrentBoss->IsAlive() != true) {

		m_pCurrentBoss->Initiate();

		int eIx = 0;
		do {
			eIx = GetNextFreeEnemy();
			if (eIx < 0) {
				break;
			}
			m_target.ResetEnemy(eIx);
		} while (m_pCurrentBoss->AddEnemy(&m_enemies[eIx]));
		m_pCurrentBoss->Update(0.0f, m_player.m_pos, m_isAgressive, m_difficulty);
		
		//m_waveIx++;
	} else {

		for (int wIx = 0; wIx < g_maxWaves; wIx++) {
			if (m_levelIx >= 0 &&
				m_waves[wIx].m_levelIx == m_levelIx &&
				m_waves[wIx].m_hasTriggered == false &&
				m_waves[wIx].m_triggerTime <= m_levelTime) {

				m_waves[wIx].m_hasTriggered = true;
				m_waveIx++;

				for (int cIx = 0; cIx < g_maxControllers; cIx++) {
					FormationController &controller(m_controllers[cIx]);

					if (controller.m_enemies.size() > 0) {
						continue;
					}

					controller.m_enemyEscaped = false;
					controller.m_speedMultiplier = m_waves[wIx].m_speedMultiplier;
					controller.m_currentFormation = m_waves[wIx].m_controller;
					controller.m_currentOffset = m_waves[wIx].m_offset;

					const int fIx(m_waves[wIx].m_formationIx);

					for (int i = 0; i < m_formations[fIx].m_enemies.size(); i++) {
						const int eIx(GetNextFreeEnemy());
						if (eIx < 0) {
							break;
						}

						Enemy *pEnemy(&m_enemies[eIx]);
						m_formations[fIx].m_enemies[i].SetEnemyAlive(*pEnemy, wIx, i);
						pEnemy->m_life *= m_loopCount + 1;


						m_target.ResetEnemy(eIx);
						controller.AddEnemy(pEnemy);
					}

					controller.Update(m_levelTime - m_waves[wIx].m_triggerTime, m_difficulty);
					break;
				}
			}

			
			/*if (IsTimeForBoss() || IsTimeForBonus()) {
				// dont spawn any more formations
				break;
			}*/
		}
	}
}

int Model::GetNextFreeEnemy() {
	static int eIx = 0;
	const int stopIx = eIx;

	while(true) {
		if (m_enemies[eIx].IsAlive() != true) {
			const int ret(eIx);

			eIx++;
			eIx %= g_maxEnemies;
			
			return ret;
		}
		eIx++;
		eIx %= g_maxEnemies;

		if (eIx == stopIx) {
			return -1;
		}
	}

	return -1;
}

int Model::GetNextFreePlayerBullet() {
	static int bIx = 0;
	const int stopIx = bIx;

	while(true) {
		if (m_playerBullets[bIx].IsAlive() != true) {
			const int ret(bIx);

			bIx++;
			bIx %= g_maxPlayerBullets;
			
			return ret;
		}
		bIx++;
		bIx %= g_maxEnemies;

		if (bIx == stopIx) {
			return -1;
		}
	}

	return -1;
}

bool Model::UpdateBullet(Bullet &a_bullet, float a_dt) {
	a_bullet.Update(a_dt);

	if (a_bullet.GetPos().x < -g_bulletSize || a_bullet.GetPos().x > g_width + g_bulletSize ||
		a_bullet.GetPos().y < -g_bulletSize || a_bullet.GetPos().y > g_height + g_bulletSize) {
		a_bullet.Kill();
	}
	return a_bullet.IsAlive();
}

void Model::UpdateBullets(float a_dt) {
	math::AABoundingBox2D playerBox;
	m_player.GetBox(playerBox);
	const bool bossInit(GetBossScale() < 1.0f);

	if (bossInit) {
		for (int bIx = 0; bIx < g_maxPlayerBullets; bIx++) {
			if (m_playerBullets[bIx].IsAlive() == true) {
				UpdateBullet(m_playerBullets[bIx], a_dt);
			}
		}

		for (int bIx = 0; bIx < g_maxEnemyBullets; bIx++) {
			if (m_enemyBullets[bIx].IsAlive()) {
				UpdateBullet(m_enemyBullets[bIx], a_dt);
			}
		}

		return;
	}

	if (m_player.IsActive(m_levelTime) && m_isAgressive == true) {
		for (int eIx = 0; eIx < g_maxEnemies; eIx++) {
			if (m_enemies[eIx].DoBeam() && m_enemies[eIx].m_beam.IsLethal()) {
				const math::Vector2f normal(cosf(m_enemies[eIx].m_beam.m_angle + PI), sinf(m_enemies[eIx].m_beam.m_angle + PI));
				int side;
				float u;

				//float angle = atan2f(normal.y, normal.x);
				
				playerBox.m_size.x += m_enemies[eIx].m_beam.m_width;
				playerBox.m_size.y += m_enemies[eIx].m_beam.m_width;

				//if (m_enemies[eIx].m_beam.m_beam.Intersects(playerBox)) {
				if (playerBox.LineAABO(m_enemies[eIx].m_pos, m_enemies[eIx].m_pos + normal * 1000.0f, side, u) && u > 0.0f && u < 1.0f) {
					HitPlayer();
					m_target.HitPlayer();
				}

				playerBox.m_size.x -= m_enemies[eIx].m_beam.m_width;
				playerBox.m_size.y -= m_enemies[eIx].m_beam.m_width;
			}
		}
	}

	for (int bIx = 0; bIx < g_maxPlayerBullets; bIx++) {
		if (m_playerBullets[bIx].IsAlive() == true) {
			if (UpdateBullet(m_playerBullets[bIx], a_dt)) {
				math::AABoundingBox2D enemyBox;
				math::AABoundingBox2D bulletBox;

				m_playerBullets[bIx].GetBox(bulletBox);

				for (int eIx = 0; eIx < g_maxEnemies; eIx++) {
					if (m_enemies[eIx].IsAlive()) {
						m_enemies[eIx].GetBox(enemyBox);

						if (enemyBox.Intersects(bulletBox)) {
							HitEnemy(eIx, 1);
							m_target.HitEnemy(eIx);
							if (m_enemies[eIx].m_hasShield) {
								m_playerBullets[bIx].Kill();
								/*math::Vector2f normal(m_playerBullets[bIx].m_pos - m_enemies[eIx].m_pos);
								math::Vector2f incoming(m_playerBullets[bIx].m_speed);

								// if the bullet is already moving away we need not bounce it
								if (normal.Dot(incoming) < 0) {
									float speed;
									math::Vector2f reflect;

									speed = incoming.Length();
									incoming /= speed;

									normal.Normalize();
									reflect = 2.0f * normal.Dot(incoming) * incoming - normal;
									reflect.Normalize();
									m_playerBullets[bIx].m_speed = reflect * speed;
								}*/


							} else {
								m_target.PlayerBulletHit(bIx);
								m_playerBullets[bIx].Kill();
							}
							
							break;
						}
					}
				}
			}
		}
	}

	if (m_isAgressive == true) {
		for (int bIx = 0; bIx < g_maxEnemyBullets; bIx++) {
			if (m_enemyBullets[bIx].IsAlive()) {
				if (UpdateBullet(m_enemyBullets[bIx], a_dt) && m_player.IsActive(m_levelTime)) {
					math::AABoundingBox2D bulletBox;
					m_enemyBullets[bIx].GetBox(bulletBox);

					if (playerBox.Intersects(bulletBox)) {

						m_enemyBullets[bIx].Kill();
						HitPlayer();
						m_target.HitPlayer();
					}
				}
			}
		}
	}
}

void Model::UpdateMissiles(float a_dt) {
	for (int mIx = 0; mIx < g_maxMissiles; mIx++) {
		if (m_missiles[mIx].IsAlive()) {
			m_missiles[mIx].Update(a_dt, m_enemies, g_maxEnemies, m_player.m_pos);

			math::AABoundingBox2D enemyBox;
			math::AABoundingBox2D missileBox;

			m_missiles[mIx].GetBox(missileBox);

			for (int eIx = 0; eIx < g_maxEnemies; eIx++) {
				if (m_enemies[eIx].IsAlive()) {
					m_enemies[eIx].GetBox(enemyBox);

					if (enemyBox.Intersects(missileBox)) {
						HitEnemy(eIx, 10);
						m_target.HitEnemy(eIx);
						m_target.MissileHitEnemy(eIx, mIx);
						m_missiles[mIx].Kill();
						break;
					}
				}
			}
		}
	}
}

void Model::HitPlayer() {
	m_player.Damage(27, m_levelTime);
	if (m_player.m_shield < 0) {
		m_instrumentationRequest.DeathAtLevel(m_levelIx, m_levelTime);
	}
}

void Model::HitEnemy(int a_enemyIx, int a_damage) {
	m_enemies[a_enemyIx].Damage(a_damage, true);

	m_isAgressive = true;

	if (m_enemies[a_enemyIx].IsAlive() == false) {

		if (m_player.IsActive(m_levelTime)) {
			m_player.m_score += 350 * (m_levelIx + m_loopCount + 1) * m_player.m_multiplier * m_difficulty;
		}

		bool doDropBonus(false);
		if (m_enemies[a_enemyIx].m_doDropEnergy) {
			doDropBonus = true;
		}
		// we need to know what controllers has enemies so we can drop a bonus if the enemy was the last enemy;
		for (int cIx = 0; cIx < g_maxControllers; cIx++) {
			if (m_controllers[cIx].m_enemies.size() > 0) {
				m_controllers[cIx].RemoveDeadEnemies();
				// drop bonus if the enemy was the last enemy in a controller...
				if (m_controllers[cIx].m_enemies.size() == 0) {
					doDropBonus = true;
					m_player.m_multiplier++;
					break;
				}
			}
		}
		if (doDropBonus) {
			for (int bIx = 0; bIx < g_maxBonuses; bIx++) {
				if (m_bonuses[bIx].m_isAlive == false) {
					m_bonuses[bIx].m_isAlive = true;

					if (m_enemies[a_enemyIx].m_waveIx >= 0 && m_waves[m_enemies[a_enemyIx].m_waveIx].m_dropWeapon > 0) {
						m_bonuses[bIx].m_type = Bonus::BT_Weapon;
						m_bonuses[bIx].m_pos = m_enemies[a_enemyIx].m_pos;
					} else {
						m_bonuses[bIx].m_type = Bonus::BT_Score;
						m_bonuses[bIx].m_pos = m_enemies[a_enemyIx].m_pos;
					}
					return;
				}
			}
		}
	}
}

Model::GameState Model::GetState() const {
	if (m_player.m_score <= 0) {
		if (m_player.m_life > 0) {
			if (m_loopCount > 0 && m_isAgressive != true) {
				return GS_GameCompleted;
			} else if (IsNextLevel()) {
				return GS_WaitingForNextLevel;
			} else {
				return GS_Playing;
			}
		} else {
			return GS_Starting;
		}
	} else {
		if (m_player.m_life > 0) {
			if (IsNextLevel()) {
				return GS_WaitingForNextLevel;
			} else {
				return GS_Playing;
			}
		} else {
			if (m_scores.IsHighScore(m_player.m_score)) {
				return GS_WaitingForName;
			} else {
				return GS_GameOver;
			}
		}
	}
}

bool Model::IsNextLevel() const {
	if (m_pCurrentBoss != NULL && m_bossTimer < 0.0f && (m_pCurrentBoss->IsAlive() != true || m_pCurrentBoss->GetTimeLeft() <= 0.0f)) {
		for (int bIx = 0; bIx < g_maxEnemyBullets; bIx++) {
			if (m_enemyBullets[bIx].IsAlive()) {
				return false;
			}
		}
		for (int bIx = 0; bIx < g_maxBonuses; bIx++) {
			if (m_bonuses[bIx].m_isAlive == true) {
				return false;
			}
		}
		return true;
	}
	return false;
}

void Model::SetPause(bool a_doPause) {
	if (a_doPause == true) {
		GameState state(GetState());
		if (state == GS_Playing) {
			m_isPaused = a_doPause;
		} else if (state == GS_GameCompleted) {
			EndGame();
		}
	} else {
		m_isPaused = false;
	}
}

void Model::UpdateBonuses(float a_dt) {
	math::AABoundingBox2D bonusBox;
	math::AABoundingBox2D playerBox;

	m_player.GetBox(playerBox);

	bonusBox.m_size.x = bonusBox.m_size.y = 10;

	for (int bIx = 0; bIx < g_maxBonuses; bIx++) {
		if (m_bonuses[bIx].m_isAlive == true) {

			m_bonuses[bIx].m_pos.y += a_dt * 200.0f;
			// attract towards player
			if (m_player.IsActive(m_levelTime)) {
				bonusBox.m_pos = m_bonuses[bIx].m_pos;
				if (bonusBox.Intersects(playerBox)) {
					m_bonuses[bIx].m_isAlive = false;
					if (m_bonuses[bIx].m_type == Bonus::BT_Score) {
						m_player.m_score += 1250 * (m_levelIx + 1 + m_loopCount) * m_player.m_multiplier * m_difficulty;
						m_player.m_energy += 20;
						m_target.EnergyPickup();
						if (m_player.m_energy > m_player.m_maxEnergy) {
							m_player.m_energy = m_player.m_maxEnergy;
						}
					} else if (m_bonuses[bIx].m_type == Bonus::BT_Shield) {
						m_player.m_life++;
						m_target.LifePickup();
					} else {
						m_player.m_weapon++;
						m_target.WeaponPickup();
					}
				} else {
					math::Vector2f toPlayer(m_player.m_pos - m_bonuses[bIx].m_pos);
					const float length2(toPlayer.Length2());
					if (length2 < 10000.0f && length2 > 0.0f) {
						m_bonuses[bIx].m_pos += (toPlayer) * 100.0f / length2 * 250.f * a_dt;
					}
				}


			}
			if (m_bonuses[bIx].m_pos.y > g_height + 32) {
				m_bonuses[bIx].m_isAlive = false;
				m_player.m_multiplier = 1;
			}
		}
	}
}


void Model::AddEnemyToFormation(int a_formationIx, int a_enemyType, float a_time) {
	if (a_formationIx >= 0 && a_formationIx < g_maxFormations) {
		Formation::Enemy e;
		e.m_timeOffset = a_time;
		e.m_enemyType = (Enemy::Type)a_enemyType;

		m_formations[a_formationIx].m_enemies.push_back(e);
		m_hasFormationChanged = true;
	}
}

void Model::UpdateEnemyInFormation(int a_formationIx, int a_enemyIx, int a_enemyType, float a_time, math::Vector2f a_offset, float a_scale, bool a_hasShield) {
	if (a_formationIx >= 0 && a_formationIx < g_maxFormations) {
		if (a_enemyIx >= 0 && a_enemyIx < m_formations[a_formationIx].m_enemies.size()) {
			m_formations[a_formationIx].m_enemies[a_enemyIx].m_timeOffset = a_time;
			m_formations[a_formationIx].m_enemies[a_enemyIx].m_offset = a_offset;
			m_formations[a_formationIx].m_enemies[a_enemyIx].m_enemyType = (Enemy::Type)a_enemyType;
			m_formations[a_formationIx].m_enemies[a_enemyIx].m_scale = a_scale;
			m_formations[a_formationIx].m_enemies[a_enemyIx].m_hasShield = a_hasShield;
			m_hasFormationChanged = true;
		}
	}
}

void Model::DeleteEnemyFromFormation(int a_formationIx, int a_enemyIx) {
	if (a_formationIx >= 0 && a_formationIx < g_maxFormations) {
		if (a_enemyIx >= 0 && a_enemyIx < m_formations[a_formationIx].m_enemies.size()) {
			m_formations[a_formationIx].m_enemies.erase(m_formations[a_formationIx].m_enemies.begin() + a_enemyIx);
			m_hasFormationChanged = true;
		}
	}
}

void Model::CopyFormation(int a_fromFormationIx, int a_toFormationIx) {
	if (a_fromFormationIx >= 0 && a_fromFormationIx < g_maxFormations &&
		a_toFormationIx >= 0 && a_toFormationIx < g_maxFormations) {
		m_formations[a_toFormationIx] = m_formations[a_fromFormationIx];
		m_hasFormationChanged = true;
	}
}

void Model::AddWave(int a_levelIx, float a_triggerTime, float a_speedMultiplier, int a_formationIx, int a_controller, const math::Vector2f &a_offset) {
	if (a_levelIx >= 0) {
		for (int i = 0; i < g_maxWaves; i++) {
			if (m_waves[i].m_levelIx < 0) {
				m_waves[i].m_levelIx = a_levelIx;
				SetWave(i, a_triggerTime, a_speedMultiplier, a_controller, a_formationIx, 0, a_offset);
				return;
			}
		}
	}
}

void Model::SetWave(int a_waveIx, float a_triggerTime, float a_speedMultiplier, int a_controller, int a_formationIx, int a_dropWeapon, const math::Vector2f &a_offset) {
	if (a_waveIx >= 0 && a_waveIx < g_maxWaves && m_waves[a_waveIx].m_levelIx >= 0) {
		if (a_triggerTime < 0.0f) {
			a_triggerTime = 0.0f;
		}

		if (a_speedMultiplier < 0.01f) {
			a_speedMultiplier = 0.01f;
		}

		if (a_controller < 0) {
			a_controller = 0;
		} else if (a_controller >= FormationController::F_Count) {
			a_controller = FormationController::F_Count - 1;
		}

		if (a_formationIx < 0) {
			a_formationIx = 0;
		} else if (a_formationIx >= g_maxFormations) {
			a_formationIx = g_maxFormations;
		}

		if (a_dropWeapon < 0) {
			a_dropWeapon = 0;
		} else if (a_dropWeapon > 1) {
			a_dropWeapon = 1;
		}

		m_waves[a_waveIx].m_offset = a_offset;
		m_waves[a_waveIx].m_triggerTime = a_triggerTime;
		m_waves[a_waveIx].m_speedMultiplier = a_speedMultiplier;
		m_waves[a_waveIx].m_controller = (FormationController::Formation)a_controller;
		m_waves[a_waveIx].m_formationIx = a_formationIx;
		m_waves[a_waveIx].m_dropWeapon = a_dropWeapon;

		m_hasWavesChanged = true;
	}
}

void Model::DeleteWave(int a_waveIx) {
	if (a_waveIx >= 0 && a_waveIx < g_maxWaves) {
		m_waves[a_waveIx].m_levelIx = -1;
	}
}

void Model::EndGame() {
	// Go to staring
	m_player.m_life = 0;
	m_player.m_score = 0;
	m_player.m_multiplier = 1;
	m_isPaused = false;
}

void Model::SetWeapon(int a_weapon) {
#ifndef PUBLIC_RELEASE
	if (a_weapon >= 0) {
		m_player.m_weapon = a_weapon;
	}
#endif
}

void Model::JumpToLevel(int a_level) {
#ifndef PUBLIC_RELEASE
	if (a_level < g_maxLevels && a_level >= 0 && m_levelIx != a_level) {
		for (int i = 0; i < g_maxEnemies; i++) {
			m_enemies[i].Kill(false);
		}
		m_levelIx = a_level;
		m_gameTime = 0.0f;
		m_target.StartGame(m_levelIx, m_difficulty);
	}
#endif
}

void Model::JumpToBoss() {
#ifndef PUBLIC_RELEASE
	for (int i = 0; i < g_maxEnemies; i++) {
		m_enemies[i].Kill(false);
	}

	for (int wIx = 0; wIx < g_maxWaves; wIx++) {
		if (m_levelIx == m_waves[wIx].m_levelIx) {
			m_waves[wIx].m_hasTriggered = true;
		}
	}

#endif
}
void Model::KillPlayer() {
#ifndef PUBLIC_RELEASE
	m_player.Damage(1000, m_gameTime);
#endif
}


// make sure bosses are displayed from the rift
float Model::GetBossScale() const {
	if (m_pCurrentBoss != NULL && m_bossTimer > -1.0f) {
		return -m_bossTimer > 0.0f ? -m_bossTimer : 0.0f;
	}
	if (m_pCurrentBoss != NULL) {
		if (m_pCurrentBoss->GetTimeLeft() < 0.01f) {

			return m_pCurrentBoss->GetTimeLeft() * 100.0f;
		}
	}

	return 1.0f;
}

void Model::UpdateVersionRequest() {
#ifdef KOMPUTER_SWIAT_VERSION
	return;
#endif


	if (m_serverVersion == 0) {
		if (m_versionRequest.update() != true) {
			m_serverVersion = -1;
			return;
		}
		const int state(m_versionRequest.getState());
		if (state == HttpRequest::OFFLINE) {
			m_serverVersion = -1;
			return;
		}

		if (state == HttpRequest::IDLE) {
			if (m_versionRequest.myBuffer == NULL) {
				if (m_versionRequest.request(HttpRequest::POST, "http://www.spellofplay.com/game_scripts/twtpb/GetGameVersion.php") != true) {
					m_serverVersion = -1;
					return;
				}
			} else {
				cmn::String version(m_versionRequest.myBuffer);
				if (version.find("HTTP/1.1 200 OK") != 0) {
					m_serverVersion = -1;
					return;
				} else {
					cmn::String::iterator it(version.begin());
					while (it != version.end()) {
						cmn::String versionnr("");

						if (*it == '#') {
							// go to the end
							it++;
							while (it != version.end()) {
								versionnr += *it;
								it++;
							}
							m_serverVersion = versionnr.ToInt();
							return;
						}
						it++;
					}
				}
			}
		}
		return;
	}
}


#pragma optimize("", off)
const bool Model::IsTrial() const {
//#ifndef PUBLIC_RELEASE
	return false;
//#endif
}
#pragma optimize("", on)


void Model::SendGPUDescription(const char * a_desc) {
	m_instrumentationRequest.GraphicsCard(a_desc);
}

void Model::SetPlayerGunPosition(const math::Vector2f a_pos, const int a_gunIx) {
	m_player.m_guns[a_gunIx].m_pos = a_pos;
}

}


