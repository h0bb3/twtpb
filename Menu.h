// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "model.h"
#include "MenuView.h"
#include "View.h"

namespace view {
class Menu {
	public:
		Menu(ViewCore &a_core);
		~Menu(void);

		enum MenuStatus {
				MS_Ok,
				MS_Reset,
				MS_Quit,
				MS_QuitGame,
				MS_StartGame,
				MS_ContinueGame,
				MS_Done,
			};

		enum MainMenu {
			MM_None = -1,
			MM_Start,
			MM_Difficulty,
			MM_Highscores,
			MM_Options,
			MM_Credits,
			MM_Buy,
			MM_Quit,
			MM_Count
		};

		enum Options {
			O_None = -1,
			O_Graphics,
			O_Sound,
			O_Input,
			O_Count
		};

		enum GraphicOptions {
			GO_None = -1,
			GO_LevelOfDetail,
			GO_Display,
			GO_Count
		};

		enum DisplayOptions {
			DO_None = -1,
			DO_Resolution,
			DO_Mode,
			DO_Count
		};

		enum InputOptions {
			IO_None = -1,
			IO_Mouse,
			IO_Keyboard,
			IO_GamePad,
			IO_Count
		};

		enum GamePadInput {
			GPI_None = -1,
			GPI_MoveLeftRight,
			GPI_MoveUpDown,
			GPI_Fire,
			GPI_Ability,
			GPI_SetRapidFire,
			GPI_SetRegeneration,
			GPI_SetTimeWarp,
			GPI_SetMissiles,
			GPI_Count
		};

		enum KeyboardInput {
			KI_None = -1,
			KI_MoveLeft,
			KI_MoveRight,
			KI_MoveUp,
			KI_MoveDown,
			KI_Next,
			KI_Prev,
			KI_Fire,
			KI_Ability,
			KI_SetRapidFire,
			KI_SetRegeneration,
			KI_SetTimeWarp,
			KI_SetMissiles,
			KI_Count
		};

		MenuStatus DoMenu(const float a_dt, const model::Model &a_model);

		void DrawCredits(const float a_yPos, const float a_time);
		void DoCredits(const float a_dt);

		bool DoBuyButton(int a_y);

		void DoKeyboardSettings();
		void DoGamePadSettings();
		void DoMouseSettings();
		void DoSoundOptions();
		bool DoGfxOptions();
		bool DoResolutions();
		void DoLevelOfDetail();
		

		MainMenu m_mainMenu;			// first level menus
		Options m_options;				// currently expanded options
		GraphicOptions m_gfxOptions;	// currently selected graphics options
		InputOptions m_inputOptions;
		GamePadInput m_gamePadInput;
		KeyboardInput m_kbdInput;
		int m_hotButton;

		int m_resolutionIx;				// current resolution index
		int m_startScore;				// score to start on

		float m_creditsYScroll;
		float m_difficulty;
		int m_level;

		void ResetMenu();

		cmn::String VKToString(int a_vk);
		ViewCore::WidgetState DoHotMenuButton(int a_x, int a_y, int a_w, int a_h, const cmn::WString &a_text, bool a_isSelected, bool a_isOptionSelected = false);

	private:

		static const int g_noneId = -1;
		static const int g_backButtonId = -2;
		static const int g_buyButtonId = (int)MM_Buy;

		ViewCore &m_core;


		
		ViewCore::WidgetState DoMenuButton(int a_x, int a_y, int a_w, int a_h, cmn::String a_text, bool a_isSelected);
		


		float DrawCreditsSection(float a_ypos, const cmn::String &a_headline, const cmn::String &a_body);
		bool DoBackButton(int a_y);
		void DoHeadline(int a_y, const cmn::WString &a_text);


		bool DoUp();
		bool DoDown();

		// gamepad wrapper functions
		bool HasPad();
		const char* GetPadName();
		const cmn::String& GetPadButtonName(int a_buttonIx);
		const cmn::String& GetPadAxisName(int a_axisIx);
		int GetFirstPadButtonPressed();
		int GetFirstPadAxisMovement();
		bool IsAnyGamePadButtonPressed() const;

		void SetHotButton(int a_buttonId);
		bool DoMenuButton(int a_x, int a_y, int a_w, int a_h, const cmn::WString &a_text, int a_buttonId);
		bool DoOptionsButton(int a_x, int a_y, int a_w, int a_h, const cmn::WString &a_text, int a_buttonId, bool a_isSelected);
		bool DoAmountButton(int a_x, int a_y, int a_w, int a_h, cmn::WString a_text, float &a_amount, float a_min, float a_max, int a_buttonId);
		ViewCore::WidgetState DoAmountButton(int a_x, int a_y, int a_w, int a_h, const cmn::WString &a_text, bool a_isSelected, float &a_amount, float a_min, float a_max);
};
}