// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "View.h"

namespace view {

class Splash : public View {
	public:
		Splash(const model::Model &a_model, ViewCore &a_core);
		~Splash(void);

		virtual ViewStatus Render(float a_dt, IEventTarget &a_target);
	private:
		float m_time;
};

}
