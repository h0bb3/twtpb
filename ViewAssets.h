// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include <DXRenderer/Texture.h>
#include <DXRenderer/EffectEx.h>
#include <DXRenderer/Model.h>
#include <DXRenderer/DXSoundEngine.h>
#include <Scene/Sprite.h>

#define DIRECTINPUT_VERSION 0x0800

#include <DInput.h>

#include "IEventTarget.h"
#include "ConsoleFont.h"
#include "SoundBox.h"
#include "MusicBox.h"
#include "PlayerViewData.h"
#include "InputDevice.h"
#include "Player.h"

namespace dxrnd
{
	class StreamFile;
}

namespace view {


class ViewAssets {
	public:
		ViewAssets();
		~ViewAssets();

		enum UV {
			UV_Player = 0,
			UV_Enemy,
			UV_Bullet,
			UV_PlayerBullet,
			UV_Logo,
			UV_PlayerShield,
			UV_BallOfFury,
			UV_Star,
			UV_Beam,
			UV_BallBoss,
			UV_TWTPB,
			UV_Drone,
			UV_Segment,
			UV_BonusScore,
			UV_BonusWeapon,
			UV_BonusShield,
			UV_Shock,
			UV_Debris1,
			UV_Debris2,
			UV_Spy,
			UV_Muzzle,
			UV_EngineParticle,
			UV_Pill,
			UV_Count
		};

		enum HudUV {
			HUV_0 = 0,
			HUV_1,
			HUV_2,
			HUV_3,
			HUV_4,
			HUV_5,
			HUV_6,
			HUV_7,
			HUV_8,
			HUV_9,
			HUV_LifeFull,
			HUV_x,
			HUV_RapidFire,
			HUV_Regenerate,
			HUV_TimeWarp,
			HUV_Missiles,
			HUV_Count
		};

		struct PlayerFiles {
			PlayerFiles(const char *a_shipFile, const char *a_diffuseFile, const char *a_specularFile, const char *a_settingsFile) :
				m_shipFile(a_shipFile), m_diffuseFile(a_diffuseFile), m_specularFile(a_specularFile), m_settingsFile(a_settingsFile) {};
			PlayerFiles();	// no implementation to give link error when to few initializers

			cmn::ConstString m_shipFile;
			cmn::ConstString m_diffuseFile;
			cmn::ConstString m_specularFile;
			cmn::ConstString m_settingsFile;
		};

		class PlayerViewData : public cmn::PComplex {
			public:
				PlayerViewData(void);
				~PlayerViewData(void);

				math::Vector3f m_lEnginePos;
				math::Vector3f m_rEnginePos;
				math::Vector2f m_engineParticleStartSize;
				math::Vector2f m_engineParticleEndSize;
				math::Vector3f m_engineParticleSpeed;

				math::Vector3f m_gunMounts[model::Player::g_maxGuns];
		};

		//static const char* g_spriteNames[UV_Count];	//was only used locally in .cpp
		HRESULT Initiate(int a_width, int a_height, HINSTANCE a_appInstance);

		ConsoleFont m_font;
		ConsoleFont m_headlinesFont;
		dxrnd::Texture m_sprites;
		dxrnd::Texture m_shockWave;
		dxrnd::Texture m_shockWave2;
		dxrnd::Texture m_explosion;
		dxrnd::Texture m_cloud1;
		dxrnd::Texture m_cloud2;
		dxrnd::Texture m_riftRamp;
		dxrnd::Texture m_warning;
		dxrnd::Texture m_hud;
		scene::Sprite::UVMap m_uvs[UV_Count];
		scene::Sprite::UVMap m_hudUVs[HUV_Count];
		bool m_hasChanged;

		static const int g_starsLayer1 = 100;
		static const int g_starsLayer2 = 200;
		static const int g_starsLayer3 = 400;
		static const int g_starCount = g_starsLayer1 + g_starsLayer2 + g_starsLayer3;
		math::Vector2f m_stars[g_starCount];

		static const int g_maxBgTiles = 2;
		static const int g_maxPlanetTiles = 4;
		static const int g_maxDecoTiles = 4;
		
