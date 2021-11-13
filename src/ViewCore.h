// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "ViewAssets.h"
#include <GamesCommon/Input.h>
#include <Math/AABoundingBox.h>
#include "Model.h"
#include "EnemySprite.h"



namespace view {

class ViewCore {
	public:
		ViewCore(void);
		~ViewCore(void);

		class Resolution {
			public:
				Resolution(int a_width, int a_height) : m_width(a_width), m_height(a_height), m_doFullscreen(false), m_doWindowed(false) {};
				bool operator == (const Resolution &a_r) { return m_width == a_r.m_width && m_height == a_r.m_height; };
				int m_width;
				int m_height;
				bool m_doFullscreen;
				bool m_doWindowed;
		};

		cmn::String Initiate(HINSTANCE a_appInstance);
		cmn::String GetAdapterDescription();
		void Terminate();

		void StartFrame();
		void EndFrame();

		void DoLabel(float a_x, float a_y, float a_w, float a_h, const cmn::String& a_text, DWORD a_align);
		void DoHeadline(float a_x, float a_y, float a_w, float a_h, const cmn::String& a_text, DWORD a_align);
		void DoLabel(int a_x, int a_y, int a_w, int a_h, const cmn::String& a_text, DWORD a_align);
		void DoLabel(const cmn::String& a_text, RECT a_rect, DWORD a_align, DWORD a_color = 0xffffffff);
		void DoLabel(ConsoleFont& a_font, const cmn::String& a_text, RECT a_rect, DWORD a_align, DWORD a_color = 0xffffffff);

		void DoLabel(float a_x, float a_y, float a_w, float a_h, const cmn::WString& a_text, DWORD a_align);
		void DoHeadline(float a_x, float a_y, float a_w, float a_h, const cmn::WString& a_text, DWORD a_align);
		void DoLabel(int a_x, int a_y, int a_w, int a_h, const cmn::WString& a_text, DWORD a_align);
		void DoLabel(const cmn::WString& a_text, RECT a_rect, DWORD a_align, DWORD a_color = 0xffffffff);
		void DoLabel(ConsoleFont& a_font, const cmn::WString& a_text, RECT a_rect, DWORD a_align, DWORD a_color = 0xffffffff);

		void AddSprite(scene::Sprite &a_sprite, const math::Vector2f &a_center, float a_width, float a_height, DWORD a_color, float a_angle);

		void DrawSprite(scene::Sprite &a_sprite, const math::Vector2f &a_center, ViewAssets::UV a_spriteUV, DWORD a_color, float a_angle);
		void DrawSprite(scene::Sprite &a_sprite, const math::Vector2f &a_center, ViewAssets::UV a_spriteUV, DWORD a_color, float a_angle, float a_scale);
		void DrawSprite(scene::Sprite &a_sprite, const math::Vector2f &a_center, ViewAssets::UV a_spriteUV, DWORD a_color);
		void DrawSprite(scene::Sprite &a_sprite, const math::AABoundingBox2D &a_box, ViewAssets::UV a_spriteUV, DWORD a_color, float a_angle);

		void DrawBeam(scene::Sprite &a_sprite, const math::Vector2f &a_pos, float a_width, float a_angle, DWORD a_color);

		void DrawBriefingBackground(float a_time);
		void DrawStaticBackground(float a_time);

		void UpdateAndDrawStars(float a_dt, scene::Sprite &a_sprite, float a_bossTimer, const math::Vector2f &a_playerPos);
		void UpdateAndDrawBackground(float a_dt, float a_bossTimer, const math::Vector2f &a_playerPos);

		void SetWorldMatrix(dxrnd::EffectEx &a_effect, const math::Matrix44f &a_matrix);

		void RenderEnemySprites(EnemySprite &a_sprite);

