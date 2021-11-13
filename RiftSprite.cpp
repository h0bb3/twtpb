// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "RiftSprite.h"
#include <Scene/Sprite.h>


namespace view {

RiftSprite::RiftSprite(void) {
	m_verts = new Vertex_PTC[g_maxSprites * 4];
}

RiftSprite::~RiftSprite(void) {
	delete [] m_verts;
}

void RiftSprite::AddRift(const math::Vector2f &a_centerPos, DWORD a_color, const math::Vector2f &a_size) {
	scene::Sprite::UVMap uv;

	uv.m_topLeftUV.Set(0.0f, 0.0f);
	uv.m_bottomRightUV.Set(1.0f, 1.0f);


	m_verts[m_nextVert].m_pos.Set(a_centerPos.x + a_size.x * -0.5f, a_centerPos.y + a_size.y * -0.5f, 0.0f);
	m_verts[m_nextVert].m_uv = uv.m_topLeftUV;
	m_verts[m_nextVert].m_color = a_color;
	m_nextVert++;

	m_verts[m_nextVert].m_pos.Set(a_centerPos.x + a_size.x * 0.5f, a_centerPos.y + a_size.y * -0.5f, 0.0f);
	m_verts[m_nextVert].m_uv.Set(uv.m_bottomRightUV.x, uv.m_topLeftUV.y);
	m_verts[m_nextVert].m_color = a_color;
	m_nextVert++;

	m_verts[m_nextVert].m_pos.Set(a_centerPos.x + a_size.x * -0.5f, a_centerPos.y + a_size.y * 0.5f, 0.0f);
	m_verts[m_nextVert].m_uv.Set(uv.m_topLeftUV.x, uv.m_bottomRightUV.y);
	m_verts[m_nextVert].m_color = a_color;
	m_nextVert++;

	m_verts[m_nextVert].m_pos.Set(a_centerPos.x + a_size.x * 0.5f, a_centerPos.y + a_size.y * 0.5f, 0.0f);
	m_verts[m_nextVert].m_uv = uv.m_bottomRightUV;
	m_verts[m_nextVert].m_color = a_color;
	m_nextVert++;

	assert(m_nextVert <= g_maxSprites * 4);
}

void RiftSprite::Render(dxrnd::Effect &a_effect) {
	HRESULT ret;

	ret = DoRender(Vertex_PTC::g_elements, m_verts, sizeof(Vertex_PTC), a_effect);
	if (ret != D3D_OK) {
		LOG_3("RiftSprite : Failed to render");
		Clear();
	}
}

}
