// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "VertexTypes.h"

namespace view {

const dxrnd::D3DVertexElement Vertex_PTC::g_elements[Vertex_PTC::g_numElements] = {
/*pos*/	{0, 0, D3DDECLTYPE_FLOAT3, 0, D3DDECLUSAGE_POSITION, 0},
/*uv*/	{0, g_elements[0].Offset + sizeof(float) * 3, D3DDECLTYPE_FLOAT2, 0, D3DDECLUSAGE_TEXCOORD, 0},
/*col*/	{0, g_elements[1].Offset + sizeof(float) * 2, D3DDECLTYPE_UBYTE4N, 0, D3DDECLUSAGE_COLOR, 0},
		D3DDECL_END()
};

}
