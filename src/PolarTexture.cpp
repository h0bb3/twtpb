// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "PolarTexture.h"
#include <DXRenderer/DX.h>

namespace view {

PolarTexture::PolarTexture(void) {
	m_pTexture = NULL;
}

PolarTexture::~PolarTexture(void) {
	Release();
}

HRESULT PolarTexture::Create() {
	HRESULT hr;

	Release();
	hr =  D3DXCreateTexture(	&dxrnd::DX::Device(),
								g_width, g_height,
								1, 0,
								D3DFMT_R8G8B8, D3DPOOL_DEFAULT,
								&m_pTexture);
	if (hr != D3D_OK) {
		return hr;
	}

	unsigned char *data = new unsigned char[g_width * g_height * 3];

	for (int y = 0; y < g_height; y++) {
		for (int x = 0; x < g_width; x++) {
			const int ix((x + y * g_width) * 3);
			const math::Vector2f uv((float)x / (float)g_width * 2.0f - 1.0f, (float)y / (float)g_height * 2.0f - 1.0f);

			data[ix] = (unsigned char)((cos((atan2f(uv.x, uv.y)) + 1.0f) / 2.0f) * 256.0f); // -PI - PI -> 0.0 - 1.0
			//data[ix + 1] = (unsigned char)(uv.Length() / sqrtf(2.0f) * 256.0f);
			data[ix + 1] = 0;
			data[ix + 2] = 0;
		}
	}

	dxrnd::IDirect3DSurface *pSurface;
	hr = m_pTexture->GetSurfaceLevel(0, &pSurface);
	if (hr != D3D_OK) {
		delete [] data;
		return hr;
	}

	RECT r;
	r.top = r.left = 0;
	r.right = g_width;
	r.bottom = g_height;
	hr = D3DXLoadSurfaceFromMemory(	pSurface, NULL, NULL,
									data,
									D3DFMT_R8G8B8,
									g_width * 3,
									NULL,
									&r,
									D3DX_DEFAULT,
									0);
	pSurface->Release();
	delete [] data;

	return hr;
}

bool PolarTexture::Release() {
	if (m_pTexture != NULL) {
		m_pTexture->Release();
		m_pTexture = NULL;
	}
	return true;
}


bool PolarTexture::Reload() {
	return Create() == D3D_OK;
}
}
