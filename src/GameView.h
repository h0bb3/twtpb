// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "ViewCore.h"
#include "BarSprite.h"

namespace view {

class GameParticles;
class EnemySprite;

class GameView {
	public:
		class EnergyAnimationState {
			public:
				EnergyAnimationState() { m_energy = 0.0f; }
				void Reset() { m_energy = 0.0f; };
				float m_energy;
		};
		class HealthAnimationState {
			public:
				HealthAnimationState() { m_health = 0.0f; }
				void Reset() { m_health = 0.0f; }
				float m_health;
		};
		
		GameView(ViewCore &a_core);
		void InitFrame();
		void EndFrame();

		void RenderRift(float a_time, float a_bossKilledTime, float a_bossTimer, bool a_bossIsAlive, bool a_bossIsAgressive);
		void RenderGlowLayer(const model::Model &a_model, float a_time, const float a_enemyHitTimers[model::Model::g_maxEnemies], const float a_playerBulletHitTimers[model::Model::g_maxPlayerBullets]);

		void DoExplosion(scene::Sprite &a_effect, const math::Vector2f a_pos, float a_dt, float a_size, int a_offset);
		void DoPlayerExplosion(scene::Sprite &a_effect, const math::Vector2f a_pos, float a_dt);
		
		void RenderGlowSprite(scene::Sprite &a_sprite) const;
		void RenderShockwaveSprite(scene::Sprite &a_sprite) const;

		void DrawBeams(scene::Sprite &a_target, const float a_bossScale, const model::Enemy a_enemies[model::Model::g_maxEnemies]);
		void DrawPlayerBullets(scene::Sprite &a_target, const model::Bullet a_playerBullets[model::Model::g_maxPlayerBullets]);
		void DrawBonuses(scene::Sprite &a_target, const model::Bonus a_bonuses[model::Model::g_maxBonuses]);
		void DrawEnemyBullets(scene::Sprite &a_glow, scene::Sprite &a_shockwave, model::Bullet const a_enemyBullets[model::Model::g_maxEnemyBullets], const float a_time);

		void DrawEnemies(EnemySprite &a_target, scene::Sprite &a_explosionTarget, const model::Enemy a_enemies[model::Model::g_maxEnemies], const float a_enemyHitTimers[model::Model::g_maxEnemies], const float a_bossScale, const int a_levelIx, const float a_time, const bool a_doRenderDeath);

		void DrawPlayerBulletDebris(view::EnemySprite &a_target, const model::Bullet a_playerBullets[model::Model::g_maxPlayerBullets], const float a_playerBulletHitTimers[model::Model::g_maxPlayerBullets], const float a_time, const int a_levelIx);

		void DoEnemyDebris(EnemySprite &a_debris, const math::Vector2f a_pos, float a_dt, float a_size, int a_offset, int a_level);

		void RenderPlayerDeath(const math::Vector2 &a_pos, const float a_time, const float a_hitTime);

		void DrawPlayer(const model::Player &a_player, float a_gunTimers[model::Player::g_maxGuns], GameParticles &a_particles, const float a_time, const math::Matrix44f &a_rotation, const math::Matrix44f &a_world, const float a_shield, float a_dt, int a_shipType);

		bool IsRenderingRiftExplosion() const;

		void RenderScore(const math::Vector2f &a_pos, const cmn::String &a_score, const float a_width, const float a_height);

		void RenderParticleEffects(GameParticles &a_particles, const math::Matrix44f &a_playerWorld, const float a_time);
		void RenderBriefingAbilityIcon(const math::Vector2f &a_pos, const int a_levelIx);

		void SetNormalRenderState();
		void RenderBossWarning(const float a_bossTimer, const float a_time);
		void RenderHUD(const int a_scoreMultiplier, const int a_score, const int a_lifeCount);
		void RenderAbilityIcons(const model::Player &a_player, const float a_time, const float a_abilityChangedTime, const int a_levelIx);

		math::Vector3f World2Ship(const math::Vector2f &a_worldPos);
		math::Vector2f Ship2World(math::Vector3f a_shipPos);
		math::Vector4f GetAbilityColor(const model::Player::Ability a_ability, float a_time) const;
		math::Vector4f GetAbilityColorNoNeg(model::Player::Ability a_ability) const;

		void AddPlayerHealth(const float a_percent, HealthAnimationState &a_health, const float a_playerHitTimer, const float a_time, const float a_dt);
		void AddPlayerEnergy(const float a_percent, EnergyAnimationState &a_energy, const float a_energyPickupTime, const float a_time, const float a_dt);
		void AddBossStatus(const float a_lifeLeft, const float a_timeLeft);

	private:

		void SetGlowRenderState() const;
		

		float GetNormalizedPeriod(float a_time, float a_lo, float a_hi);
		void DrawExplosion(scene::Sprite &a_effect, const math::Vector2f a_pos, float a_dt, float a_angle);
		ViewCore &m_core;
		bool m_riftExplosionSoundTriggered;

		BarSprite m_bars;
		
		
};

}