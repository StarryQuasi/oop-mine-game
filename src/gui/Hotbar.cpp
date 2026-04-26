#include "Hotbar.h"
#include "Anchor.h"
#include "Player.h"
#include "Slot.h"
#include "Verify.h"

namespace gui
{
Hotbar::Hotbar(Props props) :
	FlowContainer(props)
{
	setPadding(props.padding.value_or(olc::vi2d{1, 1}));
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
	selectors[0]->setVisible(true);

	selection.onValueChanged(
		[this](const int& old, const int& $new)
		{
			assert(Verify::index($new, slots.size()));
			selectors[old]->setVisible(false);
			selectors[$new]->setVisible(true);
		});
}

Slot* Hotbar::getSlot(int i) { return slots[i]; }

const ItemStack& Hotbar::getStack(int i)
{
	assert(Verify::index(i, slots.size()));
	return slots[i]->getStack();
}

// TODO: Maybe osu!framework's Bindable?
Hotbar* Hotbar::setBinding(Player& player, Bindable<int>& selection)
{
	for (int i = 0; i < 9; i++)
		slots[i]->setBinding(player.getInvItem(i));
	this->selection.rebind(selection);
	return this;
}
} // namespace gui
