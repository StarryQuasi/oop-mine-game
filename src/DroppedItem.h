#pragma once

#include "Entity.h"
#include "ItemStack.h"
#include "OopMineGame.h"
#include <chrono>

class DroppedItem : public Entity
{
public:
	DroppedItem(olc::vf2d pos, ItemStack stack);

	ItemStack getStack() const;

	void draw(OopMineGame& game) const override;

private:
	ItemStack stack = {};
	std::chrono::steady_clock::time_point drawTimeStart = {};
};
