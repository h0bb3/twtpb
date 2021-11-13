// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include ".\viewcore.h"

#include "IEventTarget.h"
#include <Time.h>
#include <Math/perlin.h>

#include <DXRenderer/DX.h>

#ifdef _DEBUG
	#define FORCE_WINDOWED false
#else
	#define FORCE_WINDOWED true
#endif

namespace view {

math::Vector4c	ViewCore::g_enemyColors[ViewCore::g_maxEnemyColors] = {	math::Vector4c(10, 150, 200, 255),
																		math::Vector4c(10, 200, 10, 255),
																		math::Vector4c(255, 10, 10, 255),
																		math::Vector4c(120, 50, 200, 255),
																		math::Vector4c(10, 200, 200, 255),
																		math::Vector4c(200, 10, 120, 255),
																		math::Vector4c(30, 150, 75, 255),
																		math::Vector4c(245, 98, 223, 255),
																		math::Vector4c(208, 194, 40, 255),
																		math::Vector4c(255, 255, 255, 255)};

	
ViewCore::ViewCore(void) : m_isFullscreen(true && FORCE_WINDOWED), m_width(1024), m_height(768), m_riftPos(400, 100) {

	m_assets.LoadSettings(&m_width, &m_height, FORCE_WINDOWED ? &m_isFullscreen : NULL);

	bool validResolution(false);
	m_fallbackResolutionIx = -1;

	m_aspectRatio = (float)m_width / (float)m_height;

	DEVMODE mode;
	DEVMODE current;
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &current);
	for (int i = 0; EnumDisplaySettings(NULL, i, &mode) != 0; i++) {
		if (mode.dmBitsPerPel == 32 && mode.dmDisplayFlags == 0) {
			//for (int rIx = 0; rIx < R_Count; rIx++) {
			Resolution r(mode.dmPelsWidth, mode.dmPelsHeight);
			r.m_doFullscreen = true;
			r.m_doWindowed = r.m_width < current.dmPelsWidth && r.m_height < current.dmPelsHeight;
			if (std::find(m_resolutions.begin(), m_resolutions.end(), r) == m_resolutions.end()) {
				m_resolutions.push_back(r);
				LOG_3_A2("%i x %i", mode.dmPelsWidth, mode.dmPelsHeight);

				if (m_width == r.m_width && m_height == r.m_height) {
					validResolution = true;
				}
				// store a suitable fallback resolution needed if we're trying to go for a non existant res
				if (m_fallbackResolutionIx < 0 || (m_resolutions[m_fallbackResolutionIx].m_width < r.m_width && r.m_width <= 1024 &&
					m_resolutions[m_fallbackResolutionIx].m_height < r.m_height && r.m_height <= 768) &&
					r.m_doWindowed) {
					m_fallbackResolutionIx = m_resolutions.size() - 1;
				}
			}
		}
	}

	if (validResolution == false) {
		m_width = m_resolutions[m_fallbackResolutionIx].m_width;
		m_height = m_resolutions[m_fallbackResolutionIx].m_height;
	}
}

ViewCore::~ViewCore(void) {

}

cmn::String ViewCore::Initiate(HINSTANCE a_appInstance) {
	cmn::String error("");
	HRESULT hr;



	hr = dxrnd::DX::GetInstance().Initiate(m_width, m_height, m_isFullscreen, "TWTPB", D3DMULTISAMPLE_NONE);

	if (hr != D3D_OK) {
		LOG_3_A2("Failed to create DX with resolution %i x %i", m_width, m_height);

		// retry with default format and windowed mode
		m_width = m_resolutions[m_fallbackResolutionIx].m_width;
		m_height = m_resolutions[m_fallbackResolutionIx].m_height;
		m_isFullscreen = false;

		hr = dxrnd::DX::GetInstance().Initiate(m_width, m_height, m_isFullscreen, "TWTPB", D3DMULTISAMPLE_NONE);
		if (hr != D3D_OK) {
			error.Format("%s\n%s", dxrnd::DX::GetErrorString(hr), dxrnd::DX::GetErrorDescription(hr));
			return error;
		}
	}

	hr = m_assets.Initiate(m_width, m_height, a_appInstance);
	if (hr != D3D_OK) {
		LOG_3_A2("%s\n%s", dxrnd::DX::GetErrorString(hr), dxrnd::DX::GetErrorDescription(hr));
#ifdef PUBLIC_RELEASE		
		error = "This game requires DirectX 9 compatible graphics card that supports shadermodel 2.0\n\nPlease download the latest drivers from your graphicscard manufacturer.";
#else
		error.Format("%s\n%s", dxrnd::DX::GetErrorString(hr), dxrnd::DX::GetErrorDescription(hr));
#endif
		return error;
	}

	m_scale.Set((float)m_width / (float)g_width, (float)m_height / (float)g_height);
	//m_core.m_view.Scale(math::Vector3f(m_core.m_scale.x, m_core.m_scale.y, 1.0f));
	m_view.Translation(math::Vector3f(0, 0, 0.0f));

	//D3DXMatrixPerspectiveFovLH(&m_core.m_host.m_projection, D3DX_PI/4, 800.0f/600.0f, 0.1f, 32.0f);
	D3DXMatrixPerspectiveFovLH(&m_projection, D3DX_PI/4, (float)m_width / (float)m_height, 0.1f, 32.0f);
	//D3DXMatrixOrthoLH(&m_core.m_projection, m_core.m_width, m_core.m_height, 0.1f, 32.0f);

	dxrnd::DX::Device().SetTransform(D3DTS_PROJECTION, &m_projection);
	dxrnd::DX::Device().SetTransform(D3DTS_VIEW, &m_view);
	dxrnd::DX::Device().GetViewport(&m_viewport);

	dxrnd::DX::Device().SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dxrnd::DX::Device().SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    dxrnd::DX::Device().SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	dxrnd::DX::Device().SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	dxrnd::DX::Device().SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	dxrnd::DX::Device().SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_input.SetWindowsFriendlyMouse(true);

	m_input.SetCursorPos(math::Vector2f(0.5f, 162.0f / (float)g_height));	// menu start button

	return error;
}

