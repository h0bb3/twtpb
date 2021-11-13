// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "HLSLSprite.h"
#include "VertexTypes.h"

namespace view {
	class RiftSprite : public HLSLSprite {
		public:
			RiftSprite(void);
			~RiftSprite(void);

			static const int g_maxSprites = 1;

			void AddRift(const math::Vector2f &a_centerPos, DWORD a_color, const math::Vector2f &a_size);
			void Render(dxrnd::Effect &a_effect);

		protected:

		private:
			Vertex_PTC *m_verts;	// need to be on heap
	};
}
