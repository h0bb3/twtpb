// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "Splash.h"

#include <DXRenderer/dx.h>

namespace view {

Splash::Splash(const model::Model &a_model, ViewCore &a_core) : View(a_model, a_core), m_time(7.0f) {
}

Splash::~Splash(void) {
}

float GetNormalizedPeriod(float a_time, float a_lo, float a_hi) {
	if (a_time > a_lo && a_time < a_hi) {
		return (a_time - a_lo) / (a_hi - a_lo);
	}

	return 0.0f;
}

View::ViewStatus Splash::Render(float a_dt, IEventTarget &a_target) {
	scene::Sprite logo;

	math::Vector4f sopColor(1.0f, 1.0f, 1.0f, 1.0f);
	math::Vector4f twtpbColor(0.0f, 0.0f, 0.0f, 0.0f);

	m_time -= a_dt;

	const float extraScale((1.0f - GetNormalizedPeriod(m_time, 0.0f, 7.0f)) * 0.3f);

	/*cmn::String str;
	str.Format("%.2f", m_core.m_assets.m_music.myTracks[MusicBox::T_Menu].GetCurrentPosition());
	m_core.DoLabel(10, 10, 100, 15, str, 0);*/

	if (m_time > 6.0f) {
		const float c(1.0f - (m_time - 6.0f));
		sopColor.Set(c, c , c, 1.0f);
	}


	/*color *= (1.0f - GetNormalizedPeriod(m_time, 6.8f, 7.0f));
	color *= (1.0f - GetNormalizedPeriod(m_time, 6.2f, 6.4f));
	color *= (1.0f - GetNormalizedPeriod(m_time, 5.55f, 5.75f));*/

	if (m_time > 4.0f && m_time < 5.0f) {
		//const float c(m_time);
		//sopColor.Set(c, c , c, 1.0f);

		sopColor *= GetNormalizedPeriod(m_time, 4.0f, 5.0f);
		
		twtpbColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
		twtpbColor *= (1.0f - GetNormalizedPeriod(m_time, 4.0f, 5.0f));
	}
	
	if (m_time < 4.0f) {
		twtpbColor.Set(1.0f, 1.0f, 1.0f, 1.0f);
		sopColor *= 0.0f;
	}

	if (m_time < 1.0f) {
		const float c(m_time);
		twtpbColor.Set(c, c , c, 1.0f);
	}

	sopColor.w = 1.0f;
	sopColor *= 255.0f;
	sopColor.Clamp(0.0f, 255.0f);

	twtpbColor.w = 1.0f;
	twtpbColor *= 255.0f;
	twtpbColor.Clamp(0.0f, 255.0f);

	logo.Create();

	m_core.DrawSprite(logo, math::Vector2f((float)(g_width) * 0.5f, (float)(g_height) * 0.5f), ViewAssets::UV_Logo, math::Vector4c(sopColor), 0, 1.5f + extraScale);

	m_core.DrawSprite(logo, math::Vector2f((float)(g_width) * 0.5f, (float)(g_height) * 0.5f), ViewAssets::UV_TWTPB, math::Vector4c(twtpbColor), 0, 2.0f + extraScale);

	dxrnd::DX::Device().SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	dxrnd::DX::Device().SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	logo.Render(m_core.m_assets.m_sprites);

	m_time -= a_dt;
	if (m_time > 0.0f) {
		return VS_Ok;
	} else {
		return VS_Done;
	}
}

}