void ViewCore::RenderSpriteSheet(const math::Vector2f &a_pos, const float a_scale) {
	scene::Sprite s;
	s.Create();

	s.AddSprite(a_pos, m_assets.m_sprites.GetWidth() * a_scale, m_assets.m_sprites.GetHeight() * a_scale);
	dxrnd::DX::Device().SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	dxrnd::DX::Device().SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	dxrnd::DX::Device().SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	s.Render(m_assets.m_sprites);
}

ViewCore::LineDrawer2D::LineDrawer2D() {
	D3DXCreateLine(&dxrnd::DX::Device(), &m_pLineDrawer);
	m_pLineDrawer->Begin();
}

void ViewCore::LineDrawer2D::Draw(const math::Vector2f *a_pointsArray, const int a_pointsCount) {
	m_pLineDrawer->Draw(a_pointsArray, 5, D3DCOLOR_ARGB(255, 255, 10, 10));
}

ViewCore::LineDrawer2D::~LineDrawer2D() {
	m_pLineDrawer->End();
	m_pLineDrawer->Release();
}

void ViewCore::Terminate() {

	dxrnd::Resource::ReleaseAll();
	m_assets.SaveSettings(m_width, m_height, m_isFullscreen);

	if (m_assets.m_pDInput != NULL) {
		delete m_assets.m_pGameCtrl;
		m_assets.m_pDInput->Release();
		m_assets.m_pDInput = NULL;
	}

	dxrnd::DX::GetInstance().Terminate();
}

cmn::String ViewCore::GetAdapterDescription() {
	cmn::String descr(dxrnd::DX::GetInstance().m_adapterId.Description);
	return descr;
}

void ViewCore::StartFrame() {
	dxrnd::DX::Device().SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	dxrnd::DX::Device().SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    dxrnd::DX::Device().SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	dxrnd::DX::Device().SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	dxrnd::DX::Device().SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	dxrnd::DX::Device().SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	if (dxrnd::DX::Device().Present(NULL, NULL, NULL, NULL) == D3DERR_DEVICELOST) {
		dxrnd::DX::GetInstance().Reset();

		dxrnd::DX::Device().SetTransform(D3DTS_PROJECTION, &m_projection);
		dxrnd::DX::Device().SetTransform(D3DTS_VIEW, &m_view);

		m_assets.m_barEffect.Get()->SetFloat("g_resolutionX", m_width);
		m_assets.m_barEffect.Get()->SetFloat("g_resolutionY", m_height);

		dxrnd::DX::Device().SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		dxrnd::DX::Device().SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		dxrnd::DX::Device().SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		dxrnd::DX::Device().SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		dxrnd::DX::Device().SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		dxrnd::DX::Device().SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
		dxrnd::DX::Device().SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		dxrnd::DX::Device().SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}
	dxrnd::DX::Device().Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 4, 7), 1.0f, 0);
	dxrnd::DX::Device().BeginScene();

	m_assets.CreateGameCtrl();
}

void ViewCore::EndFrame() {
	dxrnd::DX::Device().EndScene();
}

void ViewCore::DoLabel(int a_x, int a_y, int a_w, int a_h, const cmn::WString& a_text, DWORD a_align) {
	RECT rect;

	rect.left = a_x;
	rect.right = a_x + a_w;
	rect.top = a_y;
	rect.bottom = a_y + a_h;

	DoLabel(a_text, rect, a_align);
}

void ViewCore::DoHeadline(float a_x, float a_y, float a_w, float a_h, const cmn::WString& a_text, DWORD a_align) {
	RECT rect;

	rect.left = a_x * m_scale.x;
	rect.right = (a_x + a_w) * m_scale.x;
	rect.top = a_y * m_scale.y;
	rect.bottom = (a_y + a_h) * m_scale.y;

	DoLabel(m_assets.m_headlinesFont, a_text, rect, a_align, 0xffffffff);
}

void ViewCore::DoLabel(float a_x, float a_y, float a_w, float a_h, const cmn::WString& a_text, DWORD a_align) {
	RECT rect;

	rect.left = a_x * m_scale.x;
	rect.right = (a_x + a_w) * m_scale.x;
	rect.top = a_y * m_scale.y;
	rect.bottom = (a_y + a_h) * m_scale.y;

	DoLabel(m_assets.m_font, a_text, rect, a_align, 0xffffffff);
}

void ViewCore::DoLabel(const cmn::WString& a_text, RECT a_rect, DWORD a_align, DWORD a_color) {

	a_rect.left *= m_scale.x;
	a_rect.right *= m_scale.x;
	a_rect.top *= m_scale.y;
	a_rect.bottom *= m_scale.y;

	DoLabel(m_assets.m_font, a_text, a_rect, a_align, a_color);
}

void ViewCore::DoLabel(ConsoleFont& a_font, const cmn::WString& a_text, RECT a_rect, DWORD a_align, DWORD a_color) {
	a_font.DrawTextW(a_text, a_rect, a_align, 0xff000000);
	a_rect.left -= 1;
	a_rect.right -= 1;
	a_rect.top -= 1;
	a_rect.bottom -= 1;
	a_font.DrawTextW(a_text, a_rect, a_align, a_color);
}

