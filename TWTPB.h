// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include <RealtimeApp/RealTimeApp.h>

#include "ViewManager.h"
#include "Model.h"

class TWTPB : public RealTimeApp {
	public:
		TWTPB(const cmn::String &a_uid, HINSTANCE a_appInstance);
		virtual ~TWTPB(void);

		virtual bool Update(float a_dt);
		virtual cmn::String Initiate();
		virtual cmn::String Terminate();
	private:
		view::ViewManager m_view;
		model::Model m_model;
};
