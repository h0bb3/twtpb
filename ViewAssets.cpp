// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include ".\viewassets.h"
#include "IEventTarget.h"

#include <Common/PComplex.h>
#include <DXRenderer/DX.h>
#include "InputDeviceFactory.h"


namespace view {

static const char* g_spriteNames[ViewAssets::UV_Count] = {"Player", "Enemy", "Bullet", "PlayerBullet", "Logo", "PlayerShield", "BallOfFury", "Star", "Beam", "BallBoss", "TWTPB", "Drone", "Segment", "BonusScore", "BonusWeapon", "BonusShield", "Shock", "Debris1", "Debris2", "Spy", "Muzzle", "EngineParticle", "Pill"};

//carmack says parallell arrays are evil... :P
// no more parallell arrays then ;)


#ifdef FOUR_PLAYER_VERSION

static const ViewAssets::PlayerFiles g_playerFiles[ViewAssets::PLAYER_MAX] = {
	ViewAssets::PlayerFiles("sumo.obj", "sumo_diffuse.tga", "sumo_specular.tga", "sumo.xml"),
	ViewAssets::PlayerFiles("thesvurd.obj", "thesvurd_diffuse.tga", "thesvurd_specular.tga", "thesvurd.xml"),
	ViewAssets::PlayerFiles("transmorpher.obj", "transmorpher_diffuse.tga", "transmorpher_specular.tga", "transmorpher.xml"),
	ViewAssets::PlayerFiles("samurai.obj", "samurai_diffuse.tga", "samurai_specular.tga", "samurai.xml"),
};
#else
static const ViewAssets::PlayerFiles g_playerFiles[ViewAssets::PLAYER_MAX] = {
	PlayerFiles("sumo.obj", "sumo_diffuse.tga", "sumo_specular.tga")
};
#endif

ViewAssets::ViewAssets() : m_font() {


	m_hasChanged = false;
	m_pDInput = NULL;
	m_pGameCtrl = NULL;
	m_levelOfDetail = 2;

	ResetPadSettingsToDefault();
	ResetKbdSettingsToDefault();
	ResetMouseSettingsToDefault();

	cmn::PComplex input("input");
	RegisterInputVariables(input);
	input.Load("userdata/input.xml");


	for (int i = 0; i< 23; i++) {

		int x = 256.0f * m_uvs[i].m_topLeftUV.x;
		int y = 256.0f * m_uvs[i].m_topLeftUV.y;
		int w = 256.0f * (m_uvs[i].m_bottomRightUV.x - m_uvs[i].m_topLeftUV.x);
		int h = 256.0f * (m_uvs[i].m_bottomRightUV.y - m_uvs[i].m_topLeftUV.y);

		/*LOG_3_A3("case %d : { ret.X = %d; ret.Y = %d;", i, x, y);
		LOG_3_A2("ret.Width = %d; ret.Height = %d; break; }", w, h);*/
	}
	//LOG_3_A2("}", w, h);

}

ViewAssets::~ViewAssets(void) {

	if (m_hasChanged == true) {
		cmn::PComplex spriteUVs("spriteUVs");
		for (int i = 0; i < UV_Count; i++) {
			spriteUVs.Register(g_spriteNames[i], &(m_uvs[i]));
		}
		spriteUVs.Save("data/sprites.xml");
	}

	//m_playerData.Save("data/player.xml");
	SaveInputSettings();
	if (m_pDInput != NULL) {
		SaveInputSettings();
		delete m_pGameCtrl;
		m_pGameCtrl = NULL;
		m_pDInput->Release();
		m_pDInput = NULL;
	}

	cmn::PComplex input("input");
	RegisterInputVariables(input);
	input.Save("userdata/input.xml");


}

bool ViewAssets::LoadInputSettings() {
	if (m_pGameCtrl == NULL) {
		return true;
	} else {
		cmn::String name(m_pGameCtrl->m_deviceInfo.tszInstanceName);
		name.Replace(" ", "_");
		if (name.size() <= 0) {
			name = "generic_usb_controller";
		}

		cmn::PComplex padInput(name);
		RegisterPadVariables(padInput);
		name = cmn::String("userdata/") + name + ".xml";
		if (padInput.Load(name) != true) {
			ResetPadSettingsToDefault();
		}
	}
	return true;
}

bool ViewAssets::SaveInputSettings() {
	if (m_pGameCtrl == NULL) {
		return true;
	} else {
		cmn::String name(m_pGameCtrl->m_deviceInfo.tszInstanceName);
		name.Replace(" ", "_");
		if (name.size() <= 0) {
			name = "generic_usb_controller";
		}

		cmn::PComplex padInput(name);
		RegisterPadVariables(padInput);
		name = cmn::String("userdata/") + name + ".xml";
		return padInput.Save(name);
	}
}

void ViewAssets::ResetMouseSettingsToDefault() {
	m_mouseSensitivity = 1.0f;
	m_mouseFire = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
	m_mouseAbility = m_mouseFire == VK_LBUTTON ? VK_RBUTTON : VK_LBUTTON;
}

void ViewAssets::ResetPadSettingsToDefault() {
	if (m_pGameCtrl == NULL) {
		m_padXAxis = -1;
		m_padYAxis = -1;
		m_padFire = 4;
		m_padAbility = 5;
		m_padSetRapidFire = 0;
		m_padSetRegeneration = 1;
		m_padSetTimeWarp = 2;
		m_padSetMissiles = 3;
	} else {
		m_padXAxis = m_pGameCtrl->m_xAxisIx;
		m_padYAxis = m_pGameCtrl->m_yAxisIx;
		m_padFire = 4;
		m_padAbility = 5;
		m_padSetRapidFire = 0;
		m_padSetRegeneration = 1;
		m_padSetTimeWarp = 2;
		m_padSetMissiles = 3;
	}
	m_padSensitivity = 1.0f;
}

void ViewAssets::ResetKbdSettingsToDefault() {
	m_kbdLeft = VK_LEFT;
	m_kbdRight = VK_RIGHT;
	m_kbdUp = VK_UP;
	m_kbdDown = VK_DOWN;
	m_kbdNext = 'S';
	m_kbdPrev = 'A';
	m_kbdFire = VK_SPACE;
	m_kbdAbility = VK_LCONTROL;
	m_kbdSetRapidFire = 'Z';
	m_kbdSetRegeneration = 'X';
	m_kbdSetTimeWarp = 'C';
	m_kbdSetMissiles = 'V';
	m_kbdSensitivity = 1.0f;
}

void ViewAssets::RegisterInputVariables(cmn::PComplex &a_container) {
	a_container.Register("kbdFire", m_kbdFire, m_kbdFire);
	a_container.Register("kbdUp", m_kbdUp, m_kbdUp);
	a_container.Register("kbdDown", m_kbdDown, m_kbdDown);
	a_container.Register("kbdLeft", m_kbdLeft, m_kbdLeft);
	a_container.Register("kbdRight", m_kbdRight, m_kbdRight);
	a_container.Register("kbdPrev", m_kbdPrev, m_kbdPrev);
	a_container.Register("kbdNext", m_kbdNext, m_kbdNext);
	a_container.Register("kbdAbility", m_kbdAbility, m_kbdAbility);
	a_container.Register("kbdSetRapidFire", m_kbdSetRapidFire, m_kbdSetRapidFire);
	a_container.Register("kbdSetRegeneration", m_kbdSetRegeneration, m_kbdSetRegeneration);
	a_container.Register("kbdSetTimeWarp", m_kbdSetTimeWarp, m_kbdSetTimeWarp);
	a_container.Register("kbdSetMissiles", m_kbdSetMissiles, m_kbdSetMissiles);
	a_container.Register("kbdSensitivity", m_kbdSensitivity, m_kbdSensitivity);
	a_container.Register("mouseSensitivity", m_mouseSensitivity, m_mouseSensitivity);

}

void ViewAssets::RegisterPadVariables(cmn::PComplex &a_container) {
	a_container.Register("padFire", m_padFire, m_padFire);
	a_container.Register("xAxis", m_padXAxis, m_padXAxis);
	a_container.Register("yAxis", m_padYAxis, m_padYAxis);
	a_container.Register("padAbility", m_padAbility, m_padAbility);
	a_container.Register("padSetRapidFire", m_padSetRapidFire, m_padSetRapidFire);
	a_container.Register("padSetRegeneration", m_padSetRegeneration, m_padSetRegeneration);
	a_container.Register("padSetTimeWarp", m_padSetTimeWarp, m_padSetTimeWarp);
	a_container.Register("padSetMissiles", m_padSetMissiles, m_padSetMissiles);
	a_container.Register("padSensitivity", m_padSensitivity, m_padSensitivity);
}


BOOL CALLBACK EnumGameControllerCallback(LPCDIDEVICEINSTANCE a_pDeviceObject, VOID* a_pViewAssets) {
	ViewAssets &viewAssets(*(ViewAssets*)a_pViewAssets);
	LPDIRECTINPUTDEVICE8 pDevice;
	HRESULT hr;

    // Obtain an interface to the enumerated joystick.
    hr = viewAssets.m_pDInput->CreateDevice(a_pDeviceObject->guidInstance, &pDevice, NULL);
	if(FAILED(hr)) {
        return DIENUM_CONTINUE;
	}
	LOG_3_A2("Found Game Controller: %s, %s", a_pDeviceObject->tszProductName, a_pDeviceObject->tszInstanceName);

	//viewAssets.m_pGameCtrl = DInputDevice::Create(pDevice, dxrnd::DX::GetInstance().GetWindow()->GetHWnd());
	if (viewAssets.m_pGameCtrl == NULL) {
		return DIENUM_CONTINUE;
	}

	LOG_3_A2("Using Game Controller: %s, %s", a_pDeviceObject->tszProductName, a_pDeviceObject->tszInstanceName);

	return DIENUM_STOP;
}

cmn::String ViewAssets::GetPadButtonName(const cmn::String &a_prefix, int a_button) {
	cmn::String ret;
	if (m_pGameCtrl != NULL) {
		if (a_button < m_pGameCtrl->m_buttonNames.size()) {
			ret = a_prefix + m_pGameCtrl->m_buttonNames[a_button];
		}
	}

	return ret;
}

void ViewAssets::CreateGameCtrl() {

	if (m_pDInput != NULL) {

		if (m_pGameCtrl != NULL && dxrnd::DXWindow::g_deviceStatus == dxrnd::DXWindow::DS_Removed) {
			
			if (m_pGameCtrl->IsAlive() != true) {
				SaveInputSettings();
				delete m_pGameCtrl;
				m_pGameCtrl = NULL;

				dxrnd::InputDeviceFactory f;

				// get the next controller if there are more than one controller plugged in
				//m_pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumGameControllerCallback, this, DIEDFL_ATTACHEDONLY);
				m_pGameCtrl = f.CreateDevice(*m_pDInput, dxrnd::DX::GetInstance().GetWindow()->GetHWnd());
				if (m_pGameCtrl != NULL) {
					if (m_padXAxis < 0 || m_padXAxis >= m_pGameCtrl->m_axisCount) {
						m_padXAxis = m_pGameCtrl->m_xAxisIx;
					}
					if (m_padYAxis < 0 || m_padYAxis >= m_pGameCtrl->m_axisCount) {
						m_padYAxis = m_pGameCtrl->m_yAxisIx;
					}
					LoadInputSettings();
				}
			}
		} else if (m_pGameCtrl == NULL && dxrnd::DXWindow::g_deviceStatus == dxrnd::DXWindow::DS_PlugIn) {
			//m_pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumGameControllerCallback, this, DIEDFL_ATTACHEDONLY);
			dxrnd::InputDeviceFactory f;
			m_pGameCtrl = f.CreateDevice(*m_pDInput, dxrnd::DX::GetInstance().GetWindow()->GetHWnd());
			if (m_pGameCtrl != NULL) {
				if (m_padXAxis < 0 || m_padXAxis >= m_pGameCtrl->m_axisCount) {
					m_padXAxis = m_pGameCtrl->m_xAxisIx;
				}
				if (m_padYAxis < 0 || m_padYAxis >= m_pGameCtrl->m_axisCount) {
					m_padYAxis = m_pGameCtrl->m_yAxisIx;
				}
				LoadInputSettings();
			}
		}

		dxrnd::DXWindow::g_deviceStatus = dxrnd::DXWindow::DS_Idle;
	}
}


HRESULT ViewAssets::Initiate(int a_width, int a_height, HINSTANCE a_appInstance) {
	HRESULT hr;

	hr = DirectInput8Create(a_appInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&m_pDInput, NULL);
	if (FAILED(hr)) {
		LOG_3("NO Direct Input");
	} else {
		//m_pDInput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumGameControllerCallback, this, DIEDFL_ATTACHEDONLY);
		dxrnd::InputDeviceFactory f;
		m_pGameCtrl = f.CreateDevice(*m_pDInput, dxrnd::DX::GetInstance().GetWindow()->GetHWnd());
		if (m_pGameCtrl != NULL) {
			if (m_padXAxis < 0 || m_padXAxis >= m_pGameCtrl->m_axisCount) {
				m_padXAxis = m_pGameCtrl->m_xAxisIx;
			}
			if (m_padYAxis < 0 || m_padYAxis >= m_pGameCtrl->m_axisCount) {
				m_padYAxis = m_pGameCtrl->m_yAxisIx;
			}
		}
		LoadInputSettings();
	}

	cmn::PComplex spriteUVs("spriteUVs");
	for (int i = 0; i < UV_Count; i++) {
		spriteUVs.Register(g_spriteNames[i], &(m_uvs[i]));
	}
	if (spriteUVs.Load("data/sprites.xml") != true) {
		LOG_3("data/sprites.xml");
		return !D3D_OK;
	}

	for (int i = 0; i < PLAYER_MAX; i++) {
		cmn::String file("data/models/player/" + g_playerFiles[i].m_settingsFile);
		if (!m_playerData[i].Load(file)) {
			LOG_3(file.c_str());
			return !D3D_OK;
		}
	}

	hr = m_sprites.LoadTexture("data/sprites.tga");
	if (hr != D3D_OK) {
		LOG_3("data/sprites.tga");
		return hr;
	}

	hr = m_shockWave.LoadTexture("data/shockwave.bmp");
	if (hr != D3D_OK) {
		LOG_3("data/shockwave.bmp");
		return hr;
	}
	hr = m_shockWave2.LoadTexture("data/shockwave_blue.bmp");
	if (hr != D3D_OK) {
		LOG_3("data/shockwave_blue.bmp");
		return hr;
	}

	hr = m_explosion.LoadTexture("data/explosion.png");
	if (hr != D3D_OK) {
		LOG_3("data/explosion.png");
		return hr;
	}

	hr = m_cloud1.LoadTexture("data/rift_cloud1");
	if (hr != D3D_OK) {
		LOG_3("data/rift_cloud1.x");
		return hr;
	}

	hr = m_cloud2.LoadTexture("data/rift_cloud2");
	if (hr != D3D_OK) {
		LOG_3("data/rift_cloud2.x");
		return hr;
	}

	hr = m_riftRamp.LoadTexture("data/rift_ramp");
	if (hr != D3D_OK) {
		LOG_3("data/rift_ramp.x");
		return hr;
	}

	hr = m_warning.LoadTexture("data/warning");
	if (hr != D3D_OK) {
		LOG_3("data/warning.x");
		return hr;
	}

	
	for (int i = 0; i< PLAYER_MAX; i++) {
		cmn::String fileName = cmn::String("data/models/player/") + g_playerFiles[i].m_shipFile;
		if (m_playerShips[i].Load(fileName) != true) {
			LOG_3(fileName.c_str());
			return !D3D_OK;
		}

		fileName = cmn::String("data/models/player/") + g_playerFiles[i].m_diffuseFile;
		hr = m_playerTextures[i].LoadTexture(fileName);
		if (hr != D3D_OK) {
			LOG_3(fileName.c_str());
			return hr;
		}

		fileName = cmn::String("data/models/player/") + g_playerFiles[i].m_specularFile;
		hr = m_playerSpecmaps[i].LoadTexture(fileName);
		if (hr != D3D_OK) {
			LOG_3(fileName.c_str());
			return hr;
		}
	}

	

	

	hr = m_playerEffect.Load("data/effects/standard.fx");
	if (hr != D3D_OK) {
		LOG_3("data/effects/standard.fx");
		return hr;
	}

	hr = m_particleEffect.Load("data/effects/particle.fx");
	if (hr != D3D_OK) {
		LOG_3("data/effects/particle.fx");
		return hr;
	}

	hr = m_riftEffect.Load("data/effects/rift.fx");
	if (hr != D3D_OK) {
		LOG_3("data/effects/rift.fx");
		return hr;
	}

	hr = m_enemyEffect.Load("data/effects/enemy.fx");
	if (hr != D3D_OK) {
		LOG_3("data/effects/enemy.fx");
		return hr;
	}

	hr = m_barEffect.Load("data/effects/bar.fx");
	if (hr != D3D_OK) {
		LOG_3("data/effects/bar.fx");
		return hr;
	}
	m_barEffect.Get()->SetFloat("g_resolutionX", a_width);
	m_barEffect.Get()->SetFloat("g_resolutionY", a_height);

	hr = m_bgTiles.LoadTexture("data/merged_bg_tiles");
	if (hr != D3D_OK) {
		LOG_3("data/merged_bg_tiles.x");
		return hr;
	}

	hr = m_briefingBG.LoadTexture("data/brief_text-area");
	if (hr != D3D_OK) {
		LOG_3("data/brief_text-area");
		return hr;
	}

	hr = m_empireLogo.LoadTexture("data/empire_logo");
	if (hr != D3D_OK) {
		LOG_3("data/empire_logo");
		return hr;
	}
	
	for (int i = 1; i < model::Player::A_Count; i++) {
		cmn::String str;
		str.Format("data/icons/ability_%i", i);
		hr = m_abilityIcons[i - 1].LoadTexture(str);
		if (hr != D3D_OK) {
			LOG_3(str.c_str());
			return hr;
		}
	}

	hr = m_hud.LoadTexture("data/hud");
	if (hr != D3D_OK) {
		LOG_3("data/hud");
		return hr;
	}

	for (int uvIx = 0; uvIx < HUV_Count; uvIx++) {
		m_hudUVs[uvIx].m_topLeftUV.Set((float)(uvIx % 4) / 4.0f, (float)(uvIx / 4) / 4.0f);
		m_hudUVs[uvIx].m_bottomRightUV = m_hudUVs[uvIx].m_topLeftUV + math::Vector2f(1.0f / 4.0f, 1.0f / 4.0f);
	}

	int tIx = 0;
	for (; tIx < g_bgTileCount; tIx++) {
		m_tiles[tIx].Init(a_width, a_height, g_maxBgTiles);
	}
	for (; tIx < g_bgTileCount + g_decoTileCount; tIx++) {
		m_tiles[tIx].Init(a_width, a_height, g_maxDecoTiles);
	}
	for (; tIx < g_bgTileCount + g_decoTileCount + g_planetTileCount; tIx++) {
		m_tiles[tIx].Init(a_width, a_height, g_maxPlanetTiles);
	}

	for (int sIx = 0; sIx < g_starCount; sIx++) {
		m_stars[sIx].x = (float)(rand() % g_width);
		m_stars[sIx].y = (float)(rand() % g_height);
	}

	hr = m_font.Create((float)a_height / 720.0f);
	hr = m_headlinesFont.Create((float)a_height / 720.0f * 2.0f);

	if (m_soundEngine.Init(dxrnd::DX::GetInstance().GetWindow()->GetHWnd()) == true)
	{
		m_sfx.Init(m_soundEngine);
		if(!m_music.Init(m_soundEngine))
			return 0;
		LoadMixer();

		LoadSettings(NULL, NULL, NULL);
	} else {
		return 0;
	}

	return hr;
}

bool ViewAssets::LoadSettings(int *a_pWidth, int *a_pHeight, bool *a_pIsFullscreen)
{
	cmn::PComplex p("settings");
	if (a_pWidth != NULL) {
		p.Register("width", *a_pWidth, *a_pWidth);
	}
	if (a_pHeight != NULL) {
		p.Register("height", *a_pHeight, *a_pHeight);
	}
	p.Register("musicVolume", m_music.m_volume, m_music.m_volume);
	p.Register("soundVolume", m_sfx.m_volume, m_sfx.m_volume);
	if (a_pIsFullscreen != NULL) {
		p.Register("fullscreen", *a_pIsFullscreen, *a_pIsFullscreen);
	}
	p.Register("lod", m_levelOfDetail, m_levelOfDetail);

	return p.Load("userdata/settings.xml");
}

bool ViewAssets::SaveSettings(int a_width, int a_height, bool a_isFullscreen)
{
	cmn::PComplex p("settings");
	p.Register("width", a_width, a_width);
	p.Register("height", a_height, a_height);
	p.Register("musicVolume", m_music.m_volume, m_music.m_volume);
	p.Register("soundVolume", m_sfx.m_volume, m_sfx.m_volume);
	p.Register("fullscreen", a_isFullscreen, a_isFullscreen);
	p.Register("lod", m_levelOfDetail, m_levelOfDetail);
 
	return p.Save("userdata/settings.xml");
}

void ViewAssets::SaveMixer()
{
	cmn::PComplex p("mixer");

	m_sfx.Register(p);
	m_music.Register(p);
 
	p.Save("data/mixer.xml");
}

void ViewAssets::LoadMixer()
{
	cmn::PComplex p("mixer");

	m_sfx.Register(p);
	m_music.Register(p);
 
	p.Load("data/mixer.xml");
}

void ViewAssets::BGTile::Init(int a_maxWidth, int a_maxHeight, int a_maxTiles) {
	a_maxHeight += 600;
	m_pos.x = (float)(rand() % a_maxWidth);
	m_pos.y = (float)(rand() % a_maxHeight * 2) - a_maxHeight;
	m_tileIx = rand() % a_maxTiles;
	m_angle = (float)(rand() % 1001) / 1000.0f * PI_DOUBLE;
	m_rotSpeed = ((float)(rand() % 1001) / 1000.0f - 0.5f) * 0.25f;
}

void ViewAssets::BGTile::Randomize(int a_maxWidth, int a_maxHeight, int a_maxTiles) {
	m_pos.x = (float)(rand() % a_maxWidth);
	m_pos.y = (float)-((rand() % a_maxHeight) + 600);
	m_tileIx = rand() % a_maxTiles;
	m_rotSpeed = ((float)(rand() % 1001) / 1000.0f - 0.5f) * 0.25f;
	m_angle = (float)(rand() % 1001) / 1000.0f * PI_DOUBLE;
}


}