#pragma once

#include "../lib/entity.h"

class Bullet : public Entity {
public:
	void Init(void);
	void Destroy(void);
	void Update(void);
	void Draw(void);
	void Overlap(Entity &other);
};

class Coin : public Entity {
public:
	void Init(void);
	void Destroy(void);
	void Update(void);
	void Draw(void);
	void Overlap(Entity &other);
};

class Goomba : public Entity {
public:
	void Init(void);
	void Destroy(void);
	void Update(void);
	void Draw(void);
	void Overlap(Entity &other);
};

class Player : public Entity {
private:
	enum PlayerState {
		PLAYER_STATE_NORMAL,
		PLAYER_STATE_JUMP,
		PLAYER_STATE_DEATH,
	};

public:
	void Init(void);
	void Destroy(void);
	void Update(void);
	void Draw(void);
	void Overlap(Entity &other);

private:
	int hang_time = 0;
};

class Shooter : public Entity {
public:
	void Init(void);
	void Destroy(void);
	void Update(void);
	void Draw(void);
	void Overlap(Entity &other);

private:
	float shoot_timer;
};