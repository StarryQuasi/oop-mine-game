#pragma once

#include "Entity.h"

class Player : public Entity
{
public:
	// TODO: Should this be handled inside Player?
	struct Input
	{
		const bool left = false;
		const bool right = false;
		const bool jump = false;
		const bool attack = false;
		const bool use = false;
		const olc::vf2d target = {};
	};

	Player(olc::vf2d pos);

	void update(World& world, float elapsed) override;
	void draw(OopMineGame&) const override;

private:
	std::vector<olc::vf2d> hairPoints = {};
	olc::Renderable hairBuffer = {};

	olc::vf2d getHairOrigin() const;
};
