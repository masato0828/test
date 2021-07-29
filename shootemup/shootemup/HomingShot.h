#pragma once
#include "Trail.h"

struct Bullet {
	Position2 pos;//À•W
	Vector2 vel;//‘¬“x
	Vector2 accel;
	bool isActive = false;//¶‚«‚Ä‚é‚©`H
};

struct HomingShot :
    public Bullet
{
	Trail trail;
	HomingShot();

};

