#pragma once

#include "Entity.h"

class Player : public Entity
{
public:
	Player(olc::vf2d pos);

	void updateInput(World& world, float elapsed) override;
	void update(World& world, float elapsed) override;
	void draw(OopMineGame&) const override;

private:
	std::vector<olc::vf2d> hairPoints = {};
	olc::Renderable hairBuffer = {};

	olc::vf2d getHairOrigin() const;
};
