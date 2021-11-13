// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include <Scene/Sprite.h>
#include <DXRenderer/Effect.h>
#include "HLSLSprite.h"

namespace view {
	class BarSprite : public HLSLSprite {
	public:
		BarSprite(void);
		~BarSprite(void);

		static const int g_maxSprites = 16;

		void AddBar(const math::Vector2f &a_upperLeft, const math::Vector2f &a_bottomRight, DWORD a_color);
		void AddBarLeftRightColors(const math::Vector2f &a_upperLeft, const math::Vector2f &a_bottomRight, DWORD a_leftColor, DWORD a_rightColor);
		void AddBarTopBottomColors(const math::Vector2f &a_upperLeft, const math::Vector2f &a_bottomRight, DWORD a_topColor, DWORD a_bottomColor);
		void Render(dxrnd::Effect &a_effect);

	protected:
		class Vertex {
			public:
				math::Vector4f m_pos;
				DWORD m_color;
		};

	private:
		Vertex m_verts[g_maxSprites * 4];
};
}
