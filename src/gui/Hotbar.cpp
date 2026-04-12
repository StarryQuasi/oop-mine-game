#include "Hotbar.h"
#include "Anchor.h"
#include "Slot.h"
#include "Verify.h"

namespace gui
{
Hotbar::Hotbar(Props props) :
	FlowContainer(props)
{
	setPadding(props.padding.value_or({1, 1}));
	setAssetName(props.assetName.value_or("gui/hotbar.png"));

	for (int i = 0; i < slots.size(); i++)
	{
		Container* slotContainer = addChild<Container>({.size = {{20, 20}}});
		selectors[i] = slotContainer->addChild<Container>({
			.size = {{24, 23}},
			.pos = {{0, -1}},
			.anchor = Anchor::midMiddle,
			.origin = Anchor::midMiddle,
			.visible = false,
			.assetName = "gui/hotbar_selection.png",
		});
		slots[i] = slotContainer->addChild<Slot>();
	}
	selectors[selectedSlot]->setVisible(true);
}

int Hotbar::getSelection() const { return selectedSlot; }

Slot* Hotbar::getSlot(int i) { return slots[i]; }

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
} // namespace gui
