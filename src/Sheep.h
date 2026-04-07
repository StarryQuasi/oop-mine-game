#pragma once

#include "Entity.h"

class Sheep : public Entity
{
public:
	Sheep(olc::vf2d pos);

	void update(World& world, float elapsed) override;
	void draw(OopMineGame& game) const override;

private:
};
