// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once
#include <dxrenderer\resource.h>

namespace view {
class ConsoleFont : public dxrnd::Resource {
	public:
		ConsoleFont();
		virtual ~ConsoleFont(void);

		
		HRESULT Create(float a_scale);

		// same as ID3DXFont::DrawText
		int DrawText(const cmn::String &a_str, const RECT &a_rect, const DWORD a_format, const D3DCOLOR a_color);
		int ConsoleFont::DrawTextW(const std::wstring &a_str, const RECT &a_rect, const DWORD a_format, const D3DCOLOR a_color);
		

		// Resource realization
		virtual bool Release();
		virtual bool Reload();

	private:
		HRESULT Create();
		float m_scale;
		ID3DXFont *m_pFont;
};
}