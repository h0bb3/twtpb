// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "Particle.h"
#include "Model.h"
#include "ViewCore.h"

namespace view {

class GameParticles {
	public:
		GameParticles(void);
		~GameParticles(void);

		Particle* FindFreeEngineParticle(float a_gameTime);
		BeamParticle* FindFreeBeamParticle(float a_gameTime);
		AngledParticle *FindFreeMuzzleParticle(float a_gameTime);


		void DrawMuzzleFlashes(ParticleDrawer &a_drawer, ViewCore &a_core, float a_gameTime);
		void DrawEngineParticles(ParticleDrawer &a_drawer, ViewCore &a_core, float a_gameTime);
		void DrawMissileBeams(ParticleDrawer &a_drawer, ViewCore &a_core, float a_gameTime);

		const static int g_maxEngineParticles = 512;
		const static int g_maxMuzzleParticles = 32;
		const static int g_maxBeamParticles = 2048;

		Particle m_engineParticles[g_maxEngineParticles];
		BeamParticle m_beamParticles[g_maxBeamParticles];
		math::Vector2f m_missileOldPositions[model::Model::g_maxMissiles];
		AngledParticle m_muzzleParticles[g_maxMuzzleParticles];

	private:
		template<class T> T* FindFreeParticle(T *a_particles, const int a_maxCount, const float a_gameTime);
		template<class T> void DrawParticles(const T *a_particles, const int a_maxCount, ParticleDrawer &a_drawer, const float a_gameTime, const scene::Sprite::UVMap &a_uvs);
};

}
