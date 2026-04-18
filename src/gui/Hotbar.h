#pragma once

#include "FlowContainer.h"
#include "Slot.h"

class Player;

namespace gui
{
class Hotbar : public FlowContainer
{
public:
	Hotbar(Props props);

	Slot* getSlot(int i);
	const ItemStack& getStack(int i);

	Hotbar* setBinding(Player& player, Bindable<int>& selection);

private:
	std::array<Slot*, 9> slots{};
	std::array<Container*, 9> selectors{};
	Bindable<int> selection{};
};
} // namespace gui