void ViewCore::DoLabel(int a_x, int a_y, int a_w, int a_h, const cmn::String& a_text, DWORD a_align) {
	RECT rect;

	rect.left = a_x;
	rect.right = a_x + a_w;
	rect.top = a_y;
	rect.bottom = a_y + a_h;

	DoLabel(a_text, rect, a_align);
}

void ViewCore::DoHeadline(float a_x, float a_y, float a_w, float a_h, const cmn::String& a_text, DWORD a_align) {
	RECT rect;

	rect.left = a_x * m_scale.x;
	rect.right = (a_x + a_w) * m_scale.x;
	rect.top = a_y * m_scale.y;
	rect.bottom = (a_y + a_h) * m_scale.y;

	DoLabel(m_assets.m_headlinesFont, a_text, rect, a_align, 0xffffffff);
}

void ViewCore::DoLabel(float a_x, float a_y, float a_w, float a_h, const cmn::String& a_text, DWORD a_align) {
	RECT rect;

	rect.left = a_x * m_scale.x;
	rect.right = (a_x + a_w) * m_scale.x;
	rect.top = a_y * m_scale.y;
	rect.bottom = (a_y + a_h) * m_scale.y;

	DoLabel(m_assets.m_font, a_text, rect, a_align, 0xffffffff);
}

void ViewCore::DoLabel(const cmn::String& a_text, RECT a_rect, DWORD a_align, DWORD a_color) {

	a_rect.left *= m_scale.x;
	a_rect.right *= m_scale.x;
	a_rect.top *= m_scale.y;
	a_rect.bottom *= m_scale.y;

	DoLabel(m_assets.m_font, a_text, a_rect, a_align, a_color);
}

void ViewCore::DoLabel(ConsoleFont& a_font, const cmn::String& a_text, RECT a_rect, DWORD a_align, DWORD a_color) {
	a_font.DrawText(a_text, a_rect, a_align, 0xff000000);
	a_rect.left -= 1;
	a_rect.right -= 1;
	a_rect.top -= 1;
	a_rect.bottom -= 1;
	a_font.DrawText(a_text, a_rect, a_align, a_color);
}

void ViewCore::SetMouse(bool a_doShow) {
	m_input.SetWindowsFriendlyMouse(a_doShow);
	int count;
	if (a_doShow) {
		while((count = ShowCursor(TRUE)) <= 0);
		while(count > 1) {
			count = ShowCursor(FALSE);
		}
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	} else {
		while((count = ShowCursor(FALSE) > 0));
		while (count < 0) {
			count = ShowCursor(TRUE);
		}
	}	
}

void ViewCore::DrawSprite(scene::Sprite &a_sprite, const math::Vector2f &a_center, ViewAssets::UV a_spriteUV, DWORD a_color, float a_angle) {
	const math::Vector2f size(m_assets.m_uvs[a_spriteUV].m_bottomRightUV - m_assets.m_uvs[a_spriteUV].m_topLeftUV);
	const int width(size.x * m_assets.m_sprites.GetWidth());
	const int height(size.y * m_assets.m_sprites.GetHeight());

	const math::Vector2f pos(a_center.x * m_scale.x, a_center.y * m_scale.y);

	a_sprite.AddSprite(pos, width, height, a_angle, m_scale, m_assets.m_uvs[a_spriteUV], a_color);
}

void ViewCore::DrawSprite(scene::Sprite &a_sprite, const math::Vector2f &a_center, ViewAssets::UV a_spriteUV, DWORD a_color, float a_angle, float a_scale) {
	const math::Vector2f size(m_assets.m_uvs[a_spriteUV].m_bottomRightUV - m_assets.m_uvs[a_spriteUV].m_topLeftUV);
	const int width(size.x * m_assets.m_sprites.GetWidth());
	const int height(size.y * m_assets.m_sprites.GetHeight());

	const math::Vector2f pos(a_center.x * m_scale.x, a_center.y * m_scale.y);

	a_sprite.AddSprite(pos, width, height, a_angle, m_scale * a_scale, m_assets.m_uvs[a_spriteUV], a_color);
}

void ViewCore::DrawSprite(scene::Sprite &a_sprite, const math::Vector2f &a_center, ViewAssets::UV a_spriteUV, DWORD a_color) {
	const math::Vector2f size(m_assets.m_uvs[a_spriteUV].m_bottomRightUV - m_assets.m_uvs[a_spriteUV].m_topLeftUV);
	const float width(size.x * (float)m_assets.m_sprites.GetWidth());
	const float height(size.y * (float)m_assets.m_sprites.GetHeight());
	const math::Vector2f pos((a_center.x - width * 0.5f) * m_scale.x , (a_center.y - height * 0.5f) * m_scale.y);

	a_sprite.AddSprite(pos, (int)width * m_scale.x, (int)height * m_scale.y, m_assets.m_uvs[a_spriteUV], a_color);
}

void ViewCore::AddSprite(scene::Sprite &a_sprite, const math::Vector2f &a_center, float a_width, float a_height, DWORD a_color, float a_angle) {
	const math::Vector2f pos(a_center.x * m_scale.x , a_center.y * m_scale.y);
	scene::Sprite::UVMap uv;
	uv.m_topLeftUV.Set(0.0f, 0.0f);
	uv.m_bottomRightUV.Set(1.0f, 1.0f);
	
	a_sprite.AddSprite(pos, (int)(a_width * m_scale.x), (int)(a_height * m_scale.y), a_angle, uv, a_color);
}

