// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "IBoss.h"

namespace model {

	class BossBase : public IBoss {
	public:
		BossBase(float m_maxTime);
		virtual ~BossBase(void);

		float GetTimeLeft();
		void Initiate();
		void Update(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty);

	protected:

		virtual void DoInitiate() = 0;
		virtual void DoUpdate(float a_dt, const math::Vector2f &a_playerPos, const bool m_isAgressive, const float a_difficulty) = 0;

	private:
		const float m_timeToLive;
		float m_time;
};
}