// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once

#include <Scene/Sprite.h>
#include <DXRenderer/Effect.h>
#include "HLSLSprite.h"
#include "model.h"


namespace view {
class EnemySprite : public HLSLSprite {
	public:
		
		EnemySprite(void);
		~EnemySprite(void);

		void AddEnemy(const math::Vector2f &a_pos, const math::Vector2f &a_size, const math::Vector2f &a_scale, float a_rotation, const scene::Sprite::UVMap &a_uv, DWORD a_color);
		void AddEnemy(const math::Vector2f &a_pos, const math::Vector2f &a_size, const math::Vector2f &a_scale, float a_rotation, const scene::Sprite::UVMap &a_uv, DWORD a_color, float a_z);

		static const int g_maxSprites = model::Model::g_maxEnemies * 16;

		void Render(dxrnd::Effect &a_effect);

	protected:
		class Vertex {
			public:
				math::Vector4f m_pos;
				math::Vector2f m_translate;
				math::Vector3f m_scaleRotate;
				math::Vector2f m_uv;
				DWORD m_color;
		};

	private:
		Vertex *m_verts;	// need to be on heap
};

}