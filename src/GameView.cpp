// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "stdafx.h"

#include ".\GameView.h"

#include "RiftSprite.h"
#include "GameParticles.h"
#include "Particle.h"

#include <DXRenderer/DX.h>
#include <DXRenderer/DXDefines.h>

#include <Scene/Sprite.h>


view::GameView::GameView(ViewCore &a_core) : m_core(a_core) {
}

void view::GameView::InitFrame()
{
	dxrnd::DX::Device().SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
	dxrnd::DX::Device().SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR);
	m_bars.Clear();
}

float view::GameView::GetNormalizedPeriod(float a_time, float a_lo, float a_hi) {
	if (a_time > a_lo && a_time < a_hi) {
		return (a_time - a_lo) / (a_hi - a_lo);
	}

	return 0.0f;
}

void view::GameView::RenderRift(float a_time, float a_bossKilledTime, float a_bossTimer, bool a_bossIsAlive, bool a_bossIsAgressive) {
	if (a_time - a_bossKilledTime < 6.0f) {
		const float dt(a_time - a_bossKilledTime);
		const float nt(GetNormalizedPeriod(dt, 0.0f, 4.0f));
		const float scale(1.0f - nt * nt);

		if (scale < 1.0f) {
			RiftSprite rift;
			m_core.m_assets.m_riftEffect.Get()->SetFloat("g_resolutionX", m_core.m_width);
			m_core.m_assets.m_riftEffect.Get()->SetFloat("g_resolutionY", m_core.m_height);
			m_core.m_assets.m_riftEffect.Get()->SetFloat("g_time", a_time);
			m_core.m_assets.m_riftEffect.Get()->SetFloat("g_squeeze", 1.8f + (nt * nt) * 150.0f);
			m_core.m_assets.m_riftEffect.Get()->SetTexture(m_core.m_assets.m_riftEffect.m_textures[0].m_handle, m_core.m_assets.m_cloud1.GetTexture());
			m_core.m_assets.m_riftEffect.Get()->SetTexture(m_core.m_assets.m_riftEffect.m_textures[1].m_handle, m_core.m_assets.m_cloud2.GetTexture());
			m_core.m_assets.m_riftEffect.Get()->SetTexture(m_core.m_assets.m_riftEffect.m_textures[2].m_handle, m_core.m_assets.m_riftRamp.GetTexture());
			rift.AddRift(math::Vector2f(400 * m_core.m_scale.x, 100 * m_core.m_scale.y),D3DCOLOR_ARGB(0, 120, 50, 200), math::Vector2f(800 * m_core.m_scale.x * scale, 400 * m_core.m_scale.y * scale));
			rift.Render(m_core.m_assets.m_riftEffect);
		}

		if(a_bossIsAlive != true) {

			scene::Sprite effects;
			effects.Create();

			const float explosionScale(GetNormalizedPeriod(dt, 1.0f, 1.75f));
			if (explosionScale > 0.0f && a_bossIsAgressive) {

				
				if (m_riftExplosionSoundTriggered != true) {
					m_core.m_assets.m_sfx.mySounds[SoundBox::S_PlayerExplode].Play(m_core.m_assets.m_sfx.m_volume);
					m_riftExplosionSoundTriggered = true;
				}

				DoPlayerExplosion(effects, math::Vector2f(400, 100), explosionScale);
				SetGlowRenderState();
				effects.Render(m_core.m_assets.m_explosion);
				effects.Clear();
			}


			
			const float shockWaveScale(GetNormalizedPeriod(dt, 1.5f, 5.0f));
			if (shockWaveScale > 0.0f) {
				DoExplosion(effects, math::Vector2f(400, 100), shockWaveScale, 800.0f, 17);
				SetGlowRenderState();
				effects.Render(m_core.m_assets.m_shockWave2);
				effects.Clear();
			}

			const float shockWaveScale2(GetNormalizedPeriod(dt, 1.0f, 3.0f));
			if (shockWaveScale2 > 0.0f) {
				DoExplosion(effects, math::Vector2f(400, 100), shockWaveScale2, 400.0f, 517);
				SetGlowRenderState();
				effects.Render(m_core.m_assets.m_shockWave2);
				effects.Clear();
			}
		}

	} else if (a_bossTimer > 0.0f && a_bossTimer < 3.0f) {
		const float scale(1.0f - a_bossTimer / 3.0f);
		RiftSprite rift;
		m_core.m_assets.m_riftEffect.Get()->SetFloat("g_resolutionX", m_core.m_width);
		m_core.m_assets.m_riftEffect.Get()->SetFloat("g_resolutionY", m_core.m_height);
		m_core.m_assets.m_riftEffect.Get()->SetFloat("g_time", a_time);
		m_core.m_assets.m_riftEffect.Get()->SetFloat("g_squeeze", 1.8f + (1.0f - scale) * (1.0f - scale) * 170.0f);
		m_core.m_assets.m_riftEffect.Get()->SetTexture(m_core.m_assets.m_riftEffect.m_textures[0].m_handle, m_core.m_assets.m_cloud1.GetTexture());
		m_core.m_assets.m_riftEffect.Get()->SetTexture(m_core.m_assets.m_riftEffect.m_textures[1].m_handle, m_core.m_assets.m_cloud2.GetTexture());
		m_core.m_assets.m_riftEffect.Get()->SetTexture(m_core.m_assets.m_riftEffect.m_textures[2].m_handle, m_core.m_assets.m_riftRamp.GetTexture());
		rift.AddRift(math::Vector2f(400 * m_core.m_scale.x, 100 * m_core.m_scale.y),D3DCOLOR_ARGB(0, 120, 50, 200), math::Vector2f(800 * m_core.m_scale.x * scale, 400 * m_core.m_scale.y * scale));
		rift.Render(m_core.m_assets.m_riftEffect);

		m_riftExplosionSoundTriggered = false;

	} else if (a_bossIsAlive) {
		const float scale(1.0f);
		RiftSprite rift;
		m_core.m_assets.m_riftEffect.Get()->SetFloat("g_resolutionX", m_core.m_width);
		m_core.m_assets.m_riftEffect.Get()->SetFloat("g_resolutionY", m_core.m_height);
		m_core.m_assets.m_riftEffect.Get()->SetFloat("g_time", a_time);
		m_core.m_assets.m_riftEffect.Get()->SetTexture(m_core.m_assets.m_riftEffect.m_textures[0].m_handle, m_core.m_assets.m_cloud1.GetTexture());
		m_core.m_assets.m_riftEffect.Get()->SetTexture(m_core.m_assets.m_riftEffect.m_textures[1].m_handle, m_core.m_assets.m_cloud2.GetTexture());
		m_core.m_assets.m_riftEffect.Get()->SetTexture(m_core.m_assets.m_riftEffect.m_textures[2].m_handle, m_core.m_assets.m_riftRamp.GetTexture());
		rift.AddRift(math::Vector2f(400 * m_core.m_scale.x, 100 * m_core.m_scale.y),D3DCOLOR_ARGB(0, 120, 50, 200), math::Vector2f(800 * m_core.m_scale.x * scale, 400 * m_core.m_scale.y * scale));
		rift.Render(m_core.m_assets.m_riftEffect);
	}
}

