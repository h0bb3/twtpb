// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include <DXRenderer/Effect.h>

namespace view {
class HLSLSprite {
	public:
		HLSLSprite(void);
		~HLSLSprite(void);

		void Clear();

	protected:

		typedef unsigned short Index;

		HRESULT DoRender(const dxrnd::D3DVertexElement *a_pElements, const void *a_pVertexData, const int a_vertexSize, dxrnd::Effect &a_effect) const;
		int m_nextVert;
};
}