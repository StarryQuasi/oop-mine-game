#pragma once

#include "FlowContainer.h"
#include "Slot.h"

namespace gui
{
class Hotbar : public FlowContainer
{
public:
	Hotbar(Props props = {});

	int getSelection() const;
	Slot* getSlot(int i);
	const ItemStack& getStack(int i);

	Hotbar* setSelection(int i);
	Hotbar* setStack(int i, const ItemStack& stack);

private:
	std::array<Slot*, 9> slots = {};
	std::array<Container*, 9> selectors = {};
	int selectedSlot = 0;
};
} // namespace gui
