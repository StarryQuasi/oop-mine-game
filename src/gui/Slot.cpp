#include "Slot.h"
#include "Blocks.h"
#include "OopMineGame.h"

namespace gui
{
Slot::Slot(olc::vi2d size) :
	Container(size)
{
	textEle = addChild<TextContainer>("");
	textEle->setAnchor(Anchor::btmRight)->setOrigin(Anchor::btmRight);
}

const ItemStack& Slot::getStack() const { return stack; }

Slot* Slot::setStack(const ItemStack& v)
{
	if (stack != v)
	{
		invalidate();
		stack = v;
		if (stack.getCount() != 0)
			textEle->setText(std::to_string(stack.getCount()));
		else
			textEle->setText("");
	}
	return this;
}

void Slot::draw(OopMineGame& game) const
{
	if (stack.getItem() == Items::air)
		return;
	const bool isBlock = stack.getItem().getBlock() != Blocks::air;
	const auto& decalPatch =
		isBlock
			? game.getBlockAssetPatch(stack.getItem().getBlock().getId())
			: game.getAsset("item/" + stack.getItem().getTextureName() + ".png")
				  .transform([](const olc::Renderable& r)
							 { return (olc::DecalPatch)r.Decal(); });
	if (decalPatch.has_value())
	{
		const olc::vi2d fakePadding = {3, 3};
		const olc::vf2d realDrawSize =
			(olc::vf2d)(getDrawSize() - fakePadding * 2);
		// const olc::vi2d patchSize = isBlock ? olc::vi2d{ 32, 32 } :
		// decalPatch.value().decal->sprite->Size();
		//  The scale argument here for the olc::DecalPatch overload is actually
		//  the size
		game.DrawDecal(
			getAbsolutePos() + fakePadding, decalPatch.value(), realDrawSize);
	}

	Container::draw(game);
}
} // namespace gui
