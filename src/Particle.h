// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include <Scene/Sprite.h>
#include "HLSLSprite.h"
#include "VertexTypes.h"

#include <DXRenderer/DXDefines.h>

namespace view {
class Particle {
	public:
		Particle(void);
		~Particle(void);

		bool IsAlive(float a_gameTime) const;
		dxrnd::Color GetColor(float a_gameTime) const;
		math::Vector3f GetPos(float a_gameTime) const;
		math::Vector2f GetSize(float a_gameTime) const;

		float m_bornTime;
		math::Vector3f m_startPos;
		math::Vector3f m_speed;
		math::Vector2f m_startSize;
		math::Vector2f m_endSize;
		float m_ttl;	// time to live
};

class BeamParticle : public Particle {
	public:
		math::Vector3f m_relativeEndPos;
};

class AngledParticle : public Particle {
	public:
		float m_startAngle;
};

class ParticleDrawer : public HLSLSprite {
	public:
		ParticleDrawer(const math::Vector3f &a_up, const math::Vector3f &a_right, const math::Vector3f &a_eyePos);
		~ParticleDrawer();

		void ParticleDrawer::Draw(dxrnd::Effect &a_effect);
		void AddParticle(const Particle &a_particle, const float a_gameTime, const scene::Sprite::UVMap &a_uv);
		void AddParticle(const BeamParticle &a_particle, const float a_gameTime, const scene::Sprite::UVMap &a_uv);
		void AddParticle(const AngledParticle &a_particle, const float a_gameTime, const scene::Sprite::UVMap &a_uv);
		static const int g_maxParticles = 4096;

		Vertex_PTC *m_verts;	// needs to be on heap
	private:
		const math::Vector3f m_up;
		const math::Vector3f m_right;
		const math::Vector3f m_front;
		const math::Vector3f m_eyePos;
};

}
