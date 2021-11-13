// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "InstrumentationRequest.h"
#include "HttpRequest.h"
#include "IEventTarget.h"

#include <Common/StlWipe.h>

namespace model {

InstrumentationRequest::InstrumentationRequest(const cmn::String &a_uid) : m_uid(a_uid.ToInt()) {
}

InstrumentationRequest::~InstrumentationRequest(void) {
}

void InstrumentationRequest::StartGame() {
	Send(1);
}

void InstrumentationRequest::StartGameAtLevel(int a_level, float a_difficulty) {
	Send(4, a_level, a_difficulty);
}

void InstrumentationRequest::DeathAtLevel(int a_level, float a_levelTime) {
	Send(3, a_level, a_levelTime);
}

void InstrumentationRequest::EndGame(int a_level, float a_secondsPlayed) {
	Send(2, a_level, a_secondsPlayed);
}

void InstrumentationRequest::GraphicsCard(const char *a_gpu) {
	Send(5, 0, 0, a_gpu);
}

void InstrumentationRequest::Send(const int a_type, const int a_valueInt, const float a_valueFloat, const cmn::String &a_valueStr) {
#ifndef PUBLIC_RELEASE
	return;
#endif

#ifdef NO_INSTRUMENTATION
	return;
#endif

	if (m_requests.size() < 128) {
		cmn::String request;
		//const cmn::String url("http://194.47.173.242/instrumentation/twtpb");
		const cmn::String url("http://194.47.173.242/game_scripts/twtpb/instrumentation.php");

		if (a_valueStr.length() > 0) {
			cmn::String encodedString(a_valueStr);
			encodedString.Replace(" ", "%20");	// possibly more encoding needed...
			request.Format("%s?t=%i&u=%i&v=%i&vs=%s&vi=%i&vf=%f", url.c_str(), a_type, m_uid, g_version, encodedString.c_str(), a_valueInt, a_valueFloat);
		} else {
			request.Format("%s?t=%i&u=%i&v=%i&vi=%i&vf=%f", url.c_str(), a_type, m_uid, g_version, a_valueInt, a_valueFloat);
		}
		m_requests.push_back(request);
	}
}

void InstrumentationRequest::Update() {
	if (m_request.update() != true) {
		StartNextRequest();
	} else {
		const int state(m_request.getState());
		if (state == HttpRequest::OFFLINE || state == HttpRequest::IDLE) {
			StartNextRequest();
		}
	}
}

void InstrumentationRequest::Purge() {
	Sleep(100);
	if (m_request.update() != true) {
		return;
	} else {
		const int state(m_request.getState());
		if (state == HttpRequest::OFFLINE) {
			return;
		} else if (state == HttpRequest::IDLE) {
			if (StartNextRequest()) {
				Purge();
			}
		} else {
			Purge();
		}
	}
}

bool InstrumentationRequest::StartNextRequest() {
	if (m_requests.size() > 0) {
		delete [] m_request.myBuffer;
		m_request.myBuffer = NULL;
		cmn::String request(*m_requests.begin());
		m_requests.pop_front();
		if (m_request.request(HttpRequest::GET, request.c_str()) != true) {
			StartNextRequest();
		} else {
			return true;
		}
	}

	return false;
}

}