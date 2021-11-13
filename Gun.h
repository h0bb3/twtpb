// This code is provided by Tobias Olsson under a CC BY 4.0 licence https://creativecommons.org/licenses/by/4.0/
#pragma once


namespace model {

class Enemy;
class Bullet;

class Gun {
	public:
		Gun(void);
		virtual ~Gun(void);

		virtual void Reset();
		virtual void Update(float a_dt);
		virtual bool DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy) = 0;

		float m_shootTimer;
};

class StandardGun : public Gun {
	public:
		virtual bool DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy);
};

class OneShotBombGun : public Gun {
	public:
		virtual void Reset();
		virtual bool DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy);
		float m_time;
};

class Rotating4WayGun : public Gun {
	virtual void Reset();
	virtual bool DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy);
	int m_shootCounter;
};

class SlowRotating4WayGun : public Gun {
	public:
		SlowRotating4WayGun(float a_dir = 1.0f);
		virtual void Reset();
		virtual bool DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy);
		int m_shootCounter;
		float m_dir;	// 1 or -1.0f
};

class FunkyRotating4WayGun : public Gun {
	public:
		virtual void Reset();
		virtual bool DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy);
		virtual void Update(float a_dt);
		int m_shootCounter;
	private:
		float m_dir;
		float m_dirTime;
		float m_time;
};

class NullGun : public Gun {
	virtual bool DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy);
};

class ShockWaveGun : public Gun {
	virtual void Reset();
	virtual bool DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy);
};

class InvShockWaveGun : public Gun {
	virtual void Reset();
	virtual bool DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy);
};

class DirGun : public Gun {
	virtual void Reset();
	virtual bool DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy);
};

class TriShotDirGun : public Gun {
	virtual void Reset();
	virtual bool DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy);
	int m_shootCounter;
};

class AimOffsetGun : public Gun {
	virtual void Reset();
	virtual bool DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy);
};

class SpreadGun : public Gun {
	virtual void Reset();
	virtual bool DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy);
	int m_firedBullets;
	int m_side;
};

class CloudGun : public Gun {
	virtual void Reset();
	virtual bool DoFire(const math::Vector2f &a_playerPos, Bullet &a_bullet, const Enemy &a_enemy);
	int m_firedBullets;
};

}
