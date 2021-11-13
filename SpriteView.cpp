// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include ".\spriteview.h"
#include <DXRenderer/Dx.h>

namespace view {

SpriteView::SpriteView(const model::Model &a_model, ViewCore &a_core) : View(a_model, a_core) {
}

SpriteView::~SpriteView(void) {
}

View::ViewStatus SpriteView::Render(float a_dt, IEventTarget &a_target) {
	float scale = 2.0f;
	float invScale(1.0f / scale);
	bool hasDragged(false);
	

	m_core.SetMouse(true);
	m_core.RenderSpriteSheet(math::Vector2f(10.0f, 10.0f), scale);

	ViewCore::LineDrawer2D lines;
	
	math::Vector2f coords[5];

	for (int sIx = 0; sIx < (int)ViewAssets::UV_Count; sIx++) {
		AddUVRect(m_core.m_assets.m_uvs[sIx], coords, 0);

		for (int i = 0; i < 5; i++) {
			coords[i].x = coords[i].x * scale + 10.0f;
			coords[i].y = coords[i].y * scale + 10.0f;
		}

		RECT rect;
		rect.left = (int)coords[0].x;
		rect.top = (int)coords[0].y;

		rect.right = (int)coords[2].x;
		rect.bottom = (int)coords[2].y;

		if (hasDragged == false && DoDragRect(rect)) {
			m_core.m_assets.m_hasChanged = true;
			hasDragged = true;
			m_core.m_assets.m_uvs[sIx].m_topLeftUV.x = (float)(rect.left - 10) * invScale / m_core.GetSpriteSheetWidth();
			m_core.m_assets.m_uvs[sIx].m_topLeftUV.y = (float)(rect.top - 10)  * invScale / m_core.GetSpriteSheetHeight();
			m_core.m_assets.m_uvs[sIx].m_bottomRightUV.x = (float)(rect.right - 10) * invScale / m_core.GetSpriteSheetWidth();
			m_core.m_assets.m_uvs[sIx].m_bottomRightUV.y = (float)(rect.bottom - 10) * invScale / m_core.GetSpriteSheetHeight();
		}

		lines.Draw(coords, 5);
	}



	// this is to avoid stored clicks between frames in the imgui
	// it is important that this is the last thing that happens
	m_core.m_input.IsClicked(m_core.m_assets.m_mouseFire);

	return VS_Ok;
}

void SpriteView::AddUVRect(const scene::Sprite::UVMap &a_uv, math::Vector2f *a_rectCoords, int a_rectIx) {
	math::Vector2f coords[2];

	coords[0].x = a_uv.m_topLeftUV.x * m_core.GetSpriteSheetWidth();
	coords[0].y = a_uv.m_topLeftUV.y * m_core.GetSpriteSheetHeight();
	coords[1].x = a_uv.m_bottomRightUV.x * m_core.GetSpriteSheetWidth();
	coords[1].y = a_uv.m_bottomRightUV.y * m_core.GetSpriteSheetHeight();

	a_rectCoords[a_rectIx * 5] = coords[0];
	a_rectCoords[a_rectIx * 5 + 1].x = coords[0].x;
	a_rectCoords[a_rectIx * 5 + 1].y = coords[1].y;
	a_rectCoords[a_rectIx * 5 + 2] = coords[1];
	a_rectCoords[a_rectIx * 5 + 3].x = coords[1].x;
	a_rectCoords[a_rectIx * 5 + 3].y = coords[0].y;
	a_rectCoords[a_rectIx * 5 + 4] = coords[0];
}

bool SpriteView::DoDragRect(RECT &a_rect) {

	if (m_core.m_input.IsPressedVKRaw(m_core.m_assets.m_mouseFire)) {
		RECT r(a_rect);

		int dx(0);
		int dy(0);
		if (m_core.m_input.m_mouseDelta.y > 0) {
			dy = (int)(m_core.m_input.m_mouseDelta.y * (float)m_core.m_height + 0.999f);
		} else if (m_core.m_input.m_mouseDelta.y < 0) {
			dy = (int)(m_core.m_input.m_mouseDelta.y * (float)m_core.m_height - 0.999f);
		}
		if (m_core.m_input.m_mouseDelta.x > 0) {
			dx = (int)(m_core.m_input.m_mouseDelta.x * (float)m_core.m_width + 0.999f);
		} else if (m_core.m_input.m_mouseDelta.x < 0) {
			dx = (int)(m_core.m_input.m_mouseDelta.x * (float)m_core.m_width - 0.999f);
		}

		if (dx == 0 && dy == 0) {
			return false;
		}

		const int x((int)(m_core.m_input.m_mousePos.x * (float)m_core.m_width) - dx);
		const int y((int)(m_core.m_input.m_mousePos.y * (float)m_core.m_height) - dy);

		r.left += 2;
		r.right -= 2;
		r.top += 2;
		r.bottom -= 2;

		if (IsInside(x, y, r)) {
			a_rect.right += dx;
			a_rect.left += dx;
			a_rect.top += dy;
			a_rect.bottom += dy;

			return true;
		}

		if (dx != 0) {
			r.left = a_rect.right - 2;
			r.right = a_rect.right;
			if (IsInside(x, y, r)) {
				a_rect.right += dx;
				return true;
			}

			r.left = a_rect.left;
			r.right = a_rect.left + 2;
			if (IsInside(x, y, r)) {
				a_rect.left += dx;
				return true;
			}
		}

		if (dy != 0) {
			r = a_rect;
			r.left += 2;
			r.right -= 2;

			r.top = a_rect.top;
			r.bottom = a_rect.top + 2;
			if (IsInside(x, y, r)) {
				a_rect.top += dy;
				return true;
			}

			r.top = a_rect.bottom - 2;
			r.bottom = a_rect.bottom;
			if (IsInside(x, y, r)) {
				a_rect.bottom += dy;
				return true;
			}
		}
	}

	return false;
}

bool SpriteView::IsInside(int a_x, int a_y, const RECT &a_rect) {
	POINT p;
	p.x = (int)a_x;
	p.y = (int)a_y;

	return PtInRect(&a_rect, p) == TRUE;
}

}