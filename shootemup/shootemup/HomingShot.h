#pragma once
#include "Trail.h"

struct Bullet {
	Position2 pos;//���W
	Vector2 vel;//���x
	Vector2 accel;
	bool isActive = false;//�����Ă邩�`�H
};

struct HomingShot :
    public Bullet
{
	Trail trail;
	HomingShot();

};

