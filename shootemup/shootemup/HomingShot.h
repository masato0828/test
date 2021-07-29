#pragma once
#include "Trail.h"

struct Bullet {
	Position2 pos;//座標
	Vector2 vel;//速度
	Vector2 accel;
	bool isActive = false;//生きてるか〜？
};

struct HomingShot :
    public Bullet
{
	Trail trail;
	HomingShot();

};