		void DrawEnemy(EnemySprite &a_sprite, const model::Enemy &a_enemy, float a_dt, int a_level, float a_bossScale);
		math::Vector2f GetEnemySpriteSize(model::Enemy::Type a_type);
		math::Vector2f GetSpriteSize(const ViewAssets::UV &a_uv) const;
		ViewAssets::UV GetEnemyUV(model::Enemy::Type a_type);

		static const int g_maxEnemyColors = 10;
		static math::Vector4c g_enemyColors[g_maxEnemyColors];

		DWORD ColorARGB(const unsigned char a_a, const unsigned char a_r, const unsigned char a_g, const unsigned char a_b) {return D3DCOLOR_ARGB(a_a, a_r, a_g, a_b);};
		float GetRed(DWORD a_color);
		float GetGreen(DWORD a_color);
		float GetBlue(DWORD a_color);

		bool IsLeftPressed();
		bool IsRightPressed();

		math::Vector2f GetPlayerMovement(const float a_dt);
		void DoSetAbility(bool a_doSetAbility[model::Player::A_Count]);
		bool DoFire();
		bool DoUseAbility();


		// imgui
		enum WidgetState {
			WS_None,
			WS_Hot,				// mouse over
			WS_SuperHot,		// mouse over and lmb down
			WS_SuperHotTimed,	// mouse over and lmb timed down
			WS_Clicked,			// mouse over and lmb clicked
			WS_DblClicked		// mouse double clicked
		};

		

		WidgetState DoButton(int a_x, int a_y, int a_w, int a_h, cmn::String a_text, bool a_isSelected);
		void DoInactiveButton(int a_x, int a_y, int a_w, int a_h, cmn::WString a_text);
		bool DoSpinnerButton(int a_x, int a_y, int a_w, int a_h, float &a_value, float a_stepSize);
		bool DoSpinnerButton(int a_x, int a_y, int a_w, int a_h, int &a_value);

		void DrawUIRect(int a_x, int a_y, int a_w, int a_h, bool a_isHot, bool a_isSuperHot);
		void GetWidgetStatus(RECT& rect, bool& a_isHot, bool& a_isClicked, bool& a_isSuperHot, bool &a_isSuperHotTimed, bool &a_isDblClicked);
		WidgetState BoolsToWidgetState(bool& a_isHot, bool& a_isClicked, bool& a_isSuperHot, bool &a_isSuperHotTimed, bool &a_isDblClicked);
		void DrawRect(const RECT &a_rect, D3DCOLOR a_color);
		bool IsInside(const RECT &a_rect);

		void DrawTWTPBLogo(float a_time, float a_yPos);
		void RenderSpriteSheet(const math::Vector2f &a_pos, const float a_scale);
		int GetSpriteSheetWidth() const { return m_assets.m_sprites.GetWidth(); }
		int GetSpriteSheetHeight() const { return m_assets.m_sprites.GetHeight(); }

		void SetMouse(bool a_doShow);
		int GetMouseWheelStatus();
		bool HasFocus();
		bool m_isFullscreen;
		int m_width, m_height;	// window resolution	// could be index in resolutions
		int m_fallbackResolutionIx;						// resolution to use if all is wacked...
		math::Vector2f m_scale;	// x y scale			// could be calculated on fly
		float m_aspectRatio; // m_width / m_height		// could be calculated on fly
		math::Matrix44f m_projection;
		math::Matrix44f m_view;
		dxrnd::D3DViewPort m_viewport;

		Input m_input;
		ViewAssets m_assets;
		float m_time;

		std::vector<Resolution> m_resolutions;

		const math::Vector2f m_riftPos;

		bool IsButtonUpDownUp(const bool a_buttonPressed, char &a_state);

		class LineDrawer2D {
			public:
				LineDrawer2D();
				~LineDrawer2D();
				void Draw(const math::Vector2f *a_pointsArray, const int a_pointsCount);
			private:
				ID3DXLine *m_pLineDrawer;
		};

	private:
		
};

}