void view::GameView::RenderGlowLayer(const model::Model &a_model, float a_time, const float a_enemyHitTimers[model::Model::g_maxEnemies], const float a_playerBulletHitTimers[model::Model::g_maxPlayerBullets]) {	// this is all rendering...
	scene::Sprite glow;
	glow.Create();
	DrawBeams(glow, a_model.GetBossScale(), a_model.m_enemies);
	DrawPlayerBullets(glow, a_model.m_playerBullets);
	DrawBonuses(glow, a_model.m_bonuses);

	{
		scene::Sprite effects;
		effects.Create();
		DrawEnemyBullets(glow, effects, a_model.m_enemyBullets, a_time);

		RenderGlowSprite(glow);
		glow.Clear();

		{
			EnemySprite enemies;

			DrawEnemies(enemies, effects, a_model.m_enemies, a_enemyHitTimers, a_model.GetBossScale(), a_model.m_levelIx, a_time, a_model.GetState() != model::Model::GS_Starting);
			DrawPlayerBulletDebris(enemies, a_model.m_playerBullets, a_playerBulletHitTimers, a_time, a_model.m_levelIx);

			m_core.RenderEnemySprites(enemies);
			enemies.Clear();
		}

		RenderShockwaveSprite(effects);
		effects.Clear();
	}
}

void view::GameView::RenderScore(const math::Vector2f &a_pos, const cmn::String &a_score, const float a_width, const float a_height) {
	math::Vector2 pos(a_pos);

	scene::Sprite s;
	s.Create();
	for (size_t cIx = 0; cIx < a_score.size(); cIx++) {
		s.AddSprite(pos, a_width, a_height, m_core.m_assets.m_hudUVs[a_score[cIx] - '0']);
		pos.x += a_width;
	}
	s.Render(m_core.m_assets.m_hud);
}


void view::GameView::DoExplosion(scene::Sprite &a_effect, const math::Vector2f a_pos, float a_dt, float a_size, int a_offset) {
	const unsigned char c((unsigned char)(255.0f * (1.0f - a_dt)));
	math::Vector2f pos(a_pos);
	const scene::Sprite::UVMap uv;
	DWORD color(D3DCOLOR_ARGB(c, c, c, c));
	const float dt(1.0f - a_dt);
	const float sizeX(25 + a_size * (1.0f - (dt * dt * dt)));
	const float sizeY(25 + a_size * (1.0f - (dt * dt * dt)));

	pos.x *= m_core.m_scale.x;
	pos.y *= m_core.m_scale.y;

	a_effect.AddSprite(pos, sizeX, sizeY, ((float)a_offset) + dt * dt * dt , m_core.m_scale * cosf(a_dt), uv, color);
	a_effect.AddSprite(pos, sizeX, sizeY, PI_HALF - (((float)a_offset) + dt * dt * dt), m_core.m_scale * cosf(a_dt), uv, color);
}


