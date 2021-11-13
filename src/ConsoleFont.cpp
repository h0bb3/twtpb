// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include ".\consolefont.h"

#include <DXRenderer/DX.h>

namespace view {
ConsoleFont::ConsoleFont() : m_scale(1.0f) {
	m_pFont = NULL;
}

ConsoleFont::~ConsoleFont(void) {
	Release();
}

bool ConsoleFont::Release() {
	if (m_pFont != NULL) {
		m_pFont->Release();
		m_pFont = NULL;
	}

	return true;
}

HRESULT ConsoleFont::Create(float a_scale) {
	m_scale = a_scale;
	return Create();
}

HRESULT ConsoleFont::Create() {

	return D3DXCreateFont(&dxrnd::DX::Device(), (INT)(m_scale * 14), 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
							OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
							"Verdana", &m_pFont);
}

bool ConsoleFont::Reload() {

	return Create() == D3D_OK;
}

int ConsoleFont::DrawText(const cmn::String &a_str, const RECT &a_rect, const DWORD a_format, const D3DCOLOR a_color) {
	return m_pFont->DrawText(NULL, a_str.c_str(), -1, (LPRECT)&a_rect, a_format, a_color);
}

int ConsoleFont::DrawTextW(const std::wstring &a_str, const RECT &a_rect, const DWORD a_format, const D3DCOLOR a_color) {
	return m_pFont->DrawTextW(NULL, a_str.c_str(), -1, (LPRECT)&a_rect, a_format, a_color);
}

}
