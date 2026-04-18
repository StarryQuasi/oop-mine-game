#include "Slot.h"
#include "Anchor.h"
#include "Blocks.h"
#include "OopMineGame.h"
#include "gui/Container.h"

namespace gui
{
Slot::Slot(Props props) :
	Container(props)
{
	setSize(props.size.value_or({20, 20}));

	textEle = addChild<TextContainer>({
		.anchor = Anchor::btmRight,
		.origin = Anchor::btmRight,
	});

	stack.onValueChanged(
		[this](const ItemStack&, const ItemStack& $new)
		{
			if ($new.getCount() != 0)
				textEle->setText(std::to_string($new.getCount()));
			else
				textEle->setText("");
		});
}

Slot::Slot(Props props, Bindable<ItemStack>& binding) :
	Slot(props)
{
	setBinding(binding);
}

const ItemStack& Slot::getStack() const { return stack.get(); }

Slot* Slot::setBinding(Bindable<ItemStack>& binding)
{
	stack.unbindAll();
	stack.bindTo(binding);
	return this;
}

void Slot::draw(OopMineGame& game) const
{
	if (stack->getItem() == Items::air)
		return;
	const bool isBlock = stack->getItem().getBlock() != Blocks::air;
	const auto& decalPatch =
		isBlock ? game.getBlockAssetPatch(stack->getItem().getBlock().getId())
				: game.getAsset(
						  "item/" + stack->getItem().getTextureName() + ".png")
					  .transform([](const olc::Renderable& r)
								 { return (olc::DecalPatch)*r.Decal(); });
	if (decalPatch.has_value())
	{
		const olc::vi2d fakePadding = {3, 3};
		const olc::vf2d realDrawSize =
			(olc::vf2d)(getDrawSize() - fakePadding * 2);
		// The scale argument here for the olc::DecalPatch overload is actually
		// the size
		game.DrawDecal(
			getAbsolutePos() + fakePadding, decalPatch.value(), realDrawSize);
	}

	Container::draw(game);
}
} // namespace gui
