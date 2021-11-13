// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

namespace view {


	class Vertex_PTC {
		public:
			static const int g_numElements = 4;
			static const dxrnd::D3DVertexElement	[g_numElements];
			math::Vector3f m_pos;
			math::Vector2f m_uv;
			dxrnd::Color m_color;
	};
}
