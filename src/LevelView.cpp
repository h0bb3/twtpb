// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#include "StdAfx.h"
#include "LevelView.h"
#include "Wave.h"

#include <DXRenderer/DX.h>

namespace view {

LevelView::LevelView(const model::Model &a_model, ViewCore &a_core) : View(a_model, a_core) {
	m_levelIx = 0;
	m_time = 0;
	m_workingWaveButtonIx = -1;
}

LevelView::~LevelView(void) {
}

bool WaveSorter(const model::Wave *a_pW1, const model::Wave *a_pW2) {
	return a_pW1->m_triggerTime < a_pW2->m_triggerTime;
}

View::ViewStatus LevelView::Render(float a_dt, IEventTarget &a_target) {
	m_core.SetMouse(true);
	float multiplier(1.0f);

	if (m_core.m_input.IsPressedVKRaw(VK_LSHIFT)) {
		multiplier = 2.0f;
	} else if (m_core.m_input.IsPressedVKRaw(VK_LCONTROL)) {
		multiplier = 0.1f;
	}

	if (m_core.m_input.IsPressedVKRaw(VK_LEFT)) {
		m_time -= a_dt * multiplier;
	} else if (m_core.m_input.IsPressedVKRaw(VK_RIGHT)) {
		m_time += a_dt * multiplier;
	}

	cmn::String text;
	text.Format("time: %f, multiplier: %f", m_time, multiplier);
	m_core.DoLabel(256, 0, 128, 16, text, 0);


	m_core.DoLabel(0, 0, 64, 16, "Level:", DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	if (m_core.DoSpinnerButton(64, 0, 64, 16, m_levelIx)) {
		if (m_levelIx < 0) {
			m_levelIx = 0;
		} else if (m_levelIx > 15) {
			m_levelIx = 15;
		} else {
			m_selectedWaves.clear();
		}
	}

	// show the waves for this level in time order
	std::vector<const model::Wave*> sortedwaves;
	for (int i = 0; i < model::Model::g_maxWaves; i++) {
		if (m_model.m_waves[i].m_levelIx == m_levelIx) {
			sortedwaves.push_back(&(m_model.m_waves[i]));
		}
	}
	std::sort(sortedwaves.begin(), sortedwaves.end(), WaveSorter);

	int y(32);
	int x(0);
	bool resetWorkingWaveButtonIx(true);
	
	text.Format("%i", m_workingWaveButtonIx);
	m_core.DoLabel(512, 0, 64, 16, text, 0);

	for (size_t wIx = 0; wIx < sortedwaves.size(); wIx++) {
		cmn::String text;

		int selectedIx(GetSelectedIx(sortedwaves[wIx]));

		if (sortedwaves[wIx]->m_dropWeapon > 0) {
			text.Format("wave [dw] %i", (int)sortedwaves[wIx]->m_triggerTime);
		} else {
			text.Format("wave %i", (int)sortedwaves[wIx]->m_triggerTime);
		}
		ViewCore::WidgetState state(m_core.DoButton(x, y, 64, 16, text, selectedIx >= 0));

		if (state > ViewCore::WS_Hot) {
			resetWorkingWaveButtonIx = false;
		}

		if (state == ViewCore::WS_DblClicked) {
			m_selectedWaves.clear();
			m_selectedWaves.push_back(sortedwaves[wIx]);
		} else if (state == ViewCore::WS_SuperHot) {
			// this is to avoid imgui related problems turning the same wave on and off all the time
			if (m_workingWaveButtonIx != wIx) {
				m_workingWaveButtonIx = wIx;
				if (selectedIx >= 0) {
					m_selectedWaves.erase(m_selectedWaves.begin() + selectedIx);
					
				} else {
					m_selectedWaves.push_back(sortedwaves[wIx]);
				}
				
			}
		}
		if (wIx < sortedwaves.size() - 1) {
			if ((int)sortedwaves[wIx]->m_triggerTime == (int)sortedwaves[wIx + 1]->m_triggerTime) {
				x += 64;
				if (x + 64 >= g_width) {
					x = 0;
					y += 16;
				}
			} else {
				y += 16 + (int)(sortedwaves[wIx + 1]->m_triggerTime - sortedwaves[wIx]->m_triggerTime);
				x = 0;
			}
		}
	}

	if (resetWorkingWaveButtonIx) {
		m_workingWaveButtonIx = -1;
	}

	if (m_selectedWaves.size() == 1) {
		const model::Wave* pSelectedWave(m_selectedWaves[0]);
		const int wIx((int)(pSelectedWave - m_model.m_waves));

		float triggerTime(m_model.m_waves[wIx].m_triggerTime);
		float speedMultiplier(m_model.m_waves[wIx].m_speedMultiplier);
		int controller(m_model.m_waves[wIx].m_controller);
		int formation(m_model.m_waves[wIx].m_formationIx);
		int dropWeapon(m_model.m_waves[wIx].m_dropWeapon);
		math::Vector2f offset(m_model.m_waves[wIx].m_offset);

		m_core.DoLabel(g_width - 256, g_height - 16, 128, 16, "Trigger Time:", DT_RIGHT | DT_VCENTER);
		m_core.DoLabel(g_width - 256, g_height - 32, 128, 16, "Controller:", DT_RIGHT | DT_VCENTER);
		m_core.DoLabel(g_width - 256, g_height - 48, 128, 16, "Speed Mult:", DT_RIGHT | DT_VCENTER);
		m_core.DoLabel(g_width - 256, g_height - 64, 128, 16, "Drop Weapon:", DT_RIGHT | DT_VCENTER);
		m_core.DoLabel(g_width - 256, g_height - 80, 128, 16, "Offset Y:", DT_RIGHT | DT_VCENTER);
		m_core.DoLabel(g_width - 256, g_height - 96, 128, 16, "Offset X:", DT_RIGHT | DT_VCENTER);


		if (m_core.DoSpinnerButton(g_width - 128, g_height - 16, 128, 16, triggerTime, 1.00f * multiplier) ||
			m_core.DoSpinnerButton(g_width - 128, g_height - 32, 128, 16, controller) ||
			m_core.DoSpinnerButton(g_width - 128, g_height - 48, 128, 16, speedMultiplier, 0.1f) ||
			m_core.DoSpinnerButton(g_width - 128, g_height - 64, 128, 16, dropWeapon) ||
			m_core.DoSpinnerButton(g_width - 128, g_height - 80, 128, 16, offset.y, 1.0f * multiplier) ||
			m_core.DoSpinnerButton(g_width - 128, g_height - 96, 128, 16, offset.x, 1.0f * multiplier)) {
			a_target.SetWave(wIx, triggerTime, speedMultiplier, controller, formation, dropWeapon, offset);
		}

		m_core.DoLabel(g_width - 256, 0, 128, 16, "Formation:", DT_RIGHT | DT_VCENTER);
		if (m_core.DoSpinnerButton(g_width - 128, 0, 128, 16, formation)) {
			// go to the next or previous valid formation;
			if (formation > m_model.m_waves[wIx].m_formationIx) {
				if (formation >= model::Model::g_maxFormations) {
					formation = 0;
				}
				while (m_model.m_formations[formation].m_enemies.size() == 0) {
					formation++;
					formation %= model::Model::g_maxFormations;
				}
			} else if (formation < m_model.m_waves[wIx].m_formationIx) {
				if (formation < 0) {
					formation = model::Model::g_maxFormations - 1;
				}

				while (m_model.m_formations[formation].m_enemies.size() == 0) {
					formation--;
					if (formation < 0) {
						formation = model::Model::g_maxFormations - 1;
					}
				}
			}

			a_target.SetWave(wIx, triggerTime, speedMultiplier, controller, formation, dropWeapon, offset);

		}
	} else if (m_selectedWaves.size() > 0) {
		if (m_core.DoButton(g_width - 256, g_height - 16, 128, 16, "Trigger Time --", false) == ViewCore::WS_SuperHotTimed) {
			for (size_t i = 0; i < m_selectedWaves.size(); i++) {
				const model::Wave* pSelectedWave(m_selectedWaves[i]);
				const int wIx((int)(pSelectedWave - m_model.m_waves));

				a_target.SetWave(wIx, pSelectedWave->m_triggerTime - 1.0f * multiplier, pSelectedWave->m_speedMultiplier, pSelectedWave->m_controller, pSelectedWave->m_formationIx, pSelectedWave->m_dropWeapon, pSelectedWave->m_offset);
			}
		}
		if (m_core.DoButton(g_width - 128, g_height - 16, 128, 16, "Trigger Time ++", false) == ViewCore::WS_SuperHotTimed) {
			for (size_t i = 0; i < m_selectedWaves.size(); i++) {
				const model::Wave* pSelectedWave(m_selectedWaves[i]);
				const int wIx((int)(pSelectedWave - m_model.m_waves));

				a_target.SetWave(wIx, pSelectedWave->m_triggerTime + 1.0f * multiplier, pSelectedWave->m_speedMultiplier, pSelectedWave->m_controller, pSelectedWave->m_formationIx, pSelectedWave->m_dropWeapon, pSelectedWave->m_offset);
			}			
		}

		if (m_core.DoButton(g_width - 256, g_height - 32, 128, 16, "Offset Y --", false) == ViewCore::WS_SuperHotTimed) {
			for (size_t i = 0; i < m_selectedWaves.size(); i++) {
				const model::Wave* pSelectedWave(m_selectedWaves[i]);
				const int wIx((int)(pSelectedWave - m_model.m_waves));
				math::Vector2f offset(pSelectedWave->m_offset);
				offset.y -= 1.0f * multiplier;

				a_target.SetWave(wIx, pSelectedWave->m_triggerTime, pSelectedWave->m_speedMultiplier, pSelectedWave->m_controller, pSelectedWave->m_formationIx, pSelectedWave->m_dropWeapon, offset);
			}
		}
		if (m_core.DoButton(g_width - 128, g_height - 32, 128, 16, "Offset Y ++", false) == ViewCore::WS_SuperHotTimed) {
			for (size_t i = 0; i < m_selectedWaves.size(); i++) {
				const model::Wave* pSelectedWave(m_selectedWaves[i]);
				const int wIx((int)(pSelectedWave - m_model.m_waves));
				math::Vector2f offset(pSelectedWave->m_offset);
				offset.y += 1.0f * multiplier;

				a_target.SetWave(wIx, pSelectedWave->m_triggerTime, pSelectedWave->m_speedMultiplier, pSelectedWave->m_controller, pSelectedWave->m_formationIx, pSelectedWave->m_dropWeapon, offset);
			}
		}

		if (m_core.DoButton(g_width - 256, g_height - 48, 128, 16, "Offset X --", false) == ViewCore::WS_SuperHotTimed) {
			for (size_t i = 0; i < m_selectedWaves.size(); i++) {
				const model::Wave* pSelectedWave(m_selectedWaves[i]);
				const int wIx((int)(pSelectedWave - m_model.m_waves));
				math::Vector2f offset(pSelectedWave->m_offset);
				offset.x -= 1.0f * multiplier;

				a_target.SetWave(wIx, pSelectedWave->m_triggerTime, pSelectedWave->m_speedMultiplier, pSelectedWave->m_controller, pSelectedWave->m_formationIx, pSelectedWave->m_dropWeapon, offset);
			}
		}
		if (m_core.DoButton(g_width - 128, g_height - 48, 128, 16, "Offset X ++", false) == ViewCore::WS_SuperHotTimed) {
			for (size_t i = 0; i < m_selectedWaves.size(); i++) {
				const model::Wave* pSelectedWave(m_selectedWaves[i]);
				const int wIx((int)(pSelectedWave - m_model.m_waves));
				math::Vector2f offset(pSelectedWave->m_offset);
				offset.x += 1.0f * multiplier;

				a_target.SetWave(wIx, pSelectedWave->m_triggerTime, pSelectedWave->m_speedMultiplier, pSelectedWave->m_controller, pSelectedWave->m_formationIx, pSelectedWave->m_dropWeapon, offset);
			}
		}

	}

	for (int i = 0; i < model::Model::g_maxWaves; i++) {
		if (m_model.m_waves[i].m_levelIx != m_levelIx) {
			continue;
		}
		const int fIx(m_model.m_waves[i].m_formationIx);
		const size_t enemyCount(m_model.m_formations[fIx].m_enemies.size());
		if (enemyCount > 0 && m_time - m_model.m_waves[i].m_triggerTime >= 0.0f) {
			model::Enemy *enemies = new model::Enemy[enemyCount];
			model::FormationController f;

			for (int eIx = 0; eIx < enemyCount; eIx++) {
				m_model.m_formations[fIx].m_enemies[eIx].SetEnemyAlive(enemies[eIx], 0, eIx);
				f.AddEnemy(&enemies[eIx]);
			}

			f.m_speedMultiplier = m_model.m_waves[i].m_speedMultiplier;
			f.m_currentFormation = m_model.m_waves[i].m_controller;
			f.m_currentOffset = m_model.m_waves[i].m_offset;
			f.Update(m_time - m_model.m_waves[i].m_triggerTime - a_dt, 1.0f);
			f.Update(a_dt, 1.0f);	// this is to make the angles right...

			EnemySprite enemySprites;

			for (int eIx = 0; eIx < enemyCount; eIx++) {
				m_core.DrawEnemy(enemySprites, enemies[eIx], 1.0f, m_levelIx, 1.0f);
			}

			m_core.RenderEnemySprites(enemySprites);

			

			delete [] enemies;
		}
	}

	if (m_core.DoButton(128, 0, 64, 16, "Add Wave", false) == ViewCore::WS_Clicked) {
		if (m_selectedWaves.size() > 0) {
			for (size_t i = 0; i < m_selectedWaves.size(); i++) {
				const model::Wave* pSelectedWave(m_selectedWaves[i]);
				a_target.AddWave(m_levelIx, pSelectedWave->m_triggerTime, pSelectedWave->m_speedMultiplier, pSelectedWave->m_formationIx, pSelectedWave->m_controller, pSelectedWave->m_offset);
			}
		} else {
			a_target.AddWave(m_levelIx, 0.0f, 1.0f, 0, 0, math::Vector2f(0.0f, 0.0f));
		}
	}

	if (m_core.DoButton(128, 16, 64, 16, "Delete Wave", false) == ViewCore::WS_Clicked || m_core.m_input.IsPressedVK(VK_DELETE)) {
		for (int i = 0; i < m_selectedWaves.size(); i++) {
			const model::Wave* pSelectedWave(m_selectedWaves[i]);
			const int wIx((int)(pSelectedWave - m_model.m_waves));

			a_target.DeleteWave(wIx);
		}
		m_selectedWaves.clear();
	}

	m_core.m_input.IsClicked(m_core.m_assets.m_mouseFire);
	return View::VS_Ok;
}

int LevelView::GetSelectedIx(const model::Wave* a_pWave) {
	for (size_t i = 0; i < m_selectedWaves.size(); i++) {
		if (m_selectedWaves[i] == a_pWave) {
			return (int)i;
		}
	}
	return -1;
}

}