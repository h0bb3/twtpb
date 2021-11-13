// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include "View.h"
#include "model.h"
#include "ViewCore.h"

#include <Scene/Sprite.h>

namespace view {

	class FormationView : public View {
	public:
		FormationView(const model::Model &a_model, ViewCore &a_core);
		~FormationView(void);

		ViewStatus Render(float a_dt, IEventTarget &a_target);


	private:

		bool DoEnemyButton(EnemySprite &a_sprite, math::Vector2f &a_pos, model::Enemy::Type a_type, bool a_isSelected);



		int m_enemyIx;
		int m_formationIx;
		int m_controllerIx;
		int m_copyFormationIx;
		float m_time;

};

}