void ViewCore::DrawSprite(scene::Sprite &a_sprite, const math::AABoundingBox2D &a_box, ViewAssets::UV a_spriteUV, DWORD a_color, float a_angle) {
	const math::Vector2f pos((a_box.m_pos.x - a_box.m_size.x) * m_scale.x , (a_box.m_pos.y - a_box.m_size.y) * m_scale.y);

	a_sprite.AddSprite(pos, a_box.m_size.x * 2.0f * m_scale.x, a_box.m_size.y * 2.0f * m_scale.y, a_angle, m_assets.m_uvs[a_spriteUV], a_color);	
}

void ViewCore::DrawBeam(scene::Sprite &a_sprite, const math::Vector2f &a_pos, float a_width, float a_angle, DWORD a_color) {
	const math::Vector2f pos(a_pos.x * m_scale.x, a_pos.y * m_scale.y);
	math::Matrix44f mat44;
	const math::Vector2f scale(a_width, sqrtf(m_width * m_width + m_height * m_height));

	// depending on aspect ratio the angle must be recalculated
	// have no idea why x and y needs to be swapped though...
	const math::Vector2f normal(cosf(a_angle) * m_scale.y, sinf(a_angle) * m_scale.x);
	a_angle = atan2f(normal.y, normal.x);

	const math::Vector2f center(0.0f, -1.0f);
	D3DXMatrixTransformation2D(&mat44, &center, 0.0f, &scale, NULL, a_angle, &pos);
	math::Matrix33f mat(mat44);
	mat.m_20 = mat44._41;
	mat.m_21 = mat44._42;

	a_sprite.AddSprite(mat, m_assets.m_uvs[m_assets.UV_Beam], a_color);
}

void ViewCore::UpdateAndDrawStars(float a_dt, scene::Sprite &a_sprite, float a_bossTimer, const math::Vector2f &a_playerPos) {
//	const math::Vector2f size(m_assets.m_uvs[ViewAssets::UV_Star].m_bottomRightUV - m_assets.m_uvs[ViewAssets::UV_Star].m_topLeftUV);
//	const int width(size.x * m_assets.m_sprites.GetWidth());
//	const int height(size.y * m_assets.m_sprites.GetHeight());
	static int seed(0);

	seed += (int)(a_dt * 10000.f);

	srand(seed);

	int sIx = 0;
/*	for (; sIx < ViewAssets::g_starsLayer1; sIx++) {
		m_assets.m_stars[sIx].y += a_dt * 400.0f;
		if (m_assets.m_stars[sIx].y > g_height + 5.0f) {
			m_assets.m_stars[sIx].y -= g_height + 5.0f;
			m_assets.m_stars[sIx].x = (float)(rand() % g_width);

		}
		DrawSprite(a_sprite, m_assets.m_stars[sIx], ViewAssets::UV_Star, D3DCOLOR_ARGB(255, 30, 30, 30));	
	}

	for (; sIx < ViewAssets::g_starsLayer1 + ViewAssets::g_starsLayer2; sIx++) {
		m_assets.m_stars[sIx].y += a_dt * 200.0f;
		if (m_assets.m_stars[sIx].y > g_height + 5.0f) {
			m_assets.m_stars[sIx].y -= g_height + 5.0f;
			m_assets.m_stars[sIx].x = (float)(rand() % g_width);

		}
		DrawSprite(a_sprite, m_assets.m_stars[sIx], ViewAssets::UV_Star, D3DCOLOR_ARGB(255, 20, 20, 20));	
	}*/

	for (sIx = 0; sIx < ViewAssets::g_starsLayer1 + ViewAssets::g_starsLayer2 + ViewAssets::g_starsLayer3; sIx++) {


		if (a_dt < 0.01f && (m_assets.m_stars[sIx] - a_playerPos).Length2() < 1250.0f * 125.0f) {
			const static math::Vector2f attractor(0.0f, 25.0f);
			const static math::Vector2f deflector(0.0f, -15.0f);

			math::Vector2f force;
			
			force = (m_assets.m_stars[sIx] - (a_playerPos + deflector)) * -1.0f;
			float mag = force.Length();
			force /= mag;
			force *= 1000000 / mag;
			m_assets.m_stars[sIx] -= force * a_dt * a_dt;

			force = m_assets.m_stars[sIx] - (a_playerPos + attractor);
			mag = force.Length2();
			if (mag > 0.0f) {
				mag = force.Length();
				force /= mag;
				force *= 700000 / mag;


				m_assets.m_stars[sIx] -= force * a_dt * a_dt;
			}
		}

				m_assets.m_stars[sIx].y += a_dt * 50.0f * (1.0f - a_bossTimer);

		if (m_assets.m_stars[sIx].y > g_height + 5.0f) {
			m_assets.m_stars[sIx].y -= g_height + 5.0f;
			m_assets.m_stars[sIx].x = (float)(rand() % g_width);

		}
		DrawSprite(a_sprite, m_assets.m_stars[sIx], ViewAssets::UV_EngineParticle, D3DCOLOR_ARGB(255, 50, 35, 35));	
	}
}

