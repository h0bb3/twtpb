// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

namespace model {
class Bonus {
	public:
		Bonus(void);
		~Bonus(void);

		enum BonusType {
			BT_Score = 0,
			BT_Shield,
			BT_Weapon,
			BT_Count
		};

		BonusType m_type;
		math::Vector2f m_pos;
		bool m_isAlive;
};
}
