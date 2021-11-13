// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include <math/AABoundingBox.h>

namespace model {

class Bullet {
	public:
		Bullet(void);
		~Bullet(void);

		
		math::Vector2f m_speed;


		void Update(float a_dt);
		bool IsAlive() const;
		void Spawn(const math::Vector2f &a_pos);
		void Kill();
		const math::Vector2f &GetPos() const;
		const float GetAge() const;
		

		void GetBox(math::AABoundingBox2D &a_box);
	private:
		
		math::Vector2f m_pos;
		float m_age;	// >= 0 is alive


};

}
