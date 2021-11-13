// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "GameParticles.h"

namespace view {

GameParticles::GameParticles(void) {
}

GameParticles::~GameParticles(void) {
}

Particle* GameParticles::FindFreeEngineParticle(float a_gameTime) {
	return FindFreeParticle(m_engineParticles, g_maxEngineParticles, a_gameTime);
}

BeamParticle* GameParticles::FindFreeBeamParticle(float a_gameTime) {
	return FindFreeParticle(m_beamParticles, g_maxBeamParticles, a_gameTime);
}

AngledParticle *GameParticles::FindFreeMuzzleParticle(float a_gameTime) {
	return FindFreeParticle(m_muzzleParticles, g_maxMuzzleParticles, a_gameTime);
}

void GameParticles::DrawMuzzleFlashes(ParticleDrawer &a_drawer, ViewCore &a_core, float a_gameTime) {
	DrawParticles(m_muzzleParticles, g_maxMuzzleParticles, a_drawer, a_gameTime, a_core.m_assets.m_uvs[ViewAssets::UV_Muzzle]);
}

void GameParticles::DrawEngineParticles(ParticleDrawer &a_drawer, ViewCore &a_core, float a_gameTime) {
	DrawParticles(m_engineParticles, g_maxEngineParticles, a_drawer, a_gameTime, a_core.m_assets.m_uvs[ViewAssets::UV_EngineParticle]);
}

void GameParticles::DrawMissileBeams(ParticleDrawer &a_drawer, ViewCore &a_core, float a_gameTime) {
	DrawParticles(m_beamParticles, g_maxBeamParticles, a_drawer, a_gameTime, a_core.m_assets.m_uvs[ViewAssets::UV_EngineParticle]);
}

template<class T> T* GameParticles::FindFreeParticle(T *a_particles, const int a_maxCount, const float a_gameTime) {
	for (int i = 0; i < a_maxCount; i++) {
		if (a_particles[i].IsAlive(a_gameTime) != true) {
			return &(a_particles[i]);
		}
	}

	return NULL;
}

template<class T> void GameParticles::DrawParticles(const T *a_particles, const int a_maxCount, ParticleDrawer &a_drawer, const float a_gameTime, const scene::Sprite::UVMap &a_uvs) {
	for (int i = 0; i < a_maxCount; i++) {
		if (a_particles[i].IsAlive(a_gameTime)) {
			a_drawer.AddParticle(a_particles[i], a_gameTime, a_uvs);
		}
	}
}

}