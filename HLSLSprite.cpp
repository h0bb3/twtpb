// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "HLSLSprite.h"

#include <DXRenderer/DX.h>


namespace view {

HLSLSprite::HLSLSprite(void) {
	m_nextVert = 0;
}

HLSLSprite::~HLSLSprite(void) {
}

HRESULT HLSLSprite::DoRender(const dxrnd::D3DVertexElement *a_pElements, const void *a_pVertexData, const int a_vertexSize, dxrnd::Effect &a_effect) const {
	dxrnd::IDirect3DVertexBuffer *pVB;
	dxrnd::IDirect3DIndexBuffer  *pIB;
	dxrnd::IDirect3DVertexDeclaration *pDecl;
	HRESULT ret;

	if (m_nextVert == 0) {
		return D3D_OK;
	}

	ret = dxrnd::DX::Device().CreateVertexDeclaration(a_pElements, &pDecl);
	if (ret != D3D_OK) {
		LOG_3("HLSLSprite : Failed to create Vertex Declaration");
		return ret;
	}
	ret = dxrnd::DX::Device().SetVertexDeclaration(pDecl);
	if (ret != D3D_OK) {
		LOG_3("HLSLSprite : Failed to set Vertex Declaration");
		return ret;
	}

	ret = dxrnd::DX::Device().CreateVertexBuffer(a_vertexSize * m_nextVert, 0, 0, D3DPOOL_SYSTEMMEM, &pVB, NULL);
	if (ret != D3D_OK) {
		LOG_3("HLSLSprite : Failed to create Vertex Buffer");
		pDecl->Release();
		return ret;
	}

	const size_t indexCount(m_nextVert / 4 * 6);	// each quad has 6 indices

	ret = dxrnd::DX::Device().CreateIndexBuffer(sizeof(Index) * indexCount, 0, D3DFMT_INDEX16, D3DPOOL_SYSTEMMEM, &pIB, NULL);
	if (ret != D3D_OK) {
		LOG_3("HLSLSprite : Failed to create Index Buffer");
		pDecl->Release();
		pVB->Release();
		return ret;
	}


	void *pVerts;
	Index *pIndices;

	ret = pVB->Lock(0, m_nextVert * a_vertexSize, &pVerts, 0);
	if (ret != D3D_OK) {
		LOG_3("HLSLSprite : Failed to lock Vertex Buffer");
		pDecl->Release();
		pVB->Release();
		pIB->Release();
		return ret;
	}
	memcpy(pVerts, a_pVertexData, m_nextVert * a_vertexSize);
	pVB->Unlock();

	if (pIB->Lock(0, indexCount * sizeof(Index), (void**)&pIndices, 0) != D3D_OK) {
		LOG_3("HLSLSprite : Failed to lock Index Buffer");
		pDecl->Release();
		pVB->Release();
		pIB->Release();
		return ret;
	}
	for (size_t qIx = 0, iIx = 0; qIx < m_nextVert / 4; qIx++, iIx += 6) {
			pIndices[iIx + 0] = (Index)(qIx * 4 + 0);
			pIndices[iIx + 1] = (Index)(qIx * 4 + 1);
			pIndices[iIx + 2] = (Index)(qIx * 4 + 2);
			pIndices[iIx + 3] = (Index)(qIx * 4 + 1);
			pIndices[iIx + 4] = (Index)(qIx * 4 + 3);
			pIndices[iIx + 5] = (Index)(qIx * 4 + 2);
	}
	pIB->Unlock();

	ret = dxrnd::DX::Device().SetStreamSource(0, pVB, 0, a_vertexSize);
	if (ret != D3D_OK) {
		LOG_3("HLSLSprite : Could not set stream source.");
		pDecl->Release();
		pVB->Release();
		pIB->Release();
		return ret;
	}
	ret = dxrnd::DX::Device().SetIndices(pIB);
	if (ret != D3D_OK) {
		LOG_3("HLSLSprite : Could not set indices.");
		pDecl->Release();
		pVB->Release();
		pIB->Release();
		return ret;
	}

	unsigned int passCount(0);
	if (a_effect.Get()->Begin(&passCount, 0)  == D3D_OK) {	

		for (unsigned int pIx = 0; pIx < passCount; pIx++) {
			if (a_effect.Get()->BeginPass(pIx) == D3D_OK) {

				dxrnd::DX::Device().DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_nextVert, 0,  indexCount / 3);
				
				a_effect.Get()->EndPass();
			}
		}
		a_effect.Get()->End();
	}

	pVB->Release();
	pIB->Release();
	pDecl->Release();

	return D3D_OK;
}

void HLSLSprite::Clear() {
	m_nextVert = 0;
}

}