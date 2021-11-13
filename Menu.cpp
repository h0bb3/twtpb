// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "Menu.h"
#include "Strings.h"
#include "BarSprite.h"
#include <shellapi.h>


namespace view {

Menu::Menu(ViewCore &m_core) : m_core(m_core) {
	ResetMenu();
	m_startScore = 0;
	m_kbdInput = KI_None;
	m_gamePadInput = GPI_None;
	m_hotButton = MM_Difficulty;

}

Menu::~Menu(void)
{
}


Menu::MenuStatus Menu::DoMenu(const float a_dt, const model::Model &a_model) {
	m_core.SetMouse(true);

	if (m_mainMenu == MM_Credits) {
		DoCredits(a_dt);
		return MS_Ok;
	}

	math::Vector2f dMouse(0.0f, 0.0f);

	static float time = 0.0f;
	time += a_dt;
		
	m_core.DrawStaticBackground(time);

	m_core.DrawTWTPBLogo(time, 100.0f);

	if (a_model.m_serverVersion > 0 && a_model.m_serverVersion > g_version) {
		// Old version...
		m_core.DoLabel(0, 200, g_width, g_height - 100, OLD_TEXT, DT_CENTER | DT_TOP);
		if (DoHotMenuButton(g_width / 2 - 70, 250, 140, 25, OLD_GET, false) == ViewCore::WS_Clicked) {
			SHELLEXECUTEINFO info;
							
			ZeroMemory(&info, sizeof(info));
			info.cbSize = sizeof(info);
			info.lpVerb = "open";
			info.lpFile = "http://www.spellofplay.com/games/twtpb";
			LONG r = ShellExecuteEx(&info);

			return MS_Quit;
		}



		return MS_Ok;
	}


	switch (m_mainMenu) {
		case MM_Highscores:
		{
			DoHeadline(140, HIGSCORE_HEADLINE);

			if (m_hotButton == g_noneId) {
				m_hotButton = 0;
			}
			if (m_core.IsLeftPressed() && m_hotButton == 0) {
				SetHotButton(1);	// previous
			}
			if (m_core.IsRightPressed() && m_hotButton == 1) {
				SetHotButton(0);
			}
			if (DoDown() && (m_hotButton == 0 || m_hotButton == 1)) {
				SetHotButton(g_backButtonId);
			}
			if (DoUp() && g_backButtonId == m_hotButton) {
				SetHotButton(0);
			}


			m_startScore -= m_core.GetMouseWheelStatus();
			if (m_startScore < 0) {
				m_startScore = 0;
			} if (m_startScore + 25 >= a_model.m_scores.m_scores.size()) {
				m_startScore = a_model.m_scores.m_scores.size() - 26;
				if (m_startScore < 0) {
					m_startScore = 0;
				}
			}

			const int pageSize(30);
			cmn::String places, names, scores;
			for (size_t i = m_startScore; i < m_startScore + pageSize && i < a_model.m_scores.m_scores.size(); i++) {
				cmn::String str;

				str.Format("%i\n", i + 1);
				places += str;

				names += a_model.m_scores.m_scores[i].m_name;
				names += "\n";

				str.Format("%i\n", a_model.m_scores.m_scores[i].m_score);
				scores += str;
			}

			if (DoMenuButton(g_width / 2 - 70 - 75, g_height - 90, 140, 25, HIGSCORE_PREV, 1)) {
				m_startScore -= pageSize;
			}
			if (DoMenuButton(g_width / 2 - 70 + 75, g_height - 90, 140, 25, HIGSCORE_NEXT, 0)) {
				m_startScore += pageSize;
			}

			m_core.DoLabel(250, 170, 50, g_height - 170, places, DT_RIGHT | DT_TOP);
			m_core.DoLabel(300, 170, 200, g_height - 170, names, DT_CENTER | DT_TOP);
			m_core.DoLabel(500, 170, 200, g_height - 170, scores, DT_LEFT | DT_TOP);

			if (DoBackButton(g_height - 50)) {
				m_hotButton = (int)MM_Highscores;
				m_mainMenu = MM_None;
			}
		}
		break;
		case MM_None:
		{
			cmn::WString status;
			m_core.GetMouseWheelStatus(); // resets the m-wheel status
			const model::HighScores::State state(a_model.m_scores.GetState());
			switch (state) {
				case model::HighScores::S_Done:
					status = L"\n\n";
				break;
				case model::HighScores::S_Error:
					status = STATUS_NOSERVER;
				break;
				case model::HighScores::S_GettingScores:
					status = STATUS_DOWNLOADING;
				break;
				case model::HighScores::S_UploadingScore:
					status = STATUS_UPLOADING;
				break;
			};

			if (a_model.IsTrial() != true) {
				status += STATUS_REGISTERED;
			}
					
			status += STATUS_COPYRIGHT;

			m_core.DoLabel(0, 450, g_width, g_height - 460, status, DT_CENTER | DT_BOTTOM);

			if (DoUp() && m_hotButton > (int)MM_Difficulty) {
				SetHotButton(m_hotButton - 1);
				if (a_model.IsTrial() != true && m_hotButton == MM_Buy) {
					SetHotButton(m_hotButton - 1);
				}
			}
			if (DoDown() && m_hotButton < MM_Count - 1) {
				SetHotButton(m_hotButton + 1);
				if (a_model.IsTrial() != true && m_hotButton == MM_Buy) {
					SetHotButton(m_hotButton + 1);
				}
			}


			if (a_model.m_isPaused) {
				if (DoMenuButton(g_width / 2 - 70, 150, 140, 25, CONTINUE, (int)MM_Difficulty)) {
					ResetMenu();
					return MS_ContinueGame;
				}
			} else {
				if (DoMenuButton(g_width / 2 - 70, 150, 140, 25, START_GAME, (int)MM_Difficulty)) {
					m_mainMenu = MM_Difficulty;
					m_hotButton = g_noneId;
				}
			}

			if (DoMenuButton(g_width / 2 - 70, 195, 140, 25, SHOW_HIGHSCORES, (int)MM_Highscores)) {
				m_mainMenu = MM_Highscores;
				m_hotButton = g_noneId;
			}

			if (DoMenuButton(g_width / 2 - 70, 240, 140, 25, SHOW_OPTIONS, (int)MM_Options)) {
				m_hotButton = g_noneId;
				m_mainMenu = MM_Options;
				m_options = O_None;
				m_inputOptions = IO_None;
				m_gfxOptions = GO_None;
			}

			if (DoMenuButton( g_width / 2 - 70, 285, 140, 25, SHOW_CREDITS, (int)MM_Credits)) {
				m_mainMenu = MM_Credits;
				m_creditsYScroll = 610;
			}

			if (a_model.IsTrial() && DoBuyButton(330)) {
				return MS_Reset;
			}

			if (a_model.m_isPaused) {
				if (DoMenuButton(g_width / 2 - 70, 390, 140, 25, QUIT_CURRENT_GAME, (int)MM_Quit)) {
					return MS_QuitGame;
				}
			} else {
				if (DoMenuButton(g_width / 2 - 70, 390, 140, 25, EXIT_GAME, (int)MM_Quit)) {
					return MS_Quit;
				}
			}
		}
		break;
		case MM_Start:
		{
			cmn::WString text;

			DoHeadline(180, JUMPGATES_HEADLINE);
			text = JUMPGATES_HELPTEXT1;
			m_core.DoLabel(g_width / 2 - 200, 230, 400, 60, text, DT_TOP | DT_CENTER);

			if (m_hotButton == g_noneId) {
				m_hotButton = 0;
			}
			if (m_hotButton < a_model.m_unlockedLevels && m_core.IsRightPressed()) {
				SetHotButton(m_hotButton + 1);
			}
			if (m_hotButton > 0 && m_core.IsLeftPressed()) {
				SetHotButton(m_hotButton - 1);
			}
			if (m_hotButton != g_backButtonId && DoDown()) {
				SetHotButton(g_backButtonId);
			}
			if (m_hotButton == g_backButtonId && DoUp()) {
				SetHotButton(0);
			}


			if (a_model.IsTrial() != true) {
				text = JUMPGATES_HELPTEXT2;
			} else {
				text = JUMPGATES_HELPTEXT2_NOT_REGISTERED;
			}
			m_core.DoLabel(0, 320, g_width, 60, text, DT_TOP | DT_CENTER);

			for (int i = 0; i < model::Model::g_maxLevels; i++) {
				const int w(55);
				cmn::WString str;

				str.Format(JUMPGATES_GATE, i + 1);
				const int x(g_width / 2 - (w * model::Model::g_maxLevels / 2) + w * i);

				if ((i <= a_model.m_unlockedLevels && (i < 4 || a_model.IsTrial() == false)) || i == 0 ) {
					if (DoMenuButton(x, 270, w - 5, 25, str, i)) {
						ResetMenu();
						m_level = i;
						return MS_StartGame;

					}
				} else {
					m_core.DoInactiveButton(x, 270, w - 5, 25, str);
				}
			}

			if (DoBackButton(450)) {
				m_mainMenu = MM_Difficulty;
				m_hotButton = g_noneId;
			}
		}
		break;
		case MM_Difficulty:
		{
			const int maxButton(a_model.IsTrial() ? 1 : 2);

			if (m_hotButton == g_noneId) {
				m_hotButton = 0;
			}
			if (m_hotButton < maxButton && m_core.IsRightPressed()) {
				SetHotButton(m_hotButton + 1);
			}
			if (m_hotButton > 0 && m_core.IsLeftPressed()) {
				SetHotButton(m_hotButton - 1);
			}
			if (m_hotButton != g_backButtonId && DoDown()) {
				SetHotButton(g_backButtonId);
			}
			if (m_hotButton == g_backButtonId && DoUp()) {
				SetHotButton(0);
			}

			DoHeadline(180, LEGIO_HEADLINE);
			m_core.DoLabel(g_width / 2 - 200, 230, 400, 60, LEGIO_HELPTEXT, DT_TOP | DT_CENTER);

			const int joinYPos(a_model.IsTrial() ? 450 : 420);

			m_core.DoHeadline(g_width * 0.5f - 230 - 95, 270, 190, 30, LEGIO_EASY_HEADLINE, DT_TOP);
			m_core.DoLabel((int)(g_width * 0.5f - 230 - 95), 300, 190, 100, LEGIO_EASY, DT_TOP | DT_LEFT | DT_WORDBREAK);
			if (DoMenuButton(g_width * 0.5f - 230 - 75, joinYPos, 150, 25, LEGIO_EASY_JOIN, 0)) {
				m_mainMenu = MM_Start;
				m_hotButton = g_noneId;
				m_difficulty = 0.5f;
			}

			m_core.DoHeadline(g_width * 0.5f - 95, 270, 190, 30, LEGIO_MEDIUM_HEADLINE, DT_TOP);
			m_core.DoLabel((int)(g_width * 0.5f - 95), 300, 190, 100, LEGIO_MEDIUM, DT_TOP | DT_LEFT | DT_WORDBREAK);
			if (DoMenuButton(g_width * 0.5f - 75, joinYPos, 150, 25, LEGIO_MEDIUM_JOIN, 1)) {
				m_mainMenu = MM_Start;
				m_hotButton = g_noneId;
				m_difficulty = 1.0f;
			}

			m_core.DoHeadline(g_width * 0.5f + 230 - 95, 270, 190, 30, LEGIO_HARD_HEADLINE, DT_TOP);
			if (a_model.IsTrial()) {
				m_core.DoLabel((int)(g_width * 0.5f + 230 - 95), 300, 220, 120, (const cmn::WString&)(cmn::WString(LEGIO_HARD) + LEGIO_HARD_NOT_REGISTERED), DT_TOP | DT_LEFT | DT_WORDBREAK);
				m_core.DoInactiveButton(g_width * 0.5f + 230 - 75, joinYPos, 150, 25, LEGIO_HARD_JOIN_NOT_REGISTERED);
			} else {
				m_core.DoLabel((int)(g_width * 0.5f + 230 - 95), 300, 190, 100, LEGIO_HARD, DT_TOP | DT_LEFT | DT_WORDBREAK);
				if (DoMenuButton(g_width * 0.5f + 230 - 75, joinYPos, 150, 25, LEGIO_HARD_JOIN, 2)) {
					m_mainMenu = MM_Start;
					m_hotButton = g_noneId;
					m_difficulty = 1.5f;
				}
			}

			if (DoBackButton(500)) {
				m_hotButton = m_mainMenu;
				m_mainMenu = MM_None;
			}
		}
		break;
		case MM_Options:
		{
			switch (m_options) {
				case O_None:
				{

					if (m_hotButton == g_noneId) {
						m_hotButton = O_None + 1;
					}

					if ((m_hotButton > 0 || m_hotButton == g_backButtonId) && DoUp()) {
						if (m_hotButton == g_backButtonId) {
							m_hotButton = O_Count;
						}
						SetHotButton(m_hotButton - 1);
					}
					if (m_hotButton < (int)O_Count && m_hotButton != g_backButtonId && DoDown()) {
						SetHotButton(m_hotButton + 1);
						if (m_hotButton >= O_Count) {
							m_hotButton = g_backButtonId;
						}
					}

					DoHeadline(140, OPTIONS_HEADLINE);
					const static cmn::ConstWString options[O_Count] = {cmn::ConstWString(OPTIONS_SHOW_GRAPHICS), cmn::ConstWString(OPTIONS_SHOW_AUDIO), cmn::ConstWString(OPTIONS_SHOW_CONTROLS)};
					for (int i = 0; i < O_Count; i++) {
						if (DoMenuButton(g_width / 2 - 70, 180 + i * 40, 140, 25, options[i], i)) {
							m_options = (Options)i;
							m_hotButton = g_noneId;
						}
					}

					if (DoBackButton(340)) {
						m_mainMenu = MM_None;
						m_hotButton = MM_Options;
					}
				}
				break;
				case O_Input:
				{
					switch (m_inputOptions) {
						case IO_None:
						{

							if (m_hotButton == g_noneId) {
								m_hotButton = IO_Mouse;
							}
							if ((m_hotButton > IO_Mouse || m_hotButton == g_backButtonId) && DoUp()) {
								if (m_hotButton == g_backButtonId) {
									SetHotButton(HasPad() ? IO_GamePad : IO_Keyboard);
								} else {
									SetHotButton(m_hotButton - 1);
								}
							}
							if ((m_hotButton < IO_Count && m_hotButton != g_backButtonId) && DoDown()) {
								SetHotButton(m_hotButton + 1);
								if (HasPad() == false && m_hotButton == IO_GamePad) {
									m_hotButton = g_backButtonId;
								}
								if (m_hotButton == IO_Count) {
									m_hotButton = g_backButtonId;
								}
							}

							DoHeadline(140, CONTROLS_HEADLINE);
							m_gamePadInput = GPI_None;
							const static cmn::ConstWString options[IO_Count] = {cmn::ConstWString(CONTROLS_SHOW_MOUSE), cmn::ConstWString(CONTROLS_SHOW_KEYBOARD), cmn::ConstWString(CONTROLS_SHOW_GAMEPAD) };
							for (int i = 0; i < O_Count; i++) {
								if (i == IO_GamePad) {
									if (HasPad()) {
										if (DoMenuButton(g_width / 2 - 70, 180 + i * 40, 140, 25, options[i], i)) {
											m_inputOptions = (InputOptions)i;
											m_hotButton = g_noneId;
										}
									}
								} else {
									if (DoMenuButton(g_width / 2 - 70, 180 + i * 40, 140, 25, options[i], i)) {
										m_inputOptions = (InputOptions)i;
										m_hotButton = g_noneId;
									}
								}
							}

							if (DoBackButton(340)) {
								m_options = O_None;
								m_hotButton = O_Input;
							}
						}
						break;
						case IO_Mouse:
							DoMouseSettings();
						break;
						case IO_GamePad:
							DoGamePadSettings();
						break;
						case IO_Keyboard:
							DoKeyboardSettings();
						break;
					}
				}
				break;
				case O_Graphics:
				{
					switch (m_gfxOptions) {
						case GO_None:
						{
							if (m_hotButton == g_noneId) {
								m_hotButton = GO_LevelOfDetail;
							}
							if ((m_hotButton > GO_LevelOfDetail || m_hotButton == g_backButtonId) && DoUp()) {
								if (m_hotButton == g_backButtonId) {
									SetHotButton(GO_Display);
								} else {
									SetHotButton(m_hotButton - 1);
								}
							}
							if ((m_hotButton < GO_Count && m_hotButton != g_backButtonId) && DoDown()) {
								SetHotButton(m_hotButton + 1);
								if (m_hotButton == GO_Count) {
									m_hotButton = g_backButtonId;
								}
							}

							DoHeadline(140, L"Graphic Options");
							m_gamePadInput = GPI_None;
							const static cmn::ConstWString options[GO_Count] = {cmn::ConstWString(L"Level of Detail"), cmn::ConstWString(L"Select Resolution") };
							for (int i = 0; i < GO_Count; i++) {
								if (DoMenuButton(g_width / 2 - 70, 180 + i * 40, 140, 25, options[i], i)) {
										m_gfxOptions = (GraphicOptions)i;
										m_hotButton = g_noneId;
								}
							}

							if (DoBackButton(340)) {
								m_options = O_None;
								m_hotButton = O_Input;
							}
						}
						break;
						case GO_Display:
							if (DoGfxOptions()) {
								return MS_Reset;
							}
						break;
						case GO_LevelOfDetail:
							DoLevelOfDetail();
						break;
					}
			
				}
				break;
				
				case O_Sound:
				{
					DoSoundOptions();
				}
				break;
			}
		}
		break;
	};


	m_core.m_input.IsClicked(m_core.m_assets.m_mouseFire);

	return MS_Ok;

}


bool Menu::DoBuyButton(int a_y) {
	if (DoMenuButton(g_width / 2 - 70, a_y, 140, 25, MENU_BUY, g_buyButtonId)) {
		SHELLEXECUTEINFO info;
		
		ZeroMemory( &info, sizeof(info) );
		info.cbSize = sizeof(info);
		info.lpVerb = "open";
		info.lpFile = "http://www.spellofplay.com/games/twtpb/buy";
		LONG r = ShellExecuteEx(&info);

		if (m_core.m_isFullscreen) {
			m_core.m_isFullscreen = false;
			true;
		}
	}
	return false;
}

bool Menu::DoBackButton(int a_y) {
	if (DoMenuButton(g_width / 2 - 70, a_y, 140, 25, MENU_BACK, g_backButtonId)) {
		return true;
	}
	return false;
}

void Menu::DoMouseSettings() {
	DoHeadline(140, MOUSE_HEADLINE);

	if (m_hotButton == g_noneId) {
		m_hotButton = 1;
	}

	if (m_hotButton > 1 && DoUp()) {
		SetHotButton(m_hotButton - 1);
	}
	if (m_hotButton == 1 && DoDown()) {
		SetHotButton(m_hotButton + 1);
	}
	if (m_hotButton == 2 && DoDown()) {
		SetHotButton(g_backButtonId);
	}
	if (m_hotButton == g_backButtonId && DoUp()) {
		SetHotButton(2);
	}
	
	int ypos(180);

	if (DoAmountButton(g_width / 2 - 70 - 75, ypos, 290, 20, MOUSE_SENSITIVITY, m_core.m_assets.m_mouseSensitivity, 0.1f, 4.0f, 1)) {
		m_gamePadInput = GPI_None;
	}

	ypos += 40;
	if (DoMenuButton(g_width / 2 - 70, ypos, 140, 25, MOUSE_RESET, 2)) {
		m_core.m_assets.ResetMouseSettingsToDefault();
	}
	ypos += 80;
	if (DoBackButton(ypos)) {
		m_inputOptions = IO_None;
		m_hotButton = IO_Mouse;
	}
}

void Menu::DoKeyboardSettings() {
	DoHeadline(140, KEYBOARD_HEADLINE);

	if (m_hotButton == g_noneId) {
		m_hotButton = KI_MoveLeft;
	}

	if (m_kbdInput == KI_None) {
		if (m_hotButton > KI_MoveLeft && m_hotButton < KI_Count && m_core.IsLeftPressed()) {
			SetHotButton(m_hotButton - 1);
		}
		if (m_hotButton < KI_Count - 1 && m_core.IsRightPressed()) {
			SetHotButton(m_hotButton + 1);
		}

		if (m_hotButton > KI_MoveRight && DoUp()) {
			if (m_hotButton >= KI_Count) {
				SetHotButton(m_hotButton - 1);
			} else {
				SetHotButton(m_hotButton - 2);
			}
		}
		if (m_hotButton != g_backButtonId && m_hotButton < KI_Count + 1 && DoDown()) {
			if (m_hotButton < KI_Count - 1) {
				SetHotButton(m_hotButton + 2);
			} else {
				SetHotButton(m_hotButton + 1);
			}
		}

		if (m_hotButton == KI_Count + 1 && DoDown()) {
			SetHotButton(g_backButtonId);
		}

		if (m_hotButton == g_backButtonId && DoUp()) {
			SetHotButton(KI_Count + 1);
		}
	}
	
	int * inputIndices[KI_Count] = {	&m_core.m_assets.m_kbdLeft,
										&m_core.m_assets.m_kbdRight,
										&m_core.m_assets.m_kbdUp,
										&m_core.m_assets.m_kbdDown,
										&m_core.m_assets.m_kbdPrev,
										&m_core.m_assets.m_kbdNext,
										&m_core.m_assets.m_kbdFire, &m_core.m_assets.m_kbdAbility,
										&m_core.m_assets.m_kbdSetRapidFire, &m_core.m_assets.m_kbdSetRegeneration,
										&m_core.m_assets.m_kbdSetTimeWarp, &m_core.m_assets.m_kbdSetMissiles
									};
	
	const cmn::ConstWString options[KI_Count] = {cmn::ConstWString(cmn::WString(KEYBOARD_MOVE_LEFT)),
												cmn::ConstWString(cmn::WString(KEYBOARD_MOVE_RIGHT)),
												cmn::ConstWString(cmn::WString(KEYBOARD_MOVE_UP)),
												cmn::ConstWString(cmn::WString(KEYBOARD_MOVE_DOWN)),
												cmn::ConstWString(cmn::WString(KEYBOARD_PREV_ABILITY)),
												cmn::ConstWString(cmn::WString(KEYBOARD_NEXT_ABILITY)),
												cmn::ConstWString(cmn::WString(KEYBOARD_FIRE)),
												cmn::ConstWString(cmn::WString(KEYBOARD_USE_ABILITY)),
												cmn::ConstWString(cmn::WString(KEYBOARD_SET_RAPIDFIRE)),
												cmn::ConstWString(cmn::WString(KEYBOARD_SET_REGENERATION)),
												cmn::ConstWString(cmn::WString(KEYBOARD_SET_TIMEWARP)),
												cmn::ConstWString(cmn::WString(KEYBOARD_SET_MISSILES))};



	int xpos(g_width / 2 - 70 - 75);
	int ypos(180);
	for (int oIx = 0; oIx < KI_Count; oIx++) {
		if (m_kbdInput == (KeyboardInput)oIx) {
			m_hotButton = oIx;
			if (m_core.m_input.IsAnyKeyPressed()) {
				for (int i = 254; i > 0; i--) {	// go towards 1 since it seems that right buttons also generate a standard button click?
					if (i != m_core.m_assets.m_mouseFire && i != VK_RETURN && i != VK_ESCAPE && m_core.m_input.IsPressedVK(i)) {
						*inputIndices[oIx] = i;
						m_kbdInput = KI_None;
						break;
					}
				}
			}
		}

		cmn::WString title(options[oIx]);
		if (m_kbdInput != (KeyboardInput)oIx) {
			title += cmn::WString(VKToString(*inputIndices[oIx]));
		}

		if (DoMenuButton(xpos, ypos, 140, 20, title, oIx)) {
			if (m_kbdInput == (KeyboardInput)oIx) {
				m_kbdInput = KI_None;
			} else {
				m_kbdInput = (KeyboardInput)oIx;
			}
		}

		if (oIx % 2 == 1) {
			xpos = g_width / 2 - 70 - 75;
			ypos += 40;
		} else {
			xpos = g_width / 2 - 70 + 75;
		}
	}

	if (DoAmountButton(g_width / 2 - 70 - 75, ypos, 290, 20, KEYBOARD_SENSITIVITY, m_core.m_assets.m_kbdSensitivity, 0.1f, 4.0f, KI_Count)) {
		m_gamePadInput = GPI_None;
	}

	ypos += 40;
	if (DoMenuButton(g_width / 2 - 70, ypos, 140, 25, KEYBOARD_RESET, KI_Count + 1)) {
		m_core.m_assets.ResetKbdSettingsToDefault();
	}
	ypos += 80;
	if (DoBackButton(ypos)) {
		m_kbdInput = KI_None;
		m_inputOptions = IO_None;
		m_hotButton = IO_Keyboard;
	}
}

cmn::String Menu::VKToString(int a_vk) {
	cmn::String ret("unknown");
	char buffer[256];
	UINT code;

	code = MapVirtualKey(a_vk, 0);
	if (code > 0) {
		// this seems to mupp out on XP?
		switch(a_vk) {
			case VK_INSERT:
			case VK_DELETE:
			case VK_HOME:
			case VK_END:
			case VK_NEXT:
			case VK_PRIOR:
			case VK_LEFT:
			case VK_RIGHT:
			case VK_UP:
			case VK_DOWN:
			//case VK_RMENU:
			//case VK_RCONTROL:
			//case VK_RSHIFT:
				code |= 0x100; // Add extended bit
			//	code |= 0x0; // Add extended bit
		}

		if (GetKeyNameText(code << 16, buffer, sizeof(buffer)) > 0) {
			ret = buffer;
		}
	}

	return ret;
}

void Menu::DoGamePadSettings() {
	if (HasPad() == false) {
		m_inputOptions = IO_None;
		return;
	}


	if (m_hotButton == g_noneId) {
		m_hotButton = GPI_MoveLeftRight;
	}

	if (m_gamePadInput == KI_None) {
		if (m_hotButton > GPI_MoveLeftRight && m_hotButton < GPI_Count && m_core.IsLeftPressed()) {
			SetHotButton(m_hotButton - 1);
		}
		if (m_hotButton < GPI_Count - 1 && m_core.IsRightPressed()) {
			SetHotButton(m_hotButton + 1);
		}

		if (m_hotButton > GPI_MoveUpDown && DoUp()) {
			if (m_hotButton >= GPI_Count) {
				SetHotButton(m_hotButton - 1);
			} else {
				SetHotButton(m_hotButton - 2);
			}
		}
		if (m_hotButton != g_backButtonId && m_hotButton < GPI_Count + 1 && DoDown()) {
			if (m_hotButton < GPI_Count - 1) {
				SetHotButton(m_hotButton + 2);
			} else {
				SetHotButton(m_hotButton + 1);
			}
		}

		if (m_hotButton == GPI_Count + 1 && DoDown()) {
			SetHotButton(g_backButtonId);
		}

		if (m_hotButton == g_backButtonId && DoUp()) {
			SetHotButton(GPI_Count + 1);
		}
	}

	
	DoHeadline(140, CONTROLLER_HEADLINE);
	m_core.DoLabel(0, 155, g_width, 25, GetPadName(), DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	int * inputIndices[GPI_Count] = {	&m_core.m_assets.m_padXAxis,
										&m_core.m_assets.m_padYAxis,
										&m_core.m_assets.m_padFire, &m_core.m_assets.m_padAbility,
										&m_core.m_assets.m_padSetRapidFire, &m_core.m_assets.m_padSetRegeneration,
										&m_core.m_assets.m_padSetTimeWarp, &m_core.m_assets.m_padSetMissiles
									};
	const cmn::ConstWString options[GPI_Count] = {cmn::ConstWString(CONTROLLER_MOVE_LEFT_RIGHT),
												cmn::ConstWString(CONTROLLER_MOVE_UP_DOWN),
												cmn::ConstWString(CONTROLLER_FIRE),
												cmn::ConstWString(CONTROLLER_USE_ABILITY),
												cmn::ConstWString(CONTROLLER_SET_RAPIDFIRE),
												cmn::ConstWString(CONTROLLER_SET_REGENERATION),
												cmn::ConstWString(CONTROLLER_SET_TIMEWARP),
												cmn::ConstWString(CONTROLLER_SET_MISSILES)};



	int xpos(g_width / 2 - 70 - 75);
	int ypos(180);
	for (int oIx = 0; oIx < GPI_Count; oIx++) {
		if (m_gamePadInput == (GamePadInput)oIx) {
			m_hotButton = oIx;
			if (oIx < 2) {
				int axIx(GetFirstPadAxisMovement());
				if (axIx >= 0) {
					*inputIndices[oIx] = axIx;
					m_gamePadInput = GPI_None;
				}

			} else {
				int bIx(GetFirstPadButtonPressed());
				if (bIx >= 0) {
					*inputIndices[oIx] = bIx;
					m_gamePadInput = GPI_None;
				}
			}
		}

		cmn::WString title(options[oIx]);
		if (m_gamePadInput != (GamePadInput)oIx) {
			if (oIx < 2) {
				title += cmn::WString(GetPadAxisName(*inputIndices[oIx]));
			} else {
				title += cmn::WString(GetPadButtonName(*inputIndices[oIx]));
			}
		}

		if (DoMenuButton(xpos, ypos, 140, 20, title, oIx)) {
			if (m_gamePadInput == (GamePadInput)oIx) {
				m_gamePadInput = GPI_None;
			} else {
				m_gamePadInput = (GamePadInput)oIx;
			}
		}
		if (oIx % 2 == 1) {
			xpos = g_width / 2 - 70 - 75;
			ypos += 40;
		} else {
			xpos = g_width / 2 - 70 + 75;
		}
	}

	if (DoAmountButton(g_width / 2 - 70 - 75, ypos, 290, 20, CONTROLLER_SENSITIVITY, m_core.m_assets.m_padSensitivity, 0.1f, 4.0f, GPI_Count)) {
		m_gamePadInput = GPI_None;
	}

	ypos += 40;
	if (DoMenuButton(g_width / 2 - 70, ypos, 140, 25, CONTROLLER_RESET, GPI_Count + 1)) {
		m_core.m_assets.ResetPadSettingsToDefault();
	}

	ypos += 80;
	if (DoBackButton(ypos)) {
		m_inputOptions = IO_None;
	}
}

void Menu::DrawCredits(const float a_yPos, const float a_time) {

	m_core.DrawTWTPBLogo(a_time, a_yPos);

	float ypos(a_yPos + 40.0f);
	ypos += DrawCreditsSection(ypos, "GAME DESIGN and CONCEPT", "Tobias 'hObbE' Olsson");
	ypos += DrawCreditsSection(ypos, "PROGRAMMING", "Tobias 'hObbE' Olsson\nJohannes 'johno' Norneby\nDaniel 'dntoll' Toll");
	ypos += DrawCreditsSection(ypos, "GRAPHICS", "Mikael 'spjelk' Spejelkavik\nTobias 'hObbE' Olsson\nJohannes 'johno' Norneby");
	ypos += DrawCreditsSection(ypos, "MUSIC and AUDIO", "Mats 'samplerinfo' Persson\nJohannes 'johno' Norneby\nTobias 'hObbE' Olsson");
	ypos += DrawCreditsSection(ypos, "SPECIAL THANKS", "Martin Blomberg, freelancer, miniyou\ntann, Solidbenny, TDJ, malice\nand all the other people at the Spell of Play Community.\nYou guys rock!");
	ypos += DrawCreditsSection(ypos, "LINKS", "www.spellofplay.com\nwww.johno.se");
}

void Menu::DoCredits(const float a_dt) {
	static char lmbState = 0;
	static float time = 0.0f;
	time += a_dt;

	DrawCredits(m_creditsYScroll, time);

	m_creditsYScroll -= a_dt * 20.0f;
	if (m_creditsYScroll < -600.0f) {
		m_creditsYScroll = 610.0f;
	}

	if (m_core.IsButtonUpDownUp(m_core.m_input.IsPressedVKRaw(m_core.m_assets.m_mouseFire), lmbState)) {
		lmbState = 0;
		m_mainMenu = MM_None;
		m_hotButton = MM_Credits;
	}	
}

float Menu::DrawCreditsSection(float a_ypos, const cmn::String &a_headline, const cmn::String &a_body) {
	const float xpos(g_width * 0.5f - 100);
	const float width(300);
	float height(0.0f);
	RECT rect;

	m_core.m_assets.m_headlinesFont.DrawText(a_headline, rect, DT_CALCRECT, 0);
	height += (float)(rect.bottom - rect.top) / m_core.m_scale.y;
	m_core.DoHeadline(xpos, a_ypos, width, height, a_headline, DT_TOP | DT_LEFT);
	height += 5.0f;
	a_ypos += height;

	m_core.m_assets.m_font.DrawText(a_body, rect, DT_CALCRECT, 0);
	height += (float)(rect.bottom - rect.top) / m_core.m_scale.y;

	m_core.DoLabel(xpos, a_ypos, width, height, a_body, DT_TOP | DT_LEFT);

	height += 13.0f;

	return height;
}

void Menu::DoSoundOptions() {
	DoHeadline(140, AUDIO_HEADLINE);

	if (m_hotButton == g_noneId) {
		m_hotButton = 1;
	}

	if (m_hotButton > 1 && DoUp()) {
		SetHotButton(m_hotButton - 1);
	}
	if (m_hotButton == 1 && DoDown()) {
		SetHotButton(m_hotButton + 1);
	}
	if (m_hotButton == 2 && DoDown()) {
		SetHotButton(g_backButtonId);
	}
	if (m_hotButton == g_backButtonId && DoUp()) {
		SetHotButton(2);
	}

	float volume(m_core.m_assets.m_music.m_volume * 100.0f);
	if (DoAmountButton(g_width / 2 - 70 - 75, 180, 290, 25, AUDIO_MUSIC_VOLUME, volume, 0, 100.0, 1)) {
		m_core.m_assets.m_music.m_volume = volume * 0.01f;
	}

	volume = m_core.m_assets.m_sfx.m_volume * 100.0f;
	if (DoAmountButton(g_width / 2 - 70 - 75, 215, 290, 25, AUDIO_SOUND_VOULME, volume, 0, 100.0, 2)) {
		m_core.m_assets.m_sfx.m_volume = volume * 0.01f;
	}

	if (DoBackButton(260)) {
		m_hotButton = m_options;
		m_options = O_None;
	}
}

void Menu::DoLevelOfDetail() {
	DoHeadline(140, L"Level of Detail");

	if (m_hotButton == g_noneId) {
		m_hotButton = m_core.m_assets.m_levelOfDetail;
	}

	if (m_hotButton != g_backButtonId && (DoUp() || DoDown())) {
		SetHotButton(g_backButtonId);
	}

	if (m_hotButton != g_backButtonId) {
		if (m_core.IsLeftPressed()) {
			SetHotButton((m_hotButton - 1) % 3);
		} else if (m_core.IsRightPressed()) {
			SetHotButton((m_hotButton + 1) % 3);
		}

	}

	if (m_hotButton == g_backButtonId && (DoUp() || DoDown())) {
		SetHotButton(0);
	}

	cmn::ConstWString texts[3] = {L"Low", L"Medium", L"Awesome"};
	for (int i = 0; i < 3; i++) {
		if (DoOptionsButton(g_width / 2 - 140 - 75 + i * 145, 180, 140, 25, texts[i], i, m_core.m_assets.m_levelOfDetail == i)) {
			m_core.m_assets.m_levelOfDetail = i;
		}
	}

	if (DoBackButton(260)) {
		m_hotButton = m_gfxOptions;
		m_gfxOptions = GO_None;
	}
}
bool Menu::DoGfxOptions() {
	return DoResolutions();
}
bool Menu::DoResolutions() {
	DoHeadline(140, RESOLUTION_HEADLINE);
	int ypos(180);
	int currentResolutionIx;
	int hotButtonDelta(0);

	for (int i = 0; i < m_core.m_resolutions.size(); i++) {
		if (m_core.m_resolutions[i].m_width == m_core.m_width && m_core.m_resolutions[i].m_height == m_core.m_height) {
			currentResolutionIx = i;
			break;
		}
	}


	if (m_hotButton == g_noneId) {
		m_resolutionIx = currentResolutionIx;
	}

	const float factor((float)m_core.m_resolutions[m_resolutionIx].m_width / (float)m_core.m_resolutions[m_resolutionIx].m_height);
	cmn::WString strFactor;
	if (factor == 16.0f / 10.0f) {
		strFactor = L"(16:10)";
	} else if (factor == 16.0f / 9.0f) {
		strFactor = L"(16:9)";
	} else if (factor == 3.0f / 2.0f) {
		strFactor = L"(3:2)";
	} else {
		strFactor = L"(4:3)";
	}

	if (m_core.IsRightPressed()) {
		if (m_hotButton < 3) {
			hotButtonDelta = 1;
		}
	}

	if (m_core.IsLeftPressed()) {
		if (m_hotButton > 0) {
			hotButtonDelta = -1;
		}
	}

	if (DoUp()) {
		if (m_hotButton > 1 && m_hotButton != g_backButtonId) {
			hotButtonDelta -= 2;
		} else if (m_hotButton == g_backButtonId) {
			hotButtonDelta = 3 - m_hotButton;
		}
	}
	if (DoDown()) {
		if (m_hotButton < 2 && m_hotButton != g_backButtonId) {
			hotButtonDelta += 2;
		} else {
			SetHotButton(g_backButtonId);
		}
	}


	if (m_resolutionIx > 0) {
		if (m_hotButton == g_noneId) {
			m_hotButton = 0;
		}
		if (DoMenuButton(250, 250, 140, 25, RESOLUTION_LOWER, 0)) {
			m_resolutionIx--;
		}
	} else {
		m_core.DoInactiveButton(250, 250, 140, 25, RESOLUTION_LOWER);
		if (m_hotButton == g_noneId) {
			m_hotButton = 1;
		}

		if (hotButtonDelta != 0 && m_hotButton + hotButtonDelta == 0) {
			hotButtonDelta = 0;
		}
	}

	if (m_resolutionIx < m_core.m_resolutions.size() - 1) {
		if (DoMenuButton(410, 250, 140, 25, RESOLUTION_HIGHER, 1)) {
			m_resolutionIx++;
		}
	} else {
		m_core.DoInactiveButton(410, 250, 140, 25, RESOLUTION_HIGHER);
		if (hotButtonDelta != 0 && m_hotButton + hotButtonDelta == 1) {
			if (hotButtonDelta > 0) {
				hotButtonDelta++;
			} else {
				SetHotButton(0);
				hotButtonDelta = 0;
			}
		}
	}

	cmn::WString text;
	text.Format(L"%i x %i %s", m_core.m_resolutions[m_resolutionIx].m_width, m_core.m_resolutions[m_resolutionIx].m_height, strFactor.c_str());
	DoHeadline(200, text);
	if (m_core.m_resolutions[m_resolutionIx].m_doFullscreen && !(m_resolutionIx == currentResolutionIx && m_core.m_isFullscreen)) {
		if (DoMenuButton(250, 300, 140, 25, MODE_FULLSCREEN, 2)) {
			m_core.m_width = m_core.m_resolutions[m_resolutionIx].m_width;
			m_core.m_height = m_core.m_resolutions[m_resolutionIx].m_height;
			m_core.m_isFullscreen = true;
			m_hotButton = g_noneId;
			return true;
		}
	} else {
		m_core.DoInactiveButton(250, 300, 140, 25, MODE_FULLSCREEN);
		if (hotButtonDelta != 0 && m_hotButton + hotButtonDelta == 2) {
			if (hotButtonDelta < 0) {
				hotButtonDelta--;
			} else {
				hotButtonDelta++;
			}
		}
	}

	if (m_core.m_resolutions[m_resolutionIx].m_doWindowed && !(m_resolutionIx == currentResolutionIx && m_core.m_isFullscreen != true)) {
		if (DoMenuButton(410, 300, 140, 25, MODE_WINDOWED, 3)) {
			m_core.m_width = m_core.m_resolutions[m_resolutionIx].m_width;
			m_core.m_height = m_core.m_resolutions[m_resolutionIx].m_height;
			m_core.m_isFullscreen = false;
			m_hotButton = g_noneId;
			return true;
		}
	} else {
		m_core.DoInactiveButton(410, 300, 140, 25, MODE_WINDOWED);
		if (hotButtonDelta != 0 && m_hotButton + hotButtonDelta == 3) {
			if (m_hotButton == g_backButtonId) {
				hotButtonDelta = 2 - g_backButtonId;
			} else {
				hotButtonDelta = 0;
			}
		}
	}

	if (hotButtonDelta != 0) {
		int newHotButton(m_hotButton + hotButtonDelta);
		if (m_hotButton + hotButtonDelta < 0) {
			newHotButton = 0;
		} else if (m_hotButton + hotButtonDelta > 3) {
			newHotButton = 3;		
		} 
		SetHotButton(newHotButton);
	}


	if (DoBackButton(350)) {
		m_options = O_Graphics;
		m_gfxOptions = GO_None;
		m_hotButton = GO_Display;
	}

	return false;
}

void Menu::ResetMenu() {
	m_hotButton = (int)MM_Difficulty;
	m_mainMenu = MM_None;
	m_options = O_None;
	m_gfxOptions = GO_None;
	m_inputOptions = IO_None;
	m_gamePadInput = GPI_None;
}

void Menu::DoHeadline(int a_y, const cmn::WString &a_text) {
	m_core.DoHeadline(0, a_y, g_width, 40, a_text, DT_CENTER | DT_TOP | DT_SINGLELINE);
}



bool Menu::DoUp() {
	static bool padState(false);
	bool padUp(false);
	dxrnd::InputDevice *pPad(m_core.m_assets.m_pGameCtrl);
	
	if (pPad != NULL) {
		if (padState == false) {
			padState = pPad->GetAxis(m_core.m_assets.m_padYAxis) < -500;
			if (padState == true) {
				padUp = true;
			}
		} else {
			padState = pPad->GetAxis(m_core.m_assets.m_padYAxis) < -200;
		}
	}

	return m_core.m_input.IsPressedVK(m_core.m_assets.m_kbdUp) || m_core.m_input.IsPressedVK(VK_UP) || padUp;
}

bool Menu::DoDown() {
	static bool padState(false);
	bool padDown(false);
	dxrnd::InputDevice *pPad(m_core.m_assets.m_pGameCtrl);

	if (pPad != NULL) {
		if (padState == false) {
			padState = pPad->GetAxis(m_core.m_assets.m_padYAxis) > 500;
			if (padState == true) {
				padDown = true;
			}
		} else {
			padState = pPad->GetAxis(m_core.m_assets.m_padYAxis) > 200;
		}
	}

	return m_core.m_input.IsPressedVK(m_core.m_assets.m_kbdDown) || m_core.m_input.IsPressedVK(VK_DOWN) || padDown;
}

const char* Menu::GetPadName() {
	return m_core.m_assets.m_pGameCtrl->m_deviceInfo.tszInstanceName;
}

const cmn::String& Menu::GetPadButtonName(int a_buttonIx) {
	return m_core.m_assets.m_pGameCtrl->m_buttonNames[a_buttonIx];
}

const cmn::String& Menu::GetPadAxisName(int a_axisIx) {
	return m_core.m_assets.m_pGameCtrl->m_axisNames[a_axisIx];
}

bool Menu::HasPad() {
	return m_core.m_assets.m_pGameCtrl != NULL;
}

int Menu::GetFirstPadButtonPressed() {

	for (int bIx = 0; bIx < m_core.m_assets.m_pGameCtrl->m_buttonCount; bIx++) {
		if (m_core.m_assets.m_pGameCtrl->GetButton(bIx) > 0) {
			return bIx;
		}
	}
	return -1;
}

int Menu::GetFirstPadAxisMovement() {
	for (int axIx = 0; axIx < m_core.m_assets.m_pGameCtrl->m_axisCount; axIx++) {
		if (abs(m_core.m_assets.m_pGameCtrl->GetAxis(axIx)) > 500) {
			return axIx;
		}
	}
	return -1;
}

bool Menu::DoMenuButton(int a_x, int a_y, int a_w, int a_h, const cmn::WString &a_text, int a_buttonId) {
	/*ViewCore::WidgetState ws;
	ws = m_core.DoHotMenuButton(a_x, a_y, a_w, a_h, a_text, m_hotButton == a_buttonId);
	if (ws >= ViewCore::WS_Hot) {
		SetHotButton(m_core, a_buttonId);
	}
	if (ws == ViewCore::WS_Clicked) {
		m_core.m_assets.m_sfx.mySounds[SoundBox::S_GuiClick].Play(m_core.m_assets.m_sfx.m_volume);
		return true;
	}*/
	return DoOptionsButton(a_x, a_y, a_w, a_h, a_text, a_buttonId, false);
}

bool Menu::DoOptionsButton(int a_x, int a_y, int a_w, int a_h, const cmn::WString &a_text, int a_buttonId, bool a_isSelected) {
	ViewCore::WidgetState ws;
	ws = DoHotMenuButton(a_x, a_y, a_w, a_h, a_text, m_hotButton == a_buttonId, a_isSelected);
	if (ws >= ViewCore::WS_Hot) {
		SetHotButton(a_buttonId);
	}
	if (ws == ViewCore::WS_Clicked) {
		m_core.m_assets.m_sfx.mySounds[SoundBox::S_GuiClick].Play(m_core.m_assets.m_sfx.m_volume);
		return true;
	}
	return false;
}

void Menu::SetHotButton(int a_buttonId) {
	if (m_hotButton != a_buttonId) {
		m_core.m_assets.m_sfx.mySounds[SoundBox::S_GuiHot].Play(m_core.m_assets.m_sfx.m_volume);
		m_hotButton = a_buttonId;
	}
}

bool Menu::DoAmountButton(int a_x, int a_y, int a_w, int a_h, cmn::WString a_text, float &a_amount, float a_min, float a_max, int a_buttonId) {
	ViewCore::WidgetState ws;
	ws = DoAmountButton(a_x, a_y, a_w, a_h, a_text, m_hotButton == a_buttonId, a_amount, a_min, a_max);
	if (ws >= ViewCore::WS_Hot) {
		SetHotButton(a_buttonId);
	}
	if (ws == ViewCore::WS_Clicked) {
		m_core.m_assets.m_sfx.mySounds[SoundBox::S_GuiClick].Play(m_core.m_assets.m_sfx.m_volume);
		return true;
	}

	return false;
}


ViewCore::WidgetState Menu::DoHotMenuButton(int a_x, int a_y, int a_w, int a_h, const cmn::WString &a_text, bool a_isSelected, bool a_isOptionSelected) {
	bool hot(false), clicked(false), superHot(false), superHotTimed(false), dblClicked(false);

	RECT rect;

	rect.left = a_x + 1;
	rect.right = a_x + a_w - 1;
	rect.top = a_y + 1;
	rect.bottom = a_y + a_h - 1;

	m_core.GetWidgetStatus(rect, hot, clicked, superHot, superHotTimed, dblClicked);

	//DrawUIRect(a_x + 1, a_y + 1, a_w - 2, a_h - 2, hot || a_isSelected, superHot);

	hot = hot && m_core.m_input.m_mouseDelta.Length2() > 0.0f;

	BarSprite sprite;

	// draw outline
	if (a_isSelected || clicked || superHot || a_isOptionSelected) {
		//sprite.AddBarTopBottomColors(math::Vector2f(rect.left* m_scale.x - 1, rect.top * m_scale.y - 1), math::Vector2f(rect.right * m_scale.x + 1, rect.bottom * m_scale.y + 1), math::Vector4c(246, 188, 15), math::Vector4c(255, 255, 255));
		const float t(fabsf(cosf(m_core.m_time * 2)));
		sprite.AddBarTopBottomColors(math::Vector2f(rect.left * m_core.m_scale.x - 1, rect.top * m_core.m_scale.y - 1), math::Vector2f(rect.right * m_core.m_scale.x + 1, rect.bottom * m_core.m_scale.y + 1), math::Vector4c(0, 0, 0), math::Vector4c(123 + 123 * t, 94 + 94 * t, 8 + 8 * t));		
	} else if (a_isOptionSelected) {
		sprite.AddBarTopBottomColors(math::Vector2f(rect.left * m_core.m_scale.x - 1, rect.top * m_core.m_scale.y - 1), math::Vector2f(rect.right * m_core.m_scale.x + 1, rect.bottom * m_core.m_scale.y + 1), math::Vector4c(0, 0, 0), math::Vector4c(79, 111, 157));
		//sprite.AddBarTopBottomColors(math::Vector2f(rect.left * m_scale.x, rect.top  * m_scale.y), math::Vector2f(rect.right * m_scale.x, rect.bottom * m_scale.y), math::Vector4c(79, 111, 157), math::Vector4c(26, 28, 30));
	} else {
		sprite.AddBarTopBottomColors(math::Vector2f(rect.left * m_core.m_scale.x - 1, rect.top * m_core.m_scale.y - 1), math::Vector2f(rect.right * m_core.m_scale.x + 1, rect.bottom * m_core.m_scale.y + 1), math::Vector4c(0, 0, 0), math::Vector4c(79, 111, 157));
	}

	// draw body
	if (a_isOptionSelected) {
		sprite.AddBarTopBottomColors(math::Vector2f(rect.left * m_core.m_scale.x, rect.top * m_core.m_scale.y), math::Vector2f(rect.right * m_core.m_scale.x, rect.bottom * m_core.m_scale.y), math::Vector4c(146, 88, 0), math::Vector4c(246, 188, 15));
		sprite.AddBarTopBottomColors(math::Vector2f(rect.left * m_core.m_scale.x + 1, rect.top  * m_core.m_scale.y + 1), math::Vector2f(rect.right * m_core.m_scale.x - 1, rect.bottom * m_core.m_scale.y - 1), math::Vector4c(79, 111, 157), math::Vector4c(26, 28, 30));
	} else if (a_isSelected || clicked || superHot) {
		const float t(fabsf(cosf(m_core.m_time * 2 + PI)));
		sprite.AddBarTopBottomColors(math::Vector2f(rect.left * m_core.m_scale.x, rect.top  * m_core.m_scale.y), math::Vector2f(rect.right * m_core.m_scale.x, rect.bottom * m_core.m_scale.y), math::Vector4c(59 + 20 * t, 91 + 20 * t, 137 + 20 * t), math::Vector4c(26, 28, 30));
	} else {
		sprite.AddBarTopBottomColors(math::Vector2f(rect.left * m_core.m_scale.x, rect.top  * m_core.m_scale.y), math::Vector2f(rect.right * m_core.m_scale.x, rect.bottom * m_core.m_scale.y), math::Vector4c(59, 91, 137), math::Vector4c(6, 8, 10));
	}
		
	sprite.Render(m_core.m_assets.m_barEffect);

	if (superHot) {
		rect.left += 1;
		rect.right += 1;
		rect.top += 1;
		rect.bottom += 1;
	}

	m_core.DoLabel(a_text, rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	if (a_isSelected && (m_core.m_input.IsPressedVK(m_core.m_assets.m_kbdFire) || m_core.m_input.IsPressedVK(VK_RETURN) || IsAnyGamePadButtonPressed())) {
		clicked = true;
	}

	return m_core.BoolsToWidgetState(hot, clicked, superHot, superHotTimed, dblClicked);
}


ViewCore::WidgetState Menu::DoMenuButton(int a_x, int a_y, int a_w, int a_h, cmn::String a_text, bool a_isSelected) {
	bool hot(false), clicked(false), superHot(false), superHotTimed(false), dblClicked(false);

	RECT rect;

	rect.left = a_x + 1;
	rect.right = a_x + a_w - 1;
	rect.top = a_y + 1;
	rect.bottom = a_y + a_h - 1;

	m_core.GetWidgetStatus(rect, hot, clicked, superHot, superHotTimed, dblClicked);

	//DrawUIRect(a_x, a_y, a_w, a_h, hot || a_isSelected, superHot);

	BarSprite sprite;
	if (a_isSelected || clicked || hot || superHot) {
		sprite.AddBarTopBottomColors(math::Vector2f(rect.left* m_core.m_scale.x - 1, rect.top * m_core.m_scale.y - 1), math::Vector2f(rect.right * m_core.m_scale.x + 1, rect.bottom * m_core.m_scale.y + 1), math::Vector4c(0, 0, 0), math::Vector4c(246, 188, 15));
		sprite.AddBarTopBottomColors(math::Vector2f(rect.left * m_core.m_scale.x, rect.top  * m_core.m_scale.y), math::Vector2f(rect.right * m_core.m_scale.x, rect.bottom * m_core.m_scale.y), math::Vector4c(79, 111, 157), math::Vector4c(26, 28, 30));
	} else {
		sprite.AddBarTopBottomColors(math::Vector2f(rect.left* m_core.m_scale.x - 1, rect.top * m_core.m_scale.y - 1), math::Vector2f(rect.right * m_core.m_scale.x + 1, rect.bottom * m_core.m_scale.y + 1), math::Vector4c(0, 0, 0), math::Vector4c(79, 111, 157));
		sprite.AddBarTopBottomColors(math::Vector2f(rect.left * m_core.m_scale.x, rect.top  * m_core.m_scale.y), math::Vector2f(rect.right * m_core.m_scale.x, rect.bottom * m_core.m_scale.y), math::Vector4c(59, 91, 137), math::Vector4c(6, 8, 10));
	}
	sprite.Render(m_core.m_assets.m_barEffect);

	if (superHot) {
		rect.left += 1;
		rect.right += 1;
		rect.top += 1;
		rect.bottom += 1;
	}

	m_core.DoLabel(a_text, rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	if (a_isSelected && (m_core.m_input.IsClicked(m_core.m_assets.m_kbdFire) || m_core.m_input.IsClicked(VK_RETURN) || IsAnyGamePadButtonPressed())) {
		clicked = true;
	}

	return m_core.BoolsToWidgetState(hot, clicked, superHot, superHotTimed, dblClicked);
}

ViewCore::WidgetState Menu::DoAmountButton(int a_x, int a_y, int a_w, int a_h, const cmn::WString &a_text, bool a_isSelected, float &a_amount, float a_min, float a_max) {
	bool hot(false), clicked(false), superHot(false), superHotTimed(false), dblClicked(false), changed(false);

	RECT rect;

	rect.left = a_x + 1;
	rect.right = a_x + a_w - 1;
	rect.top = a_y + 1;
	rect.bottom = a_y + a_h - 1;

	m_core.GetWidgetStatus(rect, hot, clicked, superHot, superHotTimed, dblClicked);
	hot = hot && m_core.m_input.m_mouseDelta.Length2() > 0.0f;

	//DrawUIRect(a_x, a_y, a_w, a_h, hot, superHot);

	BarSprite sprite;
	if (a_isSelected || clicked) {
		const float t(fabsf(cosf(m_core.m_time * 2)));
		sprite.AddBarTopBottomColors(math::Vector2f(rect.left* m_core.m_scale.x - 1, rect.top * m_core.m_scale.y - 1), math::Vector2f(rect.right * m_core.m_scale.x + 1, rect.bottom * m_core.m_scale.y + 1), math::Vector4c(0, 0, 0), math::Vector4c(123 + 123 * t, 94 + 94 * t, 8 + 8 * t));
	} else {
		sprite.AddBarTopBottomColors(math::Vector2f(rect.left* m_core.m_scale.x - 1, rect.top * m_core.m_scale.y - 1), math::Vector2f(rect.right * m_core.m_scale.x + 1, rect.bottom * m_core.m_scale.y + 1), math::Vector4c(0, 0, 0), math::Vector4c(79, 111, 157));
	}
	sprite.AddBarTopBottomColors(math::Vector2f(rect.left * m_core.m_scale.x, rect.top  * m_core.m_scale.y), math::Vector2f(rect.right * m_core.m_scale.x, rect.bottom * m_core.m_scale.y), math::Vector4c(29, 61, 107), math::Vector4c(0, 0, 0));
	
	if (a_isSelected) {
		if (a_amount > a_min && m_core.IsLeftPressed()) {
			changed = true;
			a_amount -= (a_max - a_min) / 100.0f;
			if (a_amount < a_min) {
				a_amount = a_min;
			}
		}

		if (a_amount < a_max && m_core.IsRightPressed()) {
			changed = true;
			a_amount += (a_max - a_min) / 100.0f;
			if (a_amount > a_max) {
				a_amount = a_max;
			}
		}
	}


	/*if (superHot) {
		rect.left += 1;
		rect.right += 1;
		rect.top += 1;
		rect.bottom += 1;
	}*/

	math::Vector2f mpos;
	m_core.m_input.GetCursorPosWin(mpos);
	POINT mousePoint;
	mousePoint.x = (int)mpos.x * (1.0f / m_core.m_scale.x);
	mousePoint.y = (int)mpos.y * (1.0f / m_core.m_scale.y);

	const float percent(superHot ? (mousePoint.x - (float)a_x) / (float)(a_w - 2) : (a_amount - a_min) / (a_max - a_min));


	{
		math::Vector2f topLeft;
		math::Vector2f bottomRight;
		const float width((rect.right - rect.left) * m_core.m_scale.x - 2.0f);

		topLeft.x = rect.left * m_core.m_scale.x + 1.0f;
		bottomRight.x = topLeft.x + (width * percent);
		topLeft.y = rect.top * m_core.m_scale.y + 1.0f;
		bottomRight.y = rect.bottom * m_core.m_scale.y - 1.0f;

		if (clicked || superHot || a_isSelected) {
			sprite.AddBarTopBottomColors(topLeft, bottomRight, math::Vector4c(109, 141, 187), math::Vector4c(56, 58, 60));
		} else {
			sprite.AddBarTopBottomColors(topLeft, bottomRight, math::Vector4c(79, 111, 157), math::Vector4c(26, 28, 30));
		}

		sprite.Render(m_core.m_assets.m_barEffect);
	}

	cmn::WString sensitivity;
	sensitivity.Format(a_text.c_str(), percent * (a_max - a_min) + a_min);

	m_core.DoLabel(sensitivity, rect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

	if (dblClicked) {
		return ViewCore::WS_DblClicked;
	}
	if (changed) {
		return ViewCore::WS_Clicked;
	}
	if (clicked) {
		a_amount = ((mousePoint.x - (float)a_x) / (float)(a_w - 2)) * (a_max - a_min) + a_min;
		return ViewCore::WS_Clicked;
	}
	return m_core.BoolsToWidgetState(hot, clicked, superHot, superHotTimed, dblClicked);
}

bool Menu::IsAnyGamePadButtonPressed() const {
	static int state(-1);	// we need to hold what key was pressed

	if (m_core.m_assets.m_pGameCtrl != NULL) {
		if (state < 0) {
			for (int bIx = 0; bIx < m_core.m_assets.m_pGameCtrl->m_buttonCount; bIx++) {
				if (m_core.m_assets.m_pGameCtrl->GetButton(bIx) > 0) {
					state = bIx;
					return true;
				}
			}
		} else {
			if (m_core.m_assets.m_pGameCtrl->GetButton(state) == 0) {
				state = -1;
			}
		}

		return false;
	}
	state = -1;

	return false;

}


}
