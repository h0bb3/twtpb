// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include <math/AABoundingBox.h>
namespace model {

class Beam {
	public:
		Beam(void);
		virtual ~Beam(void);

		bool IsAlive() const;
		bool IsLethal() const;
		void SetAlive(float a_timeToLive, float a_width);
		float GetNormalizedPowerupTimer() const;
		void Kill();

		void Update(float a_dt);

		const static float g_powerUpTime;

		float m_angle;
		float m_width;

	private:
		float m_timer;
		float m_time;
};

}
