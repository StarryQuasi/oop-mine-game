#include "../Verify.h"
#include "Hotbar.h"
#include "Slot.h"

namespace gui
{
	Hotbar::Hotbar() :
		FlowContainer()
	{
		setPadding({ 1, 1 });
		setAssetName("gui/hotbar.png");
		for (int i = 0; i < slots.size(); i++)
		{
			Container* slotContainer = addChild<Container>(olc::vi2d{ 20, 20 });
			selectors[i] = slotContainer->addChild<Container>(olc::vi2d{ 24, 23 });
			selectors[i]->setAnchor(Anchor::midMiddle);
			selectors[i]->setOrigin(Anchor::midMiddle);
			selectors[i]->setPos({ 0, -1 });
			selectors[i]->setVisible(false);
			selectors[i]->setAssetName("gui/hotbar_selection.png");
			slots[i] = slotContainer->addChild<Slot>();
		}
		selectors[selectedSlot]->setVisible(true);
	}

	int Hotbar::getSelection() const
	{
		return selectedSlot;
	}

	Slot* Hotbar::getSlot(int i)
	{
		return slots[i];
	}

	const ItemStack& Hotbar::getStack(int i)
	{
		assert(Verify::index(i, slots.size()));
		return slots[i]->getStack();
	}

	Hotbar* Hotbar::setSelection(int i)
	{
		assert(Verify::index(i, slots.size()));
		if (selectedSlot != i)
		{
			selectors[selectedSlot]->setVisible(false);
			selectedSlot = i;
			selectors[selectedSlot]->setVisible(true);
		}
		return this;
	}

	// TODO: Maybe osu!framework's Bindable?
	Hotbar* Hotbar::setStack(int i, const ItemStack& stack)
	{
		assert(Verify::index(i, slots.size()));
		slots[i]->setStack(stack);
		return this;
	}
}