		dxrnd::Texture m_bgTiles;

		class BGTile {
			public:
				void Init(int a_maxWidth, int a_maxHeight, int a_maxTiles);
				void Randomize(int a_maxWidth, int a_maxHeight, int a_maxTiles);
				math::Vector2f m_pos;
				float m_angle;
				float m_rotSpeed;
				int m_tileIx;
		};
		static const int g_bgTileCount = 32;	// used in backgrounds rendering
		static const int g_decoTileCount = 32;
		static const int g_planetTileCount = 4;
		static const int g_tileCount = g_bgTileCount + g_decoTileCount + g_planetTileCount;

		BGTile m_tiles[g_tileCount];

		dxrnd::Texture m_abilityIcons[model::Player::A_Count - 1];
		dxrnd::Texture m_briefingBG;
		dxrnd::Texture m_empireLogo;

		

		dxrnd::Model& GetPlayerShip(int a_shipType) { return m_playerShips[a_shipType]; }
		dxrnd::Texture& GetPlayerTexture(int a_shipType) { return m_playerTextures[a_shipType]; }
		dxrnd::Texture& GetPlayerSpecmap(int a_shipType) { return m_playerSpecmaps[a_shipType]; }

		
		dxrnd::EffectEx m_playerEffect;
		dxrnd::EffectEx m_particleEffect;
		dxrnd::EffectEx m_enemyEffect;
		dxrnd::Effect m_barEffect;
		dxrnd::EffectEx m_riftEffect;

#ifdef FOUR_PLAYER_VERSION
		enum PlayerShip {
			PLAYER_0 = 0,
			PLAYER_1,
			PLAYER_2,
			PLAYER_3,
			PLAYER_MAX
		};
#else
		enum PlayerShip {
			PLAYER_0,
			PLAYER_MAX
		};
#endif


		PlayerViewData m_playerData[PLAYER_MAX];

		bool LoadSettings(int *a_pWidth, int *a_pHeight, bool *a_pIsFullscreen);
		bool SaveSettings(int a_width, int a_height, bool a_isFullscreen);

		bool LoadInputSettings();
		bool SaveInputSettings();

		void CreateGameCtrl();
		cmn::String GetPadButtonName(const cmn::String &a_prefix, int a_button);
		void ResetPadSettingsToDefault();
		void ResetKbdSettingsToDefault();
		void ResetMouseSettingsToDefault();

		void SaveMixer();

		dxrnd::DXSoundEngine m_soundEngine;
		SoundBox m_sfx;
		MusicBox m_music;

		LPDIRECTINPUT8 m_pDInput;	// should be moved to dxrnd
		dxrnd::InputDevice *m_pGameCtrl;


		// pad input mappings
		int m_padXAxis;
		int m_padYAxis;
		int m_padFire;
		int m_padAbility;
		int m_padSetRapidFire;
		int m_padSetRegeneration;
		int m_padSetTimeWarp;
		int m_padSetMissiles;
		float m_padSensitivity;

		// keyboard input mappings
		int m_kbdLeft, m_kbdRight, m_kbdUp, m_kbdDown;
		int m_kbdNext, m_kbdPrev;
		int m_kbdFire, m_kbdAbility;
		int m_kbdSetRapidFire, m_kbdSetRegeneration;
		int m_kbdSetTimeWarp, m_kbdSetMissiles;
		float m_kbdSensitivity;

		// mouse input mappings
		int m_mouseFire;
		int m_mouseAbility;
		float m_mouseSensitivity;

		// graphics level of detail
		int m_levelOfDetail;



	private:	
		void RegisterPadVariables(cmn::PComplex &a_container);
		void RegisterInputVariables(cmn::PComplex &a_container);
		void LoadMixer();



		//static const char* g_playerShipFiles[PLAYER_MAX];	//was only used locally in .cpp

		dxrnd::Model m_playerShips[PLAYER_MAX];
		dxrnd::Texture m_playerTextures[PLAYER_MAX];
		dxrnd::Texture m_playerSpecmaps[PLAYER_MAX];
};

}