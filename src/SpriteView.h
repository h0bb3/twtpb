// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "View.h"


namespace view {

	class SpriteView : public View {
		public:
			SpriteView(const model::Model &a_model, ViewCore &a_core);
			virtual ~SpriteView(void);

			ViewStatus Render(float a_dt, IEventTarget &a_target);

		private:

			void AddUVRect(const scene::Sprite::UVMap &a_uv, math::Vector2f *a_rectCoords, int a_rectIx);
			bool DoDragRect(RECT &a_rect);

			bool IsInside(int a_x, int a_y, const RECT &a_rect);
	};
}