void ViewCore::UpdateAndDrawBackground(float a_dt, float a_bossTimer, const math::Vector2f &a_playerPos) {
	scene::Sprite::UVMap uv;
	scene::Sprite sprite;
	sprite.Create();
	const math::Vector2f gravityPos(a_playerPos.x * m_scale.x, a_playerPos.y * m_scale.y); 

	if (m_assets.m_levelOfDetail == 0) {
		return;
	}

	static int seed(0);

	seed += (int)(a_dt * 71717.f);

	srand(seed);

	const float duvBg(512.0f / 1024.0f);
	const float duvDeco(256.0f / 1024.0f);
	int size;
	for (int i = 0; i < ViewAssets::g_tileCount; i++) {
		float duv, uvOffset;
		int tileCount;
		const int ix((16 + i) % ViewAssets::g_tileCount);	// we use this to get some bg tiles to render on top...


		m_assets.m_tiles[ix].m_pos.y += (1.0f - a_bossTimer) * a_dt * (15.0f + (float)(ix % 4) * 50.5f);
		m_assets.m_tiles[ix].m_angle += a_dt * (m_assets.m_tiles[ix].m_rotSpeed + (float)(ix % 4) * 0.009f);


		// we scale with x to keep a good aspect ratio
		if (m_assets.m_tiles[ix].m_pos.y > -300 * m_scale.x) {
			static const DWORD colors[4] = {D3DCOLOR_ARGB(255, 100, 175, 125), D3DCOLOR_ARGB(255, 125, 175, 200), D3DCOLOR_ARGB(255, 200, 200, 200), D3DCOLOR_ARGB(255, 250, 225, 225)};

			if (ix < ViewAssets::g_bgTileCount) {
				tileCount = ViewAssets::g_maxBgTiles;
				duv = duvBg;
				uvOffset = duv;
				size = 512;

			} else if (ix < ViewAssets::g_bgTileCount + ViewAssets::g_decoTileCount) {
				tileCount = ViewAssets::g_maxDecoTiles;
				duv = duvDeco;
				uvOffset = duv;
				size = 256;

			} else {
				tileCount = ViewAssets::g_maxPlanetTiles;
				duv = duvDeco;
				uvOffset = 0;
				size = 64 + 64 * (ix % 4);
			}

			if (m_assets.m_tiles[ix].m_pos.y > (g_height + 300) * m_scale.x) {
				m_assets.m_tiles[ix].Randomize(m_width, m_height * 2, tileCount);
			}

			uv.m_topLeftUV.x = uvOffset + duv * (float)(m_assets.m_tiles[ix].m_tileIx % tileCount);
			uv.m_topLeftUV.y = uvOffset + duv * (float)(m_assets.m_tiles[ix].m_tileIx / tileCount);
			uv.m_bottomRightUV = uv.m_topLeftUV;
			uv.m_bottomRightUV.x += duv;
			uv.m_bottomRightUV.y += duv;
			sprite.AddSprite(m_assets.m_tiles[ix].m_pos, size, size, m_assets.m_tiles[ix].m_angle, math::Vector2f(m_scale.x,m_scale.x), uv, colors[ix % 4]);
		}
	}

	sprite.Render(m_assets.m_bgTiles);
}

void ViewCore::DrawEnemy(EnemySprite &a_sprite, const model::Enemy &a_enemy, float a_dt, int a_level, float a_bossScale) {
	
	ViewAssets::UV uv(GetEnemyUV(a_enemy.m_type));

	if (a_level < 0) {
		a_level = g_maxEnemyColors - 1;
	}

	math::Vector2f size(m_assets.m_uvs[uv].m_bottomRightUV - m_assets.m_uvs[uv].m_topLeftUV);
	size.x *= m_assets.m_sprites.GetWidth();
	size.y *= m_assets.m_sprites.GetHeight();

	math::Vector2f scale(m_scale * a_enemy.m_scale * a_bossScale);

	math::Vector2f pos(m_riftPos + (a_enemy.m_pos - m_riftPos) * a_bossScale * a_bossScale * a_bossScale);

	pos.x *=  m_scale.x;
	pos.y *=  m_scale.y;

	if (a_dt <= 0.1f) {
		a_dt *= 10.0f;
		const unsigned char r((unsigned char)((float)g_enemyColors[a_level % g_maxEnemyColors].m_r * a_dt));
		const unsigned char g((unsigned char)((float)g_enemyColors[a_level % g_maxEnemyColors].m_g * a_dt));
		const unsigned char b((unsigned char)((float)g_enemyColors[a_level % g_maxEnemyColors].m_b * a_dt));
		DWORD color(D3DCOLOR_ARGB(255, r, g, b));

		a_sprite.AddEnemy(pos, size, scale, a_enemy.m_angle, m_assets.m_uvs[uv], color);
		//DrawSprite(a_sprite, a_enemy.m_pos, uv, color, a_enemy.m_angle, a_enemy.m_scale);
	} else {
		if (a_enemy.m_hasShield) {
			a_sprite.AddEnemy(pos, size, scale, a_enemy.m_angle, m_assets.m_uvs[uv], D3DCOLOR_ARGB(255, 200, 170, 25));
		} else {
			DWORD color(D3DCOLOR_ARGB(255, g_enemyColors[a_level % g_maxEnemyColors].m_r, g_enemyColors[a_level % g_maxEnemyColors].m_g, g_enemyColors[a_level % g_maxEnemyColors].m_b));
			a_sprite.AddEnemy(pos, size, scale, a_enemy.m_angle, m_assets.m_uvs[uv], color);
		}
		//DrawSprite(a_sprite, a_enemy.m_pos, uv, D3DCOLOR_ARGB(255, 255, 255, 255), a_enemy.m_angle, a_enemy.m_scale);
	}

	//DrawSprite(a_sprite, a_enemy.m_pos, uv, D3DCOLOR_ARGB(255, 40, 5, 5), a_enemy.m_angle, a_enemy.m_scale * 3.0f);
}

