// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "BarSprite.h"

#include <DXRenderer/DX.h>

namespace view {

BarSprite::BarSprite(void) {
	m_nextVert = 0;
}

BarSprite::~BarSprite(void) {
}

void BarSprite::AddBar(const math::Vector2f &a_upperLeft, const math::Vector2f &a_bottomRight, DWORD a_color) {
	
	m_verts[m_nextVert].m_pos.Set(a_upperLeft.x, a_upperLeft.y, 0.0f, 1.0f);
	m_verts[m_nextVert].m_color = a_color;
	m_nextVert++;


	m_verts[m_nextVert].m_pos.Set(a_bottomRight.x, a_upperLeft.y, 0.0f, 1.0f);
	m_verts[m_nextVert].m_color = a_color;
	m_nextVert++;

	m_verts[m_nextVert].m_pos.Set(a_upperLeft.x, a_bottomRight.y, 0.0f, 1.0f);
	m_verts[m_nextVert].m_color = a_color;
	m_nextVert++;

	m_verts[m_nextVert].m_pos.Set(a_bottomRight.x, a_bottomRight.y, 0.0f, 1.0f);
	m_verts[m_nextVert].m_color = a_color;
	m_nextVert++;
}

void BarSprite::AddBarLeftRightColors(const math::Vector2f &a_upperLeft, const math::Vector2f &a_bottomRight, DWORD a_leftColor, DWORD a_rightColor) {
	m_verts[m_nextVert].m_pos.Set(a_upperLeft.x, a_upperLeft.y, 0.0f, 1.0f);
	m_verts[m_nextVert].m_color = a_leftColor;
	m_nextVert++;


	m_verts[m_nextVert].m_pos.Set(a_bottomRight.x, a_upperLeft.y, 0.0f, 1.0f);
	m_verts[m_nextVert].m_color = a_rightColor;
	m_nextVert++;

	m_verts[m_nextVert].m_pos.Set(a_upperLeft.x, a_bottomRight.y, 0.0f, 1.0f);
	m_verts[m_nextVert].m_color = a_leftColor;
	m_nextVert++;

	m_verts[m_nextVert].m_pos.Set(a_bottomRight.x, a_bottomRight.y, 0.0f, 1.0f);
	m_verts[m_nextVert].m_color = a_rightColor;
	m_nextVert++;
}

void BarSprite::AddBarTopBottomColors(const math::Vector2f &a_upperLeft, const math::Vector2f &a_bottomRight, DWORD a_topColor, DWORD a_bottomColor) {
	m_verts[m_nextVert].m_pos.Set(a_upperLeft.x, a_upperLeft.y, 0.0f, 1.0f);
	m_verts[m_nextVert].m_color = a_topColor;
	m_nextVert++;


	m_verts[m_nextVert].m_pos.Set(a_bottomRight.x, a_upperLeft.y, 0.0f, 1.0f);
	m_verts[m_nextVert].m_color = a_topColor;
	m_nextVert++;

	m_verts[m_nextVert].m_pos.Set(a_upperLeft.x, a_bottomRight.y, 0.0f, 1.0f);
	m_verts[m_nextVert].m_color = a_bottomColor;
	m_nextVert++;

	m_verts[m_nextVert].m_pos.Set(a_bottomRight.x, a_bottomRight.y, 0.0f, 1.0f);
	m_verts[m_nextVert].m_color = a_bottomColor;
	m_nextVert++;
}

void BarSprite::Render(dxrnd::Effect &a_effect) {
	HRESULT ret;
	dxrnd::D3DVertexElement elements[3];

	elements[0].Stream = 0;
	elements[0].Offset = 0;
	elements[0].Type = D3DDECLTYPE_FLOAT4;
	elements[0].Method = 0;
	elements[0].Usage = D3DDECLUSAGE_POSITION;
	elements[0].UsageIndex = 0;

	elements[1].Stream = 0;
	elements[1].Offset = elements[0].Offset + sizeof(float) * 4;
	elements[1].Type = D3DDECLTYPE_UBYTE4N;
	elements[1].Method = 0;
	elements[1].Usage = D3DDECLUSAGE_COLOR;
	elements[1].UsageIndex = 0;

	// same as D3DDECL_END()
	elements[2].Stream = 0xFF;
	elements[2].Offset = 0;
	elements[2].Type = D3DDECLTYPE_UNUSED;
	elements[2].Method = 0;
	elements[2].Usage = 0;
	elements[2].UsageIndex = 0;

	ret = DoRender(elements, m_verts, sizeof(Vertex), a_effect);
	if (ret != D3D_OK) {
		LOG_3("EnemySprite : Failed to render");
		Clear();
	}
}

}
