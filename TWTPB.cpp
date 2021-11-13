// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include ".\twtpb.h"

#include <Time.h>


RealTimeApp* RealTimeApp::CreateInstance(HINSTANCE	hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	char date[32], strtime[32];
	cmn::Log::Init(17, "TWTPB.log");
	cmn::Log::GetInstance()->Clear();
	_strdate(date);
	_strtime(strtime);
	LOG_3_A2("TWTPB log: %s - %s", date, strtime);
//	LOG_3_A1("%s", PUBLIC_VERSION);
	LOG_3_A1("cmdline: %s", lpCmdLine);

	//lpCmdLine = "sopuserid=275 sopusername=hObbE soppassword=bamse-zorba";

	cmn::String cmdLine(lpCmdLine);
	cmn::String uid("0");
	size_t uidStart = cmdLine.find_first_of("sopuserid=");
	if (uidStart != cmn::String::npos) {
		uidStart += 10;
		size_t uidEnd = cmdLine.find_first_of(" ", uidStart);
		if (uidEnd != cmn::String::npos) {
			uid = cmdLine.substr(uidStart, uidEnd - uidStart);
		}
	}
	LOG_3_A1("uid: %s", uid.c_str());

	// create random offset to twart trainers and cheaters...
	srand(time(NULL));
	char *buffer = new char[rand() % 1024];	// this will create a small memory leak but it's only called once so...

	return new TWTPB(uid, hInstance);
}


TWTPB::TWTPB(const cmn::String &a_uid, HINSTANCE a_appInstance) : m_model(m_view, a_uid), m_view(m_model, a_appInstance) {
}

TWTPB::~TWTPB(void) {
}


cmn::String TWTPB::Initiate() {
	if (m_model.Initiate() != true) {
		return "Failed to initiate model";
	}
	return m_view.Initiate(m_model);
}

cmn::String TWTPB::Terminate() {
	return m_view.Terminate();
}

bool TWTPB::Update(float a_dt) {
	if (a_dt < 1.0f && a_dt > 0.0f) {
		if (m_model.Update(a_dt) != true) {
			return false;
		}
		return m_view.Render(a_dt, m_model);
	}

	return true;
}