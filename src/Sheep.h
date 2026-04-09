#pragma once

#include "Mob.h"

class Sheep : public Mob
{
public:
	Sheep(olc::vf2d pos);

	void updateAi(World& world, float elapsed) override;
	void update(World& world, float elapsed) override;
	void draw(OopMineGame& game) const override;

private:
};
