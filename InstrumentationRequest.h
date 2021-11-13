// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once
#include "HttpRequest.h"

namespace model {

class InstrumentationRequest {
	public:
		InstrumentationRequest(const cmn::String &a_uid);
		~InstrumentationRequest(void);

		void StartGame();
		void StartGameAtLevel(int a_level, float a_difficulty);
		void DeathAtLevel(int a_level, float a_levelTime);
		void EndGame(int a_level, float a_secondsPlayed);
		void GraphicsCard(const char *a_gpu);

		void Update();
		void Purge();
	private:
		void Send(const int a_type, const int a_valueInt = 0, const float a_valueFloat = 0.0f, const cmn::String &a_valueStr = "");

		const int m_uid;
		bool StartNextRequest();

		typedef std::list<cmn::String> StringList;
		StringList m_requests;
		HttpRequest m_request;

/*	$type = intval($_GET['t']);
	$uid = intval($_GET['u']);
	$version = intval($_GET['v']);
	$valueStr = $_GET['vs'];
	$valueInt = intval($_GET['vi']);
	$valueFloat = floatval($_GET['vf']);*/
};
}
