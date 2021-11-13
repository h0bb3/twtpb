// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "View.h"

namespace view {
class LevelView : public View {
	public:
		LevelView(const model::Model &a_model, ViewCore &a_core);
		virtual ~LevelView(void);

		virtual ViewStatus Render(float a_dt, IEventTarget &a_target);

	private:
		int GetSelectedIx(const model::Wave* a_pWave);

		int m_workingWaveButtonIx;	// to know what wave button just have been switched
		int m_levelIx;
		typedef std::vector<const model::Wave*> WavePtrVector;
		WavePtrVector m_selectedWaves;

		float m_time;
};

}
