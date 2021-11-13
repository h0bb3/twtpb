// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include ".\formationview.h"

#include <DXRenderer/DX.h>


namespace view {

FormationView::FormationView(const model::Model &a_model, ViewCore &a_core) : View(a_model, a_core) {
	m_formationIx = 4;
	m_copyFormationIx = 0;
	m_time = 1.0f;
	m_enemyIx = 0;
	m_controllerIx = 1;
}

FormationView::~FormationView(void) {
}

View::ViewStatus FormationView::Render(float a_dt, IEventTarget &a_target) {
	model::FormationController f;


	EnemySprite sprites;

	m_core.SetMouse(true);


	if (m_core.DoSpinnerButton(0, 0, 64, 16, m_formationIx)) {
		m_enemyIx = 0;
		if (m_formationIx < 0) {
			m_formationIx = 0;
		} else if (m_formationIx >= model::Model::g_maxFormations) {
			m_formationIx = model::Model::g_maxFormations - 1;
		}
	}

	if (m_core.DoButton(g_width - 200, 0, 100, 16, "Copy", false) == ViewCore::WS_Clicked) {
		m_copyFormationIx = m_formationIx;
	}

	cmn::String paste;
	paste.Format("Paste (%i)", m_copyFormationIx);
	if (m_core.DoButton(g_width - 100, 0, 100, 16, paste, false) == ViewCore::WS_Clicked) {
		if (m_copyFormationIx != m_formationIx) {
			a_target.CopyFormation(m_copyFormationIx, m_formationIx);
		}
	}

	const int enemyCount((int)m_model.m_formations[m_formationIx].m_enemies.size());

	math::Vector2f pos(0.0f, 32.0f);
	for (int etIx = 0; etIx < model::Enemy::T_Count; etIx++) {
		if (DoEnemyButton(sprites, pos, (model::Enemy::Type)etIx, false)) {
			a_target.AddEnemyToFormation(m_formationIx, etIx, 0.0f);
			m_enemyIx = m_model.m_formations[m_formationIx].m_enemies.size() - 1;
		}
	}

	pos.Set(0.0f, g_height - 82.0f);
	for (int eIx = 0; eIx < enemyCount; eIx++) {
		if (DoEnemyButton(sprites, pos, m_model.m_formations[m_formationIx].m_enemies[eIx].m_enemyType, eIx == m_enemyIx)) {
			m_enemyIx = eIx;
		}
	}

	if (m_core.m_input.IsPressedVKRaw(VK_LEFT)) {
		m_time -= a_dt;
	} else if (m_core.m_input.IsPressedVKRaw(VK_RIGHT)) {
		m_time += a_dt;
	}

	if (enemyCount > 0) {
		model::Enemy *enemies = new model::Enemy[enemyCount];
		float queueOffset(m_model.m_formations[m_formationIx].m_enemies[m_enemyIx].m_timeOffset);
		float scale(m_model.m_formations[m_formationIx].m_enemies[m_enemyIx].m_scale);
		math::Vector2f offset(m_model.m_formations[m_formationIx].m_enemies[m_enemyIx].m_offset);
		bool hasShield(m_model.m_formations[m_formationIx].m_enemies[m_enemyIx].m_hasShield);

		// per enemy stuff
		if (m_core.DoButton(g_width - 128, g_height - 80, 128, 16, "Shield", hasShield) == view::ViewCore::WS_Clicked) {
			hasShield = !hasShield;
			a_target.UpdateEnemyInFormation(m_formationIx, m_enemyIx, m_model.m_formations[m_formationIx].m_enemies[m_enemyIx].m_enemyType, m_model.m_formations[m_formationIx].m_enemies[m_enemyIx].m_timeOffset, offset, scale, hasShield);
		}

		m_core.DoLabel(g_width - 256, g_height - 48, 128, 16, "Scale:", DT_RIGHT | DT_VCENTER);
		m_core.DoLabel(g_width - 256, g_height - 64, 128, 16, "Queue Offset:", DT_RIGHT | DT_VCENTER);
		m_core.DoLabel(g_width - 256, g_height - 32, 128, 16, "Horizontal Offset:", DT_RIGHT | DT_VCENTER);
		m_core.DoLabel(g_width - 256, g_height - 16, 128, 16, "Vertical Offset:", DT_RIGHT | DT_VCENTER);
		if (m_core.DoSpinnerButton(g_width - 128, g_height - 48, 128, 16, scale, 0.1f) ||
			m_core.DoSpinnerButton(g_width - 128, g_height - 64, 128, 16, queueOffset, 0.001f) ||
			m_core.DoSpinnerButton(g_width - 128, g_height - 32, 128, 16, offset.y, 1.0f) ||
			m_core.DoSpinnerButton(g_width - 128, g_height - 16, 128, 16, offset.x, 1.0f)) {
			a_target.UpdateEnemyInFormation(m_formationIx, m_enemyIx, m_model.m_formations[m_formationIx].m_enemies[m_enemyIx].m_enemyType, queueOffset, offset, scale, hasShield);
		}

		if (m_core.m_input.IsPressedVK(VK_UP)) {
			a_target.UpdateEnemyInFormation(m_formationIx, m_enemyIx, m_model.m_formations[m_formationIx].m_enemies[m_enemyIx].m_enemyType, m_model.m_formations[m_formationIx].m_enemies[m_enemyIx].m_timeOffset - 0.001f, offset, scale, hasShield);
		} else if (m_core.m_input.IsPressedVK(VK_DOWN)) {
			a_target.UpdateEnemyInFormation(m_formationIx, m_enemyIx, m_model.m_formations[m_formationIx].m_enemies[m_enemyIx].m_enemyType, m_model.m_formations[m_formationIx].m_enemies[m_enemyIx].m_timeOffset + 0.001f, offset, scale, hasShield);
		}


		for (int eIx = 0; eIx < enemyCount; eIx++) {
			m_model.m_formations[m_formationIx].m_enemies[eIx].SetEnemyAlive(enemies[eIx], 0, eIx);
			f.AddEnemy(&enemies[eIx]);
		}

		if (m_enemyIx >= 0) {
			cmn::String str;
			str.Format("x: %f, y: %f, t: %f", enemies[m_enemyIx].m_pos.x, enemies[m_enemyIx].m_pos.y, enemies[m_enemyIx].m_t);
			m_core.DoLabel(0, 200 - 16, 300, 16, str, 0);
		}


		f.m_currentFormation = (model::FormationController::Formation)m_controllerIx;
		f.Update(m_time - a_dt, 1.0f);
		if (m_enemyIx >= 0) {
			cmn::String str;
			str.Format("x: %f, y: %f, t: %f", enemies[m_enemyIx].m_pos.x, enemies[m_enemyIx].m_pos.y, enemies[m_enemyIx].m_t);
			m_core.DoLabel(0, 200, 300, 16, str, 0);
		}
		f.Update(a_dt, 1.0f);	// this is to make the angles right...
		//f.Update(m_time);

		if (m_enemyIx >= 0) {
			cmn::String str;
			str.Format("x: %f, y: %f, t: %f", enemies[m_enemyIx].m_pos.x, enemies[m_enemyIx].m_pos.y, enemies[m_enemyIx].m_t);
			m_core.DoLabel(0, 216, 300, 16, str, 0);
		}

		for (int eIx = 0; eIx < enemyCount; eIx++) {
			m_core.DrawEnemy(sprites, enemies[eIx], 1.0f, 0, 1.0f);
		}

		if (m_core.m_input.IsPressedVK(VK_DELETE)) {
			a_target.DeleteEnemyFromFormation(m_formationIx, m_enemyIx);
			m_enemyIx--;
			if (m_enemyIx < 0) {
				m_enemyIx = 0;
			}
		}

		delete [] enemies;
	}

	m_core.RenderEnemySprites(sprites);

	m_core.m_input.IsClicked(m_core.m_assets.m_mouseFire);

	return VS_Ok;
}

bool FormationView::DoEnemyButton(EnemySprite &a_sprite, math::Vector2f &a_pos, model::Enemy::Type a_type, bool a_isSelected) {
	model::Enemy e;
	math::Vector2f size(m_core.GetEnemySpriteSize(a_type));

	const bool ret(m_core.DoButton((int)(a_pos.x), (int)(a_pos.y), size.x, size.y, "", a_isSelected) == ViewCore::WS_Clicked);

	e.SetAlive(a_type, 1.0f, 1.0f, -1, -1, false);
	e.m_pos = a_pos + size * 0.5f;
	e.m_angle = 0.0f;

	m_core.DrawEnemy(a_sprite, e, 1.0f, 0, 1.0f);
	a_pos.x += size.x + 5.0f;

	return ret;
}



}