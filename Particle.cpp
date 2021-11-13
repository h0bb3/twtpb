// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "Particle.h"



namespace view {
Particle::Particle(void) {
	m_ttl = 0.0f;
	m_bornTime = FLT_MAX;
}

Particle::~Particle(void) {
}

bool Particle::IsAlive(float a_gameTime) const {
	return a_gameTime < m_bornTime + m_ttl && a_gameTime >= m_bornTime;
}

math::Vector3f Particle::GetPos(float a_gameTime) const {
	return m_startPos + m_speed * (a_gameTime - m_bornTime) / m_ttl;
}

math::Vector2f Particle::GetSize(float a_gameTime) const {
	return m_startSize + (m_endSize - m_startSize) * (a_gameTime - m_bornTime) / m_ttl;
}
dxrnd::Color Particle::GetColor(float a_gameTime) const {
	const float dt((a_gameTime - m_bornTime) / m_ttl);
	const char c((char)(255.0f - 255.0f * dt));
	return D3DCOLOR_ARGB(c, c, c, c);
}

ParticleDrawer::ParticleDrawer(const math::Vector3f &a_up, const math::Vector3f &a_right, const math::Vector3f &a_eyePos) :
m_up(a_up), m_right(a_right), m_eyePos(a_eyePos), m_front(a_right.Cross(a_up)) {
	m_verts = new Vertex_PTC[g_maxParticles * 4];
}

ParticleDrawer::~ParticleDrawer() {
	delete [] m_verts;
}

void ParticleDrawer::AddParticle(const Particle &a_particle, const float a_gameTime, const scene::Sprite::UVMap &a_uv) {
	const math::Vector3f pos(a_particle.GetPos(a_gameTime));
	const math::Vector2f size(a_particle.GetSize(a_gameTime));
	const COLORREF color(a_particle.GetColor(a_gameTime));

	m_verts[m_nextVert].m_pos = pos - m_right * size.x + m_up * size.y;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv = a_uv.m_topLeftUV;
	m_nextVert++;

	m_verts[m_nextVert].m_pos = pos + m_right * size.x + m_up * size.y;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv.Set(a_uv.m_bottomRightUV.x, a_uv.m_topLeftUV.y);
	m_nextVert++;

	m_verts[m_nextVert].m_pos = pos - m_right * size.x - m_up * size.y;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv.Set(a_uv.m_topLeftUV.x, a_uv.m_bottomRightUV.y);
	m_nextVert++;

	m_verts[m_nextVert].m_pos = pos + m_right * size.x - m_up * size.y;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv = a_uv.m_bottomRightUV;
	m_nextVert++;
}

void RotateZ(const math::Vector3f &a_source, math::Vector3f &a_dest, const float a_cosAngle, const float a_sinAngle) {
	a_dest.x = a_source.y * a_sinAngle - a_source.x * a_cosAngle;
	a_dest.y = a_source.x * a_sinAngle - a_source.y * a_cosAngle;
	a_dest.z = a_source.z;
	//a_dest = a_source;
}

void ParticleDrawer::AddParticle(const AngledParticle &a_particle, const float a_gameTime, const scene::Sprite::UVMap &a_uv) {
	const math::Vector3f pos(a_particle.GetPos(a_gameTime));
	const math::Vector2f size(a_particle.GetSize(a_gameTime));
	const dxrnd::Color color(a_particle.GetColor(a_gameTime));
	math::Vector3f vertex;

	const float c(cosf(a_particle.m_startAngle));
	const float s(sinf(a_particle.m_startAngle));
	
	RotateZ(m_right * size.x - m_up * size.y, m_verts[m_nextVert].m_pos, c, s);
	m_verts[m_nextVert].m_pos = pos - m_verts[m_nextVert].m_pos;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv = a_uv.m_topLeftUV;
	m_nextVert++;

	RotateZ(m_right * size.x + m_up * size.y, m_verts[m_nextVert].m_pos, c, s);
	m_verts[m_nextVert].m_pos = pos + m_verts[m_nextVert].m_pos;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv.Set(a_uv.m_bottomRightUV.x, a_uv.m_topLeftUV.y);
	m_nextVert++;

	RotateZ(m_right * size.x + m_up * size.y, m_verts[m_nextVert].m_pos, c, s);
	m_verts[m_nextVert].m_pos = pos - m_verts[m_nextVert].m_pos;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv.Set(a_uv.m_topLeftUV.x, a_uv.m_bottomRightUV.y);
	m_nextVert++;

	RotateZ(m_right * size.x - m_up * size.y, m_verts[m_nextVert].m_pos, c, s);
	m_verts[m_nextVert].m_pos = pos + m_verts[m_nextVert].m_pos;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv = a_uv.m_bottomRightUV;
	m_nextVert++;
}

void ParticleDrawer::AddParticle(const BeamParticle &a_particle, const float a_gameTime, const scene::Sprite::UVMap &a_uv) {
	const math::Vector3f pos(a_particle.GetPos(a_gameTime));
	const math::Vector2f size(a_particle.GetSize(a_gameTime));
	const dxrnd::Color color(a_particle.GetColor(a_gameTime));
	const float middleU(a_uv.m_topLeftUV.x + (a_uv.m_bottomRightUV.x - a_uv.m_topLeftUV.x) * 0.5f);

	math::Vector3f	v1 = math::Vector3f(size.x * 1.5f, size.y, 0);
	math::Vector3f	v2 = math::Vector3f(size.x * 1.5f, -size.y, 0);
	math::Vector3f	v3 = math::Vector3f(-size.x * 1.5f, size.y, 0);
	math::Vector3f	v4 = math::Vector3f(-size.x * 1.5f, -size.y, 0);

	//math::Vector3f	v5 = math::Vector3f(-size.x * 0.5f, size.y, 0);
	//math::Vector3f	v6 = math::Vector3f(-size.x * 0.5f, -size.y, 0);
	//math::Vector3f	v7 = math::Vector3f(size.x * 0.5f, size.y, 0);
	//math::Vector3f	v8 = math::Vector3f(size.x * 0.5f, -size.y, 0);

	math::Vector3f vEye(m_eyePos - pos);
	math::Vector3f vP(a_particle.m_relativeEndPos.Cross(vEye));

	math::Vector3f right(m_front.Cross(vP));
	right.Normalize();
	math::Vector3f up(m_front.Cross(right));

	math::Matrix44f m;

	m.Id();
	m.Set(m_front, up, right);
	//m.Translation(pos);
	m._41 = pos.x;
	m._42 = pos.y;
	m._43 = pos.z;
	m._44 = 1.0f;

	v1.Transform(m);
	v2.Transform(m);
//	v5.Transform(m);
//	v6.Transform(m);

	//m.Translation(pos + a_particle.m_relativeEndPos);
	m._41 = pos.x + a_particle.m_relativeEndPos.x;
	m._42 = pos.y + a_particle.m_relativeEndPos.y;
	m._43 = pos.z + a_particle.m_relativeEndPos.z;

	v3.Transform(m);
	v4.Transform(m);
//	v7.Transform(m);
//	v8.Transform(m);


	m_verts[m_nextVert].m_pos = v1;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv = a_uv.m_topLeftUV;
	m_nextVert++;

	m_verts[m_nextVert].m_pos = v3;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv.Set(a_uv.m_bottomRightUV.x, a_uv.m_topLeftUV.y);
	m_nextVert++;

	m_verts[m_nextVert].m_pos = v2;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv.Set(a_uv.m_topLeftUV.x, a_uv.m_bottomRightUV.y);
	m_nextVert++;

	m_verts[m_nextVert].m_pos = v4;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv = a_uv.m_bottomRightUV;
	m_nextVert++;

/*	m_verts[m_nextVert].m_pos = v5;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv.Set(middleU, a_uv.m_topLeftUV.y);
	m_nextVert++;

	m_verts[m_nextVert].m_pos = v7;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv.Set(middleU, a_uv.m_topLeftUV.y);
	m_nextVert++;

	m_verts[m_nextVert].m_pos = v6;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv.Set(middleU, a_uv.m_bottomRightUV.y);
	m_nextVert++;

	m_verts[m_nextVert].m_pos = v8;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv.Set(middleU, a_uv.m_bottomRightUV.y);
	m_nextVert++;
*/

/*	m_verts[m_nextVert].m_pos = v1;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv = a_uv.m_topLeftUV;
	m_nextVert++;

	m_verts[m_nextVert].m_pos = v5;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv.Set(middleU, a_uv.m_topLeftUV.y);
	m_nextVert++;

	m_verts[m_nextVert].m_pos = v2;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv.Set(a_uv.m_topLeftUV.x, a_uv.m_bottomRightUV.y);
	m_nextVert++;

	m_verts[m_nextVert].m_pos = v6;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv.Set(middleU, a_uv.m_bottomRightUV.y);
	m_nextVert++;

	m_verts[m_nextVert].m_pos = v5;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv.Set(middleU, a_uv.m_topLeftUV.y);
	m_nextVert++;

	m_verts[m_nextVert].m_pos = v7;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv.Set(middleU, a_uv.m_topLeftUV.y);
	m_nextVert++;

	m_verts[m_nextVert].m_pos = v6;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv.Set(middleU, a_uv.m_bottomRightUV.y);
	m_nextVert++;

	m_verts[m_nextVert].m_pos = v8;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv.Set(middleU, a_uv.m_bottomRightUV.y);
	m_nextVert++;


	m_verts[m_nextVert].m_pos = v7;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv.Set(middleU, a_uv.m_topLeftUV.y);
	m_nextVert++;

	m_verts[m_nextVert].m_pos = v3;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv.Set(a_uv.m_bottomRightUV.x, a_uv.m_topLeftUV.y);
	m_nextVert++;

	m_verts[m_nextVert].m_pos = v8;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv.Set(middleU, a_uv.m_bottomRightUV.y);
	m_nextVert++;

	m_verts[m_nextVert].m_pos = v4;
	m_verts[m_nextVert].m_color = color;
	m_verts[m_nextVert].m_uv.Set(a_uv.m_bottomRightUV.x, a_uv.m_bottomRightUV.y);
	m_nextVert++;*/

}

void ParticleDrawer::Draw(dxrnd::Effect &a_effect) {
	HRESULT ret;
	
	ret = DoRender(Vertex_PTC::g_elements, m_verts, sizeof(Vertex_PTC), a_effect);
	if (ret != D3D_OK) {
		LOG_3("ParticleDrawer : Failed to Draw");
		Clear();
	}
}

}