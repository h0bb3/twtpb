// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "IEventTarget.h"

#include "Model.h"
#include "ViewCore.h"

namespace view {
	class View {
		public:


			View(const model::Model &a_model, ViewCore &a_core);
			virtual ~View(void);

			enum ViewStatus {
				VS_Ok,
				VS_Reset,
				VS_Quit,
				VS_Done,
			};

			virtual ViewStatus Render(float a_dt, IEventTarget &a_target) = 0;

		protected:
			ViewCore &m_core;
			const model::Model &m_model;
	};
}
