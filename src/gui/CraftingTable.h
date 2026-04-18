#pragma once

#include <array>

#include "Container.h"
#include "Player.h"
#include "Slot.h"

namespace gui
{
class CraftingTable : public Container
{
public:
	CraftingTable(Player& player);

	void update(OopMineGame& game) override;

private:
	Player* player{};
	std::array<Slot*, 36> slots;
};
} // namespace gui