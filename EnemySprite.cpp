// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "EnemySprite.h"

#include <DXRenderer/DX.h>

namespace view {
EnemySprite::EnemySprite(void) {
	m_nextVert = 0;
	m_verts = new Vertex[g_maxSprites * 4];
}

EnemySprite::~EnemySprite(void) {
	delete [] m_verts;
}

void EnemySprite::AddEnemy(const math::Vector2f &a_pos, const math::Vector2f &a_size, const math::Vector2f &a_scale, float a_rotation, const scene::Sprite::UVMap &a_uv, DWORD a_color, float a_z) {
	m_verts[m_nextVert].m_pos.Set(a_size.x * -0.5f, a_size.y *-0.5f, a_z, 1.0f);
	m_verts[m_nextVert].m_translate = a_pos;
	m_verts[m_nextVert].m_scaleRotate.Set(a_scale.x, a_scale.y, a_rotation);
	m_verts[m_nextVert].m_uv = a_uv.m_topLeftUV;
	m_verts[m_nextVert].m_color = a_color;
	m_nextVert++;

	m_verts[m_nextVert].m_pos.Set(a_size.x * 0.5f, a_size.y * -0.5f, a_z, 1.0f);
	m_verts[m_nextVert].m_translate = a_pos;
	m_verts[m_nextVert].m_scaleRotate.Set(a_scale.x, a_scale.y, a_rotation);
	m_verts[m_nextVert].m_uv.Set(a_uv.m_bottomRightUV.x, a_uv.m_topLeftUV.y);
	m_verts[m_nextVert].m_color = a_color;
	m_nextVert++;

	m_verts[m_nextVert].m_pos.Set(a_size.x * -0.5f, a_size.y * 0.5f, a_z, 1.0f);
	m_verts[m_nextVert].m_translate = a_pos;
	m_verts[m_nextVert].m_scaleRotate.Set(a_scale.x, a_scale.y, a_rotation);
	m_verts[m_nextVert].m_uv.Set(a_uv.m_topLeftUV.x, a_uv.m_bottomRightUV.y);
	m_verts[m_nextVert].m_color = a_color;
	m_nextVert++;

	m_verts[m_nextVert].m_pos.Set(a_size.x * 0.5f, a_size.y * 0.5f, a_z, 1.0f);
	m_verts[m_nextVert].m_translate = a_pos;
	m_verts[m_nextVert].m_scaleRotate.Set(a_scale.x, a_scale.y, a_rotation);
	m_verts[m_nextVert].m_uv = a_uv.m_bottomRightUV;
	m_verts[m_nextVert].m_color = a_color;
	m_nextVert++;

	assert(m_nextVert <= g_maxSprites * 4);
}

void EnemySprite::AddEnemy(const math::Vector2f &a_pos, const math::Vector2f &a_size, const math::Vector2f &a_scale, float a_rotation, const scene::Sprite::UVMap &a_uv, DWORD a_color) {
	AddEnemy(a_pos, a_size, a_scale, a_rotation, a_uv, a_color, 0.0f);
}

void EnemySprite::Render(dxrnd::Effect &a_effect) {
	HRESULT ret;
	dxrnd::D3DVertexElement elements[6];

	elements[0].Stream = 0;
	elements[0].Offset = 0;
	elements[0].Type = D3DDECLTYPE_FLOAT4;
	elements[0].Method = 0;
	elements[0].Usage = D3DDECLUSAGE_POSITION;
	elements[0].UsageIndex = 0;

	// trans
	elements[1].Stream = 0;
	elements[1].Offset = elements[0].Offset + sizeof(float) * 4;
	elements[1].Type = D3DDECLTYPE_FLOAT2;
	elements[1].Method = 0;
	elements[1].Usage = D3DDECLUSAGE_TEXCOORD;
	elements[1].UsageIndex = 0;

	// scale rotate
	elements[2].Stream = 0;
	elements[2].Offset = elements[1].Offset + sizeof(float) * 2;
	elements[2].Type = D3DDECLTYPE_FLOAT3;
	elements[2].Method = 0;
	elements[2].Usage = D3DDECLUSAGE_TEXCOORD;
	elements[2].UsageIndex = 1;

	// uv
	elements[3].Stream = 0;
	elements[3].Offset = elements[2].Offset + sizeof(float) * 3;
	elements[3].Type = D3DDECLTYPE_FLOAT2;
	elements[3].Method = 0;
	elements[3].Usage = D3DDECLUSAGE_TEXCOORD;
	elements[3].UsageIndex = 2;

	elements[4].Stream = 0;
	elements[4].Offset = elements[3].Offset + sizeof(float) * 2;
	elements[4].Type = D3DDECLTYPE_UBYTE4N;
	elements[4].Method = 0;
	elements[4].Usage = D3DDECLUSAGE_COLOR;
	elements[4].UsageIndex = 0;

	// same as D3DDECL_END()
	elements[5].Stream = 0xFF;
	elements[5].Offset = 0;
	elements[5].Type = D3DDECLTYPE_UNUSED;
	elements[5].Method = 0;
	elements[5].Usage = 0;
	elements[5].UsageIndex = 0;

	ret = DoRender(elements, m_verts, sizeof(Vertex), a_effect);
	if (ret != D3D_OK) {
		LOG_3("EnemySprite : Failed to render");
		Clear();
	}
}

}
