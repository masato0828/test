#pragma once
#include"Geometry.h"
#include<list>
struct HomingShot;
class Trail
{
private:
	HomingShot& owner_;
	std::list<Position2>history_;
public:
	Trail(HomingShot& owner);
	void Update();
	void Draw();
	void Clear();

};