void view::GameView::DrawExplosion(scene::Sprite &a_effect, const math::Vector2f a_pos, float a_dt, float a_angle) {
	if (a_dt <= 1.0f && a_dt > 0.0f) {
		scene::Sprite::UVMap uv;
		const float dU(1.0f / 4.0f);
		const float dV(1.0f / 4.0f);
		const int frame((int)(a_dt * 16.0f));
		math::Vector2f pos(a_pos);

		uv.m_topLeftUV.x = (float)(frame % 4) * dU;
		uv.m_topLeftUV.y = (float)(frame / 4) * dV;

		uv.m_bottomRightUV = uv.m_topLeftUV;
		uv.m_bottomRightUV.x += dU;
		uv.m_bottomRightUV.y += dV;
		
		pos.x *= m_core.m_scale.x;
		pos.y *= m_core.m_scale.y;

		a_effect.AddSprite(pos, 128.0f, 128.0f, a_angle, m_core.m_scale, uv, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
}

void view::GameView::DoPlayerExplosion(scene::Sprite &a_effect, const math::Vector2f a_pos, float a_dt) {
	srand(255671);

	DrawExplosion(a_effect, a_pos, a_dt, 0);
}

bool view::GameView::IsRenderingRiftExplosion() const {
	return m_riftExplosionSoundTriggered;
}

void view::GameView::DrawBeams(scene::Sprite &a_target, const float a_bossScale, const model::Enemy a_enemies[model::Model::g_maxEnemies]) {
	for (int eIx = 0; eIx < model::Model::g_maxEnemies; eIx++) {
		if (a_enemies[eIx].DoBeam()) {
			math::Vector2f pos;
			const float bossScale(a_bossScale);

			pos = m_core.m_riftPos + (a_enemies[eIx].m_pos - m_core.m_riftPos) * bossScale * bossScale * bossScale;
			if (a_enemies[eIx].m_beam.IsLethal()) {
				m_core.DrawBeam(a_target, a_enemies[eIx].m_pos, a_enemies[eIx].m_beam.m_width, a_enemies[eIx].m_beam.m_angle + PI_HALF, D3DCOLOR_ARGB(255, 255, 255, 255));
			} else {
				const unsigned char c(55 + (unsigned char)(200.0f * a_enemies[eIx].m_beam.GetNormalizedPowerupTimer()));
				m_core.DrawBeam(a_target, a_enemies[eIx].m_pos, a_enemies[eIx].m_beam.m_width * a_enemies[eIx].m_beam.GetNormalizedPowerupTimer(), a_enemies[eIx].m_beam.m_angle + PI_HALF, D3DCOLOR_ARGB(255, c, c, c));
			}
		}
	}
}

void view::GameView::DrawPlayerBullets(scene::Sprite &a_target, const model::Bullet a_playerBullets[model::Model::g_maxPlayerBullets]) {
 	for (int bIx = 0; bIx < model::Model::g_maxPlayerBullets; bIx++) {
		if (a_playerBullets[bIx].IsAlive() == true) {
			//m_core.DrawSprite(glow, m_model.m_playerBullets[bIx].m_pos, ViewAssets::UV_PlayerBullet, D3DCOLOR_ARGB(255, 255, 255, 255));

			
			const int lod[3] = {1, 5, 10};

			const int pCount(lod[m_core.m_assets.m_levelOfDetail]);
			const float t(0.002f);
			for (int i = 0; i < pCount; i++) {
				if (a_playerBullets[bIx].GetAge() - i * t > 0.0f) {
					math::Vector2f pos(a_playerBullets[bIx].GetPos() - a_playerBullets[bIx].m_speed * t * i);
					//pos.x += sin(pos.y * 0.25f) * (0.25f * i);
					const char c = (const char)(255.0f * ((float)(pCount - i) / (float)pCount));

					m_core.DrawSprite(a_target, pos, ViewAssets::UV_PlayerBullet, D3DCOLOR_ARGB(255, c, c, c), 0, 0.5f);
				}
			}

		}
	}
}

void view::GameView::SetGlowRenderState() const {
	dxrnd::DX::Device().SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	dxrnd::DX::Device().SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	dxrnd::DX::Device().SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	dxrnd::DX::Device().SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	dxrnd::DX::Device().SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
}

void view::GameView::RenderGlowSprite(scene::Sprite &a_sprite) const {

	SetGlowRenderState();
	a_sprite.Render(m_core.m_assets.m_sprites);
}

void view::GameView::RenderShockwaveSprite(scene::Sprite &a_sprite) const {
	a_sprite.Render(m_core.m_assets.m_shockWave);
}

void view::GameView::RenderPlayerDeath(const math::Vector2 &a_pos, const float a_time, const float a_hitTime) {
	const float dt(a_time - a_hitTime);
	scene::Sprite effects;
	effects.Create();
	if (dt <= 2.0f) {

		if (dt < 0.75f) {
			DoPlayerExplosion(effects, a_pos, dt / 0.75f);
			effects.Render(m_core.m_assets.m_explosion);
			effects.Clear();
		}
		DoExplosion(effects, a_pos, dt / 2.0f, 500.0f, 0);
		SetGlowRenderState();
		effects.Render(m_core.m_assets.m_shockWave2);
		effects.Clear();
	}
}

void view::GameView::DrawPlayer(const model::Player &a_player, float a_gunTimers[model::Player::g_maxGuns], view::GameParticles &a_particles, const float a_time, const math::Matrix44f &a_rotation, const math::Matrix44f &a_world, const float a_shield, float a_dt, int a_shipType) {

	static float engineTimer(0.0f);
	engineTimer -= a_dt;
	if (engineTimer < 0.0f) {
		const float dt(0.0035);
		for (float i = 1.0f; engineTimer < 0.0f; i += 1.0f) {
			view::Particle *pParticle(a_particles.FindFreeEngineParticle(a_time));
			const ViewAssets::PlayerViewData &playerData(m_core.m_assets.m_playerData[a_shipType]);
			if (pParticle) {
			
				pParticle->m_bornTime = a_time - dt * i;
				pParticle->m_startPos.Transform(playerData.m_lEnginePos, a_world);

				pParticle->m_startSize = playerData.m_engineParticleStartSize;
				pParticle->m_endSize = playerData.m_engineParticleEndSize;
				pParticle->m_speed = playerData.m_engineParticleSpeed;
				pParticle->m_ttl = 0.075f;
			} else {
				break;
			}
			pParticle = a_particles.FindFreeEngineParticle(a_time);
			if (pParticle) {	
				pParticle->m_bornTime = a_time - dt * i;
				pParticle->m_startPos.Transform(playerData.m_rEnginePos, a_world);

				pParticle->m_startSize = playerData.m_engineParticleStartSize;
				pParticle->m_endSize = playerData.m_engineParticleEndSize;
				pParticle->m_speed = playerData.m_engineParticleSpeed;
				pParticle->m_ttl = 0.075f;
			} else {
				break;
			}

			engineTimer += dt;
		}
	}

	unsigned int passCount;

	m_core.m_assets.m_playerEffect.Get()->SetVector("g_color", &(GetAbilityColor(a_player.m_ability, a_time)));
	m_core.m_assets.m_playerEffect.Get()->SetTexture(m_core.m_assets.m_playerEffect.m_textures[0].m_handle, m_core.m_assets.GetPlayerTexture(a_shipType).GetTexture());
	m_core.m_assets.m_playerEffect.Get()->SetTexture(m_core.m_assets.m_playerEffect.m_textures[1].m_handle, m_core.m_assets.GetPlayerSpecmap(a_shipType).GetTexture());
	m_core.m_assets.m_playerEffect.SetViewMatrix(m_core.m_view);
	m_core.m_assets.m_playerEffect.SetProjectionMatrix(m_core.m_projection);
	m_core.SetWorldMatrix(m_core.m_assets.m_playerEffect, a_world);

	if (m_core.m_assets.m_playerEffect.Get()->Begin(&passCount, 0)  == D3D_OK) {	

		for (unsigned int pIx = 0; pIx < passCount; pIx++) {
			if (m_core.m_assets.m_playerEffect.Get()->BeginPass(pIx) == D3D_OK) {

				for (size_t mIx = 0; mIx < m_core.m_assets.GetPlayerShip(a_shipType).GetMeshes().size(); mIx++) {
					m_core.m_assets.GetPlayerShip(a_shipType).GetMeshes()[mIx].Draw(m_core.m_assets.m_playerEffect.m_input);
				}
				
				m_core.m_assets.m_playerEffect.Get()->EndPass();
			}
		}
		m_core.m_assets.m_playerEffect.Get()->End();
	}

	// muzzle flashes
	for (int gIx = 0; gIx < model::Player::g_maxGuns; gIx++) {

		if (a_gunTimers[gIx] > 0) {
			view::AngledParticle *pParticle(a_particles.FindFreeMuzzleParticle(a_time));
			if (pParticle) {

				pParticle->m_startPos = m_core.m_assets.m_playerData[a_shipType].m_gunMounts[gIx];

				math::Vector3f offset;

				math::Vector2f dir;
				dir.x = a_player.m_guns[gIx].m_dir.x + g_width / 2.0f;
				dir.y = -a_player.m_guns[gIx].m_dir.y + g_height / 2.0f;

				math::Vector3f angle(World2Ship(dir));
				
				
				pParticle->m_startAngle = atan2f(angle.x, angle.y);
				offset.x = sinf(pParticle->m_startAngle) * 0.5f;
				offset.y = -cosf(pParticle->m_startAngle) * 0.5f;
				offset.z = 0.0f;

				pParticle->m_startPos += offset;
				pParticle->m_startSize = math::Vector2f(0.5f, 1.0f);
				pParticle->m_endSize = math::Vector2f(1.0f, 2.3f);
				pParticle->m_speed = math::Vector3f(0.0f, 0.0f, 0.0f);
				pParticle->m_ttl = 0.35f;
				pParticle->m_bornTime = a_time;
			}
			a_gunTimers[gIx] = -1.0f;
		}
	}
}


math::Vector4f view::GameView::GetAbilityColor(const model::Player::Ability a_ability, float a_time) const {
	static const Vector4f abilityColors[model::Player::A_Count] = {
		Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4f(-0.1f, 0.2f, 1.0f, 1.0f),
		Vector4f(1.0f, -0.1f, -0.1f, 1.0f),
		Vector4f(0.1f, 0.9f, 0.1f, 1.0f),
		Vector4f(0.4f, 0.3f, 1.0f, 1.0f)
	};

	return abilityColors[a_ability] * (cosf(a_time * 5.0f) + 1.0f) * 0.5f;
}

math::Vector4f view::GameView::GetAbilityColorNoNeg(model::Player::Ability a_ability) const {
	static const Vector4f abilityColors[model::Player::A_Count] = {
		Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
		Vector4f(0.0f, 0.2f, 1.0f, 1.0f),
		Vector4f(1.0f, 0.0f, 0.0f, 1.0f),
		Vector4f(0.1f, 0.9f, 0.1f, 1.0f),
		Vector4f(0.4f, 0.3f, 1.0f, 1.0f)
	};

	return abilityColors[a_ability];
}

math::Vector3f view::GameView::World2Ship(const math::Vector2f &a_worldPos) {
	dxrnd::D3DViewPort viewPort;
	dxrnd::DX::Device().GetViewport(&viewPort);

	math::Vector3f p(	a_worldPos.x * m_core.m_scale.x,
						a_worldPos.y * m_core.m_scale.y, 0.993104f);	// z value is just experimental to get out.z = 10.0f
	math::Vector3f out;
	D3DXVec3Unproject(&out, &p, &viewPort, &m_core.m_projection, &m_core.m_view, NULL);
	return out;
}

math::Vector2f view::GameView::Ship2World(math::Vector3f a_shipPos) {
	math::Vector3f pos;

	dxrnd::D3DViewPort viewPort;
	dxrnd::DX::Device().GetViewport(&viewPort);
	a_shipPos.z = 10.0f;
	
	D3DXVec3Project(&pos, &a_shipPos, &viewPort, &m_core.m_projection, &m_core.m_view, NULL);
	// now we have a screen coordinate i.e. scaled world coord

	// from scalled world coord to world coord.
	pos.x /= m_core.m_scale.x;
	pos.y /= m_core.m_scale.y;

	return math::Vector2f(pos.x - g_width * 0.5f, pos.y - g_height * 0.5f);
}

void view::GameView::DrawBonuses(scene::Sprite &a_target, const model::Bonus a_bonuses[model::Model::g_maxBonuses]) {
	for (int bIx = 0; bIx < model::Model::g_maxBonuses; bIx++) {
		if (a_bonuses[bIx].m_isAlive) {
			ViewAssets::UV uv;


			if (a_bonuses[bIx].m_type == model::Bonus::BT_Score) {
				uv = ViewAssets::UV_BonusScore;
			} else if (a_bonuses[bIx].m_type == model::Bonus::BT_Weapon) {
				uv = ViewAssets::UV_BonusWeapon;
			} else {
				uv = ViewAssets::UV_BonusShield;
			}
			m_core.DrawSprite(a_target, a_bonuses[bIx].m_pos, uv, D3DCOLOR_ARGB(255, 255, 255, 255));
		}
	}
}

void view::GameView::DrawEnemyBullets(scene::Sprite &a_glow, scene::Sprite &a_shockwave, const model::Bullet a_enemyBullets[model::Model::g_maxEnemyBullets], const float a_time) {
	for (int bIx = 0; bIx < model::Model::g_maxEnemyBullets; bIx++) {
		if (a_enemyBullets[bIx].IsAlive() == true) {
			m_core.DrawSprite(a_glow, a_enemyBullets[bIx].GetPos(), ViewAssets::UV_Bullet, D3DCOLOR_ARGB(255, 255, 255, 255), 0.0f, 0.5f);
			
			if (m_core.m_assets.m_levelOfDetail > 1) {
				const int pCount(8);
				const float t(0.01f);
				
				for (int i = 1; i < pCount; i++) {
					if (a_enemyBullets[bIx].GetAge() - i * t > 0.0f) {
						const float delta((float)(pCount - i) / (float)(pCount));
						const char c = (const char)(255.0f * delta);
						math::Vector2f direction(a_enemyBullets[bIx].m_speed);
						direction.Normalize();
						direction *= 200.0f;

						math::Vector2f pos(a_enemyBullets[bIx].GetPos() - direction * t * i);
						const float size(2.0f + 5.0f * (/*1.0f - */delta));

						pos.x *= m_core.m_scale.x;
						pos.y *= m_core.m_scale.y;

						if (a_shockwave.CanAdd(4) != true) {
							SetGlowRenderState();
							a_shockwave.Render(m_core.m_assets.m_shockWave);
							a_shockwave.Clear();
						}

						const float sign(i % 2 == 0 ? -1.5f : 2.0f);

						a_shockwave.AddSprite(pos, size + fabsf(sinf(a_enemyBullets[bIx].GetAge() * 1.3)) * size, size, sign * a_time * PI_DOUBLE + bIx + i, m_core.m_scale, scene::Sprite::UVMap(), D3DCOLOR_ARGB(255, c, c, c));
					}
				}
			}
		}
	}
}

void view::GameView::DrawEnemies(view::EnemySprite &a_target, scene::Sprite &a_explosionTarget, const model::Enemy a_enemies[model::Model::g_maxEnemies], const float a_enemyHitTimers[model::Model::g_maxEnemies], const float a_bossScale, const int a_levelIx, const float a_time, const bool a_doRenderDeath) {
	for (int eIx = 0; eIx < model::Model::g_maxEnemies; eIx++) {
		float dt(a_time - a_enemyHitTimers[eIx]);
		if (a_enemies[eIx].IsAlive() == true) {
			m_core.DrawEnemy(a_target, a_enemies[eIx], dt, a_levelIx, a_bossScale);

			if (dt <= 1.0f) {
				math::Vector2f pos(a_enemies[eIx].m_pos);
				float scale(model::Enemy::g_params[a_enemies[eIx].m_type].m_size.x * a_enemies[eIx].m_scale * 4.0f);
				DoEnemyDebris(a_target, pos, dt, scale, eIx, a_levelIx);
			}

		} else if (a_doRenderDeath && dt <= 1.0f) {
			math::Vector2f pos(a_enemies[eIx].m_pos);
			float scale(1.0f);
			if (a_enemies[eIx].m_waveIx < 0) {
				scale = model::Enemy::g_params[a_enemies[eIx].m_type].m_size.x * a_enemies[eIx].m_scale * 10.0f + 100.0f;
			} else {
				scale = model::Enemy::g_params[a_enemies[eIx].m_type].m_size.x * a_enemies[eIx].m_scale * 4.0f + 75.0f;
			}
			DoEnemyDebris(a_target, pos, dt, scale, eIx, a_levelIx);
			DoExplosion(a_explosionTarget, pos, dt, scale, eIx);
		}
	}
}

void view::GameView::DrawPlayerBulletDebris(view::EnemySprite &a_target, const model::Bullet a_playerBullets[model::Model::g_maxPlayerBullets], const float a_playerBulletHitTimers[model::Model::g_maxPlayerBullets], const float a_time, const int a_levelIx) {
	for (int bIx = 0; bIx < model::Model::g_maxPlayerBullets; bIx++) {
		const float dt((a_time - a_playerBulletHitTimers[bIx]) * 10.0f);
		if (dt <= 1.0f) {
			math::Vector2f pos(a_playerBullets[bIx].GetPos());
			float scale(8.0f);
			DoEnemyDebris(a_target, pos, dt, scale, bIx, a_levelIx);
		}
	}
}

void view::GameView::DoEnemyDebris(EnemySprite &a_debris, const math::Vector2f a_pos, float a_dt, float a_size, int a_offset, int a_level) {
	const unsigned char a((unsigned char)(255.0f * (1.0f - a_dt)));
	const unsigned char r((unsigned char)((float)m_core.g_enemyColors[a_level % m_core.g_maxEnemyColors].m_r * (1.0f - a_dt)));
	const unsigned char g((unsigned char)((float)m_core.g_enemyColors[a_level % m_core.g_maxEnemyColors].m_g * (1.0f - a_dt)));
	const unsigned char b((unsigned char)((float)m_core.g_enemyColors[a_level % m_core.g_maxEnemyColors].m_b * (1.0f - a_dt)));
	const DWORD color(D3DCOLOR_ARGB(a, r, g, b));
	math::Vector2f pos(a_pos);
	const float dt(1.0f - a_dt);
	const float sizeX(25 + a_size * (1.0f - (dt * dt * dt)));
	const float sizeY(25 + a_size * (1.0f - (dt * dt * dt)));

	pos.x *= m_core.m_scale.x;
	pos.y *= m_core.m_scale.y;

	srand(a_offset);
	const float sign(a_offset % 2 ? -1.0f :  1.0f);

	for (int i = 0; i < 16; i++) {
		float t((float)i / 16.0f);
		math::Vector2f pos;

		pos.x = cosf(sign * a_dt * 2.0f + t * PI_DOUBLE) * a_dt * a_size * (rand() % 25) * 0.10f;
		pos.y = sinf(sign * a_dt * 2.0f + t * PI_DOUBLE) * a_dt * a_size * (rand() % 25) * 0.10f + 50 * a_dt * a_dt;

		pos *= cosf(a_dt) * cosf(a_dt) * cosf(a_dt);
		pos += a_pos;

		pos.x *= m_core.m_scale.x;
		pos.y *= m_core.m_scale.y;

		const ViewAssets::UV uv((ViewAssets::UV)(ViewAssets::UV_Debris1 + rand() % 2));

		a_debris.AddEnemy(pos, m_core.GetSpriteSize(uv), m_core.m_scale, (rand() % 100) + a_dt * (rand() % 5), m_core.m_assets.m_uvs[uv], color);
		//m_core.DrawSprite(a_debris, pos, (ViewAssets::UV)(ViewAssets::UV_Debris1 + rand() % 2), color, (rand() % 100) + a_dt * (rand() % 5)	);
	}
	//DoExplosion(a_effect, a_pos, a_dt, a_size, a_offset);
	//a_sprite.AddSprite(pos, sizeX, sizeY, uv, color);
}

void view::GameView::RenderParticleEffects(GameParticles &a_particles, const math::Matrix44f &a_playerWorld, const float a_time) {
	view::ParticleDrawer particles(math::Vector3f(0.0f, 1.0f, 0.0f), math::Vector3f(1.0f, 0.0f, 0.0f), math::Vector3f(0.0f, 0.0f, 0.0f));

	a_particles.DrawEngineParticles(particles, m_core, a_time);
	a_particles.DrawMissileBeams(particles, m_core, a_time);

	m_core.m_assets.m_particleEffect.Get()->SetTexture(m_core.m_assets.m_particleEffect.m_textures[0].m_handle, m_core.m_assets.m_sprites.GetTexture());
	m_core.m_assets.m_particleEffect.SetViewMatrix(m_core.m_view);
	m_core.m_assets.m_particleEffect.SetProjectionMatrix(m_core.m_projection);
	{
		math::Matrix44f id;
		id.Id();
		m_core.m_assets.m_particleEffect.SetWorldMatrix(id);
	}
	particles.Draw(m_core.m_assets.m_particleEffect);

	particles.Clear();
	a_particles.DrawMuzzleFlashes(particles, m_core, a_time);
	m_core.m_assets.m_particleEffect.SetWorldMatrix(a_playerWorld);
	particles.Draw(m_core.m_assets.m_particleEffect);
}

void view::GameView::RenderBriefingAbilityIcon(const math::Vector2f &a_pos, const int a_levelIx) {
	dxrnd::DX::Device().SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	dxrnd::DX::Device().SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	dxrnd::DX::Device().SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	dxrnd::DX::Device().SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	{
		scene::Sprite bg;
		bg.Create();
		bg.AddSprite(a_pos, 128.0f * m_core.m_scale.x, 128.0f * m_core.m_scale.y, GetAbilityColorNoNeg((model::Player::Ability)(a_levelIx + 2)));
		bg.Render(m_core.m_assets.m_abilityIcons[a_levelIx + 1]);
	}
						
	dxrnd::DX::Device().SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	dxrnd::DX::Device().SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
}

void view::GameView::RenderBossWarning(const float a_bossTimer, const float a_time) {
	const float scaleIn(GetNormalizedPeriod(a_bossTimer, 4.75f, 10.0f));
	const float scale(scaleIn > 0.0f ? scaleIn * 128.0f : 1.0f);
	scene::Sprite warning;
	warning.Create();
	float width(400 * scale);
	float height(200 * scale);
	warning.AddSprite(math::Vector2f((g_width / 2 - width * 0.5f) * m_core.m_scale.x, (g_height / 2 - height * 0.5f - 50) * m_core.m_scale.y), width * m_core.m_scale.x, height * m_core.m_scale.y);
	dxrnd::DX::Device().SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dxrnd::DX::Device().SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	warning.Render(m_core.m_assets.m_warning);
	warning.Clear();

	const float offset(sin(a_time * 3.0f) * 10.0f);
	width += offset;

	warning.AddSprite(math::Vector2f((g_width / 2 - width * 0.5f) * m_core.m_scale.x, (g_height / 2 - height * 0.5f - 50) * m_core.m_scale.y), width * m_core.m_scale.x, height * m_core.m_scale.y);
	dxrnd::DX::Device().SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dxrnd::DX::Device().SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	warning.Render(m_core.m_assets.m_warning);
}

void view::GameView::RenderHUD(const int a_scoreMultiplier, const int a_score, const int a_lifeCount) {
scene::Sprite hud;
	hud.Create();
	cmn::String score;
					
					
	const float width(16.0f * m_core.m_scale.x), height(16.0f * m_core.m_scale.y);

	score.ToString((int)a_scoreMultiplier);
	math::Vector2f pos((float)(g_width - 8) * m_core.m_scale.x - (1 + score.size()) * width, (float)(g_height - 8) * m_core.m_scale.y - 4.0f * height);
	hud.AddSprite(pos, width, height, m_core.m_assets.m_hudUVs[ViewAssets::HUV_x]);
	pos.x += width;
	for (size_t cIx = 0; cIx < score.size(); cIx++) {
		hud.AddSprite(pos, width, height, m_core.m_assets.m_hudUVs[score[cIx] - '0']);
		pos.x += width;
	}

	score.ToString(a_score);
	pos.Set((float)(g_width - 8) * m_core.m_scale.x - score.size() * width, (float)(g_height - 8) * m_core.m_scale.y - 1.0f * height);
	for (size_t cIx = 0; cIx < score.size(); cIx++) {
		hud.AddSprite(pos, width, height, m_core.m_assets.m_hudUVs[score[cIx] - '0']);
		pos.x += width;
	}

	pos.y -= height * 1.5f;
	pos.x = (float)(g_width - 8) * m_core.m_scale.x - width;
	for (int i = 0; i < a_lifeCount - 1; i++) {
		hud.AddSprite(pos, width, height, m_core.m_assets.m_hudUVs[ViewAssets::HUV_LifeFull]);
		pos.x -= width;
	}

	dxrnd::DX::Device().SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dxrnd::DX::Device().SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	hud.Render(m_core.m_assets.m_hud);
}

void view::GameView::RenderAbilityIcons(const model::Player &a_player, const float a_time, const float a_abilityChangedTime, const int a_levelIx) {
	scene::Sprite abilityIcon;
	abilityIcon.Create();
	dxrnd::DX::Device().SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	dxrnd::DX::Device().SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	dxrnd::DX::Device().SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dxrnd::DX::Device().SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	{
		math::Vector2f pos((g_width * 0.5f - 38.0f * 2.0f) * m_core.m_scale.x, 4.0f * m_core.m_scale.y);
		const float width(24.0f * m_core.m_scale.x), height(24.0f * m_core.m_scale.y);
		pos.x += width * 0.25f;
		for (int aIx = 0; aIx < model::Player::A_Count - 1; aIx++) {
			if (a_player.CanUseAbility((model::Player::Ability)(aIx + 1), a_levelIx)) {
				abilityIcon.AddSprite(pos, width, height, math::Vector4f(0.5, 0.5, 0.5, 1.0));
				if (aIx == a_player.m_ability - 1) {
					abilityIcon.AddSprite(pos, width, height, GetAbilityColorNoNeg(a_player.m_ability));
				}
			} else {
				abilityIcon.AddSprite(pos, width, height, math::Vector4f(0.15, 0.15, 0.15, 1.0));
			}
			abilityIcon.Render(m_core.m_assets.m_abilityIcons[aIx]);
			abilityIcon.Clear();
			pos.x += 38.0f * m_core.m_scale.x;
		}
	}		

	const float abilityDt(a_time - a_abilityChangedTime);
	if (abilityDt < 1.0f) {

		const float dc(cosf(abilityDt * PI_HALF));
		const float width(32.0f * m_core.m_scale.x), height(32.0f * m_core.m_scale.y);
					
		math::Vector2f pos((a_player.m_pos.x) * m_core.m_scale.x - width * 2.0f, (a_player.m_pos.y - 96.0f) * m_core.m_scale.y);

		if (pos.y < 20.0f * m_core.m_scale.y) {
			pos.y = (a_player.m_pos.y + 96.0f) * m_core.m_scale.y;
		}
		if (pos.x < 0.0f) {
			pos.x = 0.0f;
		} else if (pos.x + width * 4.0f > m_core.m_width) {
			pos.x = m_core.m_width - width * 4.0f;
		}

		for (int aIx = 0; aIx < model::Player::A_Count - 1; aIx++) {
			abilityIcon.AddSprite(pos, width , height, math::Vector4f(0.5, 0.5, 0.5, 1.0) * dc);
			if (aIx == a_player.m_ability - 1) {
				abilityIcon.AddSprite(pos, width, height, GetAbilityColorNoNeg(a_player.m_ability) * dc);
			}
			pos.x += width;
			abilityIcon.Render(m_core.m_assets.m_abilityIcons[aIx]);
			abilityIcon.Clear();
		}
	}

	dxrnd::DX::Device().SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	dxrnd::DX::Device().SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
}

void view::GameView::SetNormalRenderState() {
	dxrnd::DX::Device().SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	dxrnd::DX::Device().SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
}


void view::GameView::AddPlayerHealth(const float a_percent, HealthAnimationState &a_health, const float a_playerHitTimer, const float a_time, const float a_dt) {
	const float width(300.0f);
	const float height(10.0f);
	const float scaledMax(width * m_core.m_scale.x);
	const float scaledHeight(height * m_core.m_scale.y);

	const math::Vector2f barPos((g_width / 4 - width / 2 - 30) * m_core.m_scale.x, 10.0f * m_core.m_scale.y);
	
	if (a_health.m_health < 0) {
		a_health.m_health = 0;
	} else if (a_health.m_health > scaledMax - 0.5f) {
		a_health.m_health = scaledMax;
	}

	float percent((int)(a_percent * scaledMax));
	percent = a_health.m_health + (percent - a_health.m_health) * a_dt * 10.0f;
	a_health.m_health = percent;

	math::Vector4c leftColor(255, 204, 197);
	math::Vector4c rightColor(255, 50, 25);

	m_bars.AddBarLeftRightColors(barPos + math::Vector2f(-1, -1), barPos + math::Vector2f(scaledMax + 1, scaledHeight + 1), rightColor, leftColor);

	if (a_time - a_playerHitTimer <= 1.0f) {
		float m(1.0f - (a_time - a_playerHitTimer));
		math::Vector4c white(255, 255, 255);
		rightColor.Interpolate(rightColor, white, m);
		leftColor.Interpolate(leftColor, white, m);
	}
					
	m_bars.AddBar(barPos + math::Vector2f(0, 0), barPos + math::Vector2f(scaledMax - percent, scaledHeight), m_core.ColorARGB(255, 75, 75, 75));

	leftColor.Interpolate(rightColor, leftColor, percent / scaledMax);

	m_bars.AddBarLeftRightColors(barPos + math::Vector2f(scaledMax - percent, 0), barPos + math::Vector2f(scaledMax, scaledHeight), leftColor, rightColor);
}

void view::GameView::AddPlayerEnergy(const float a_percent, EnergyAnimationState &a_energy, const float a_energyPickUpTime, const float a_time, const float a_dt) {
	const float width(300.0f);
	const float scaledMax(width * m_core.m_scale.x);
	const float scaledHeight(10.0f * m_core.m_scale.y);
					
	const math::Vector2f barPos((g_width / 2 + g_width / 4 - width / 2 + 30) * m_core.m_scale.x, 10.0f * m_core.m_scale.y);
				
	if (a_energy.m_energy < 0) {
		a_energy.m_energy = 0;
	} else if (a_energy.m_energy > scaledMax) {
		a_energy.m_energy = scaledMax;
	}
	math::Vector4c leftColor(80, 185, 52);
	math::Vector4c rightColor(211, 237, 204);

	m_bars.AddBarLeftRightColors(barPos + math::Vector2f(-1, -1), barPos + math::Vector2f(scaledMax + 1, scaledHeight + 1), rightColor, leftColor);

	if (a_time - a_energyPickUpTime <= 1.0f) {
		float m(1.0f - (a_time - a_energyPickUpTime));
		math::Vector4c white(255, 255, 255);
		rightColor.Interpolate(rightColor, white, m);
		leftColor.Interpolate(leftColor, white, m);
	}
	float percent((int)(a_percent * scaledMax));
	percent = a_energy.m_energy + (percent - a_energy.m_energy) * a_dt * 10.0f;
	a_energy.m_energy = percent;
					
	m_bars.AddBar(barPos + math::Vector2f(percent,0), barPos + math::Vector2f(scaledMax, scaledHeight), m_core.ColorARGB(255, 75, 75, 75));
					
	rightColor.Interpolate(leftColor, rightColor, percent / scaledMax);
	m_bars.AddBarLeftRightColors(barPos + math::Vector2f(0,0), barPos + math::Vector2f(percent, scaledHeight), leftColor, rightColor);
}

void view::GameView::AddBossStatus(const float a_lifeLeft, const float a_timeLeft) {
	const float width(300.0f);
	const float thickness(4.0f);
	const float scaledMax(width * m_core.m_scale.x);
	math::Vector2f barPos((g_width / 4 - width / 2 - 30) * m_core.m_scale.x, 35.0f * m_core.m_scale.y);
	m_bars.AddBar(barPos, barPos + math::Vector2f(scaledMax, thickness * m_core.m_scale.y), m_core.ColorARGB(255, 75, 75, 75));
	m_bars.AddBar(barPos + math::Vector2f(scaledMax * (1.0f - a_lifeLeft), 0), barPos + math::Vector2f(scaledMax, thickness * m_core.m_scale.y), m_core.ColorARGB(255, 255, 50, 25));

	barPos.x = (g_width / 2 + g_width / 4 - width / 2 + 30) * m_core.m_scale.x;
	m_bars.AddBar(barPos, barPos + math::Vector2f(scaledMax, thickness * m_core.m_scale.y), D3DCOLOR_ARGB(255, 75, 75, 75));
	m_bars.AddBar(barPos, barPos + math::Vector2f(scaledMax * a_timeLeft, thickness * m_core.m_scale.y), m_core.ColorARGB(255, 100, 150, 255));
}

void view::GameView::EndFrame() {
	m_bars.Render(m_core.m_assets.m_barEffect);
	m_bars.Clear();
}