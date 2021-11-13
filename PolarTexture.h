// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include <dxrenderer\resource.h>

namespace view {

	class PolarTexture : public dxrnd::Resource {
	public:
		PolarTexture(void);
		virtual ~PolarTexture(void);

		HRESULT Create();

		// Resource realization
		virtual bool Release();
		virtual bool Reload();


		static const int g_width = 512;
		static const int g_height = 512;

		dxrnd::IDirect3DTexture *m_pTexture;

};


}
