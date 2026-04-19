#pragma once

#include <array>

#include "Bindable.h"
#include "Container.h"
#include "ItemStack.h"
#include "Player.h"
#include "Slot.h"

namespace gui
{
class CraftingTable : public Container
{
public:
	CraftingTable(Player& player);

	void update(OopMineGame& game) override;
	void draw(OopMineGame& game) const override;

private:
	Player* player{};
	std::array<Slot*, 36> slots{};
	std::array<Slot*, 9> slotsInput{};
	Slot* slotsOutput{};
	Slot* slotsOnHand{};
	std::array<Bindable<ItemStack>, 9> stacksInput{};
	Bindable<ItemStack> stacksOutput{};
	Bindable<ItemStack> stacksOnHand{};

	bool onSlotClick(Container& me, const MouseEvent& event);
};
} // namespace gui