void ViewCore::RenderEnemySprites(EnemySprite &a_sprite) {
		m_assets.m_enemyEffect.Get()->SetTexture(m_assets.m_enemyEffect.m_textures[0].m_handle, m_assets.m_sprites.GetTexture());
		m_assets.m_enemyEffect.Get()->SetFloat("g_resolutionX", m_width);
		m_assets.m_enemyEffect.Get()->SetFloat("g_resolutionY", m_height);
		a_sprite.Render(m_assets.m_enemyEffect);
}

ViewAssets::UV ViewCore::GetEnemyUV(model::Enemy::Type a_type) {
	ViewAssets::UV uv;

	if (a_type == model::Enemy::T_Head) {
		uv = ViewAssets::UV_Enemy;
	} else if (a_type == model::Enemy::T_Ball) {
		uv = ViewAssets::UV_BallOfFury;
	} else if (a_type == model::Enemy::T_Drone) {
		uv = ViewAssets::UV_Drone;
	} else if (a_type == model::Enemy::T_Segment) {
		uv = ViewAssets::UV_Segment;
	} else if (a_type == model::Enemy::T_Shock || a_type == model::Enemy::T_Shock2) {
		uv = ViewAssets::UV_Shock;
	} else if (a_type == model::Enemy::T_Spy) {
		uv = ViewAssets::UV_Spy;
	} else if (a_type == model::Enemy::T_Pill) {
		uv = ViewAssets::UV_Pill;
	} else {
		uv = ViewAssets::UV_BallBoss;
	}

	return uv;
}

math::Vector2f ViewCore::GetEnemySpriteSize(model::Enemy::Type a_type) {
	return GetSpriteSize(GetEnemyUV(a_type));
}

math::Vector2f ViewCore::GetSpriteSize(const ViewAssets::UV &a_uv) const {
	math::Vector2f size(m_assets.m_uvs[a_uv].m_bottomRightUV - m_assets.m_uvs[a_uv].m_topLeftUV);

	const int width(m_assets.m_sprites.GetWidth());
	const int height(m_assets.m_sprites.GetHeight());

	size.x *= width;
	size.y *= height;

	return size;
}

bool ViewCore::IsLeftPressed() {
	static bool padState(false);
	bool padLeft(false);
	if (m_assets.m_pGameCtrl != NULL) {
		if (padState == false) {
			padState = m_assets.m_pGameCtrl->GetAxis(m_assets.m_padXAxis) < -500;
			if (padState == true) {
				padLeft = true;
			}
		} else {
			padState = m_assets.m_pGameCtrl->GetAxis(m_assets.m_padXAxis) < -200;
		}
	}

	return m_input.IsPressedVK(m_assets.m_kbdLeft) || m_input.IsPressedVK(VK_LEFT) || padLeft;
}

bool ViewCore::IsRightPressed() {
	static bool padState(false);
	bool padRight(false);
	if (m_assets.m_pGameCtrl != NULL) {
		if (padState == false) {
			padState = m_assets.m_pGameCtrl->GetAxis(m_assets.m_padXAxis) > 500;
			if (padState == true) {
				padRight = true;
			}
		} else {
			padState = m_assets.m_pGameCtrl->GetAxis(m_assets.m_padXAxis) > 200;
		}
	}

	return m_input.IsPressedVK(m_assets.m_kbdRight) || m_input.IsPressedVK(VK_RIGHT) || padRight;
}



ViewCore::WidgetState ViewCore::DoButton(int a_x, int a_y, int a_w, int a_h, cmn::String a_text, bool a_isSelected) {
	bool hot(false), clicked(false), superHot(false), superHotTimed(false), dblClicked(false);

	RECT rect;

	rect.left = a_x + 1;
	rect.right = a_x + a_w - 1;
	rect.top = a_y + 1;
	rect.bottom = a_y + a_h - 1;

	GetWidgetStatus(rect, hot, clicked, superHot, superHotTimed, dblClicked);

	DrawUIRect(a_x, a_y, a_w, a_h, hot || a_isSelected, superHot);

	if (superHot) {
		rect.left += 1;
		rect.right += 1;
		rect.top += 1;
		rect.bottom += 1;
	}

	DoLabel(a_text, rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	return BoolsToWidgetState(hot, clicked, superHot, superHotTimed, dblClicked);
}







void ViewCore::DoInactiveButton(int a_x, int a_y, int a_w, int a_h, cmn::WString a_text) {
	RECT rect;

	rect.left = a_x * m_scale.x;
	rect.right = (a_x + a_w) * m_scale.x;
	rect.top = a_y * m_scale.y;
	rect.bottom = (a_y + a_h) * m_scale.y;

	DrawRect(rect, 0xff333333);

	rect.left += 1;
	rect.right -= 1;
	rect.top += 1;
	rect.bottom -= 1;

	DrawRect(rect, 0xff000000);

	rect.left = a_x + 1;
	rect.right = a_x + a_w - 1;
	rect.top = a_y + 1;
	rect.bottom = a_y + a_h - 1;

	DoLabel(a_text, rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER, 0xff777777);
}

bool ViewCore::DoSpinnerButton(int a_x, int a_y, int a_w, int a_h, float &a_value, float a_stepSize) {
	cmn::String valueStr;
	bool ret(false);

	valueStr.Format("%f", a_value);

	if (DoButton(a_x, a_y, a_h, a_h, "<", false) == WS_SuperHotTimed) {
		a_value -= a_stepSize;
		ret = true;
	}
	DoLabel(a_x + a_h, a_y, a_w - a_h * 2, a_h, valueStr, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	if (DoButton(a_x + a_w - a_h, a_y, a_h, a_h, ">", false) == WS_SuperHotTimed) {
		a_value += a_stepSize;
		ret = true;
	}

	return ret;
}

bool ViewCore::DoSpinnerButton(int a_x, int a_y, int a_w, int a_h, int &a_value) {
	cmn::String valueStr;
	bool ret(false);

	valueStr.Format("%i", a_value);

	if (DoButton(a_x, a_y, a_h, a_h, "<", false) == WS_SuperHotTimed) {
		a_value--;
		ret = true;
	}
	DoLabel(a_x + a_h, a_y, a_w - a_h * 2, a_h, valueStr, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	if (DoButton(a_x + a_w - a_h, a_y, a_h, a_h, ">", false) == WS_SuperHotTimed) {
		a_value++;
		ret = true;
	}

	return ret;
}


void ViewCore::DrawRect(const RECT &a_rect, D3DCOLOR a_color) {
	IDirect3DSurface9 *pBackBuffer;
	dxrnd::DX::Device().GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
	if (pBackBuffer != NULL) {
		// if rect is out of bounds this seems to crashelicrash
		dxrnd::DX::Device().ColorFill(pBackBuffer, &a_rect, a_color);
		pBackBuffer->Release();
	}
}

bool ViewCore::IsInside(const RECT &a_rect) {
	math::Vector2f mpos;
	m_input.GetCursorPosWin(mpos);
	POINT p;
	p.x = (int)mpos.x * (1.0f / m_scale.x);
	p.y = (int)mpos.y * (1.0f / m_scale.y);

	return PtInRect(&a_rect, p) == TRUE;
}


void ViewCore::GetWidgetStatus(RECT& rect, bool& a_isHot, bool& a_isClicked, bool& a_isSuperHot, bool &a_isSuperHotTimed, bool &a_isDblClicked) {
	if (IsInside(rect)) {
		a_isHot = true;
		if (m_input.IsLMBDoubleClicked()) {
			a_isDblClicked = true;
		} else if (m_input.IsClicked(m_assets.m_mouseFire)) {
			a_isClicked = true;
			a_isSuperHot = true;
		} else if (m_input.IsPressedVK(m_assets.m_mouseFire)) {
			a_isSuperHot = true;
			a_isSuperHotTimed = true;
		} else if (m_input.IsPressedVKRaw(m_assets.m_mouseFire)) {
			a_isSuperHot = true;
		}
	}
}

void ViewCore::DrawUIRect(int a_x, int a_y, int a_w, int a_h, bool a_isHot, bool a_isSuperHot) {
	RECT rect;

	rect.left = a_x * m_scale.x;
	rect.right = (a_x + a_w) * m_scale.x;
	rect.top = a_y * m_scale.y;
	rect.bottom = (a_y + a_h) * m_scale.y;

	if (a_isSuperHot) {
		DrawRect(rect, 0xffffaa22);
	} else {
		DrawRect(rect, 0xffcc9900);
	}
	rect.left += 1;
	rect.right -= 1;
	rect.top += 1;
	rect.bottom -= 1;

	if (a_isHot || a_isSuperHot) {
		DrawRect(rect, 0xff333333);
	} else {
		DrawRect(rect, 0xff171717);
	}
}

void ViewCore::SetWorldMatrix(dxrnd::EffectEx &a_effect, const math::Matrix44f &a_matrix) {
	math::Matrix44f invtransView(m_view);

	invtransView.Inverse();

	math::Vector3f dir(0.2, 0.0, -1.0);
	dir.Normalize();
	a_effect.Get()->SetValue("g_lightDir", &dir, sizeof(math::Vector3f));

	math::Vector3f pos(invtransView._41, invtransView._42, invtransView._43);
	//math::Vector3f pos(-m_view._14, -m_view._24, -m_view._34);
	a_effect.Get()->SetValue("g_eyePos", &pos, sizeof(math::Vector3f));

	if (a_effect.m_worldMatrix.m_handle != NULL) {
		a_effect.Get()->SetMatrix(a_effect.m_worldMatrix.m_handle, &a_matrix);
	}
}

bool ViewCore::IsButtonUpDownUp(const bool a_buttonPressed, char &a_state) {
	if (a_buttonPressed) {
		if (a_state == 1) {
			a_state = 2;
		}
	} else {
		
		if (a_state == 2) {
			a_state = 0;
			return true;
		}

		a_state = 1;	
	}

	return false;
}

void ViewCore::DrawBriefingBackground(float a_time) {
	dxrnd::DX::Device().SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dxrnd::DX::Device().SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	scene::Sprite bg;
	bg.Create();

	bg.AddSprite(math::Vector2f(math::Perlin::PerlinNoise_1D(a_time + 10, 20, 2), math::Perlin::PerlinNoise_1D(a_time, 20, 2)), m_width, m_height);
	bg.AddSprite(math::Vector2f(math::Perlin::PerlinNoise_1D(a_time + 15, 20, 2), math::Perlin::PerlinNoise_1D(a_time + 5, 40, 2)), m_width, m_height);
	bg.Render(m_assets.m_briefingBG);
	bg.Clear();
	bg.AddSprite(math::Vector2f(m_width / 2 - 128.0f * m_scale.x, 16.0f * m_scale.y), 256.0f * m_scale.x, 128.0f * m_scale.y);
	bg.Render(m_assets.m_empireLogo);
	bg.Clear();
}

void ViewCore::DrawStaticBackground(float a_time) {
	dxrnd::DX::Device().SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	dxrnd::DX::Device().SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	scene::Sprite bg;
	bg.Create();

	bg.AddSprite(math::Vector2f(math::Perlin::PerlinNoise_1D(a_time + 10, 20, 2), math::Perlin::PerlinNoise_1D(a_time, 20, 2)), m_width, m_height);
	bg.AddSprite(math::Vector2f(math::Perlin::PerlinNoise_1D(a_time + 15, 20, 2), math::Perlin::PerlinNoise_1D(a_time + 5, 40, 2)), m_width, m_height);
	bg.Render(m_assets.m_briefingBG);
}

int ViewCore::GetMouseWheelStatus() {
	return dxrnd::DXWindow::GetMouseWheelStatus();
}

void ViewCore::DrawTWTPBLogo(float a_time, float a_yPos) {
	scene::Sprite glow;
	glow.Create();

	const char c((char)(cosf(a_time) * 25.0f + 25.0f + 205));
	DrawSprite(glow, math::Vector2f((float)(g_width) * 0.5f, a_yPos), ViewAssets::UV_TWTPB, D3DCOLOR_ARGB(255, c, c, c), 0, 2.0f);

	dxrnd::DX::Device().SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	dxrnd::DX::Device().SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	dxrnd::DX::Device().SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	glow.Render(m_assets.m_sprites);
	glow.Clear();

}

math::Vector2f ViewCore::GetPlayerMovement(const float a_dt) {
	math::Vector2f dMouse(m_input.m_mouseDelta * m_assets.m_mouseSensitivity);

	const float speed(0.4f);
	if (m_input.IsPressedVKRaw(m_assets.m_kbdRight)) {
		dMouse.x += speed * a_dt * m_assets.m_kbdSensitivity;
	}
	if (m_input.IsPressedVKRaw(m_assets.m_kbdLeft)) {
		dMouse.x -= speed * a_dt * m_assets.m_kbdSensitivity;
	}
	if (m_input.IsPressedVKRaw(m_assets.m_kbdDown)) {
		dMouse.y += speed * a_dt * m_assets.m_kbdSensitivity;
	}
	if (m_input.IsPressedVKRaw(m_assets.m_kbdUp)) {
		dMouse.y -= speed * a_dt * m_assets.m_kbdSensitivity;
	}

	if (m_assets.m_pGameCtrl != NULL) {
		HRESULT hr;
		hr = m_assets.m_pGameCtrl->GetState();
		if (hr == DI_OK) {
			math::Vector2f movement(m_assets.m_pGameCtrl->GetAxis(m_assets.m_padXAxis), m_assets.m_pGameCtrl->GetAxis(m_assets.m_padYAxis));

			// circular deadzone
			const float length(movement.Length());
			const float deadzone(200.0f);
			if (length > deadzone) {
				movement /= length;
				movement *= (length - deadzone);
				dMouse += movement * a_dt * 0.0005f * m_assets.m_padSensitivity;
			}
		}
	}

	return dMouse;
}

bool ViewCore::DoFire() {
	return (m_input.IsPressedVKRaw(m_assets.m_mouseFire) ||
		(m_assets.m_pGameCtrl != NULL && m_assets.m_pGameCtrl->GetButton(m_assets.m_padFire) > 0) ||
		m_input.IsPressedVKRaw(m_assets.m_kbdFire));

}

bool ViewCore::DoUseAbility() {
	return (m_input.IsPressedVKRaw(m_assets.m_mouseAbility) || 
		(m_assets.m_pGameCtrl != NULL && m_assets.m_pGameCtrl->GetButton(m_assets.m_padAbility) > 0) ||
		m_input.IsPressedVKRaw(m_assets.m_kbdAbility));
}

void ViewCore::DoSetAbility(bool a_doSetAbility[model::Player::A_Count]) {
	if ((m_assets.m_pGameCtrl != NULL && m_assets.m_pGameCtrl->GetButton(m_assets.m_padSetRapidFire) > 0) ||
		m_input.IsPressedVKRaw(m_assets.m_kbdSetRapidFire)) {
		a_doSetAbility[1] = true;
	}
	if ((m_assets.m_pGameCtrl != NULL && m_assets.m_pGameCtrl->GetButton(m_assets.m_padSetRegeneration) > 0) ||
		m_input.IsPressedVKRaw(m_assets.m_kbdSetRegeneration)) {
		a_doSetAbility[2] = true;
	}
	if ((m_assets.m_pGameCtrl != NULL && m_assets.m_pGameCtrl->GetButton(m_assets.m_padSetTimeWarp) > 0) ||
		m_input.IsPressedVKRaw(m_assets.m_kbdSetTimeWarp)) {
		a_doSetAbility[3] = true;
	}
	if ((m_assets.m_pGameCtrl != NULL && m_assets.m_pGameCtrl->GetButton(m_assets.m_padSetMissiles) > 0) ||
		m_input.IsPressedVKRaw(m_assets.m_kbdSetMissiles)) {
		a_doSetAbility[4] = true;
	}
}


ViewCore::WidgetState ViewCore::BoolsToWidgetState(bool& a_isHot, bool& a_isClicked, bool& a_isSuperHot, bool &a_isSuperHotTimed, bool &a_isDblClicked) {
	if (a_isDblClicked) {
		return ViewCore::WS_DblClicked;
	}
	if (a_isClicked) {
		return ViewCore::WS_Clicked;
	}
	if (a_isSuperHotTimed) {
		return ViewCore::WS_SuperHotTimed;
	}
	if (a_isSuperHot) {
		return ViewCore::WS_SuperHot;
	}
	if (a_isHot) {
		return ViewCore::WS_Hot;
	}

	return ViewCore::WS_None;
}

bool ViewCore::HasFocus() {
	return dxrnd::DX::GetInstance().HasFocus();
}



}
