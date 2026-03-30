#include "Slot.h"
#include "../Blocks.h"
#include "../OopMineGame.h"

namespace gui
{
	Slot::Slot(olc::vi2d size) :
		Container(size)
	{
		textEle = addChild<TextContainer>("");
		textEle->setAnchor(Anchor::btmRight)
			->setOrigin(Anchor::btmRight);
	}

	ItemStack Slot::getStack() const
	{
		return stack;
	}

	Slot* Slot::setStack(const ItemStack& v)
	{
		auto v2 = v.getValidated();
		if (stack != v2)
		{
			invalidate();
			stack = v2;
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
		const std::string ns = stack.getItem().getBlock() != Blocks::air ? "block/" : "item/";
		const std::string assetName = ns + stack.getItem().getTextureName() + ".png";
		if (const auto& asset = game.getAsset(assetName); asset.has_value())
		{
			const olc::vi2d fakePadding = { 3, 3 };
			game.DrawDecal(
				getAbsolutePos() + fakePadding,
				asset->Decal(),
				(olc::vf2d)(getDrawSize() - fakePadding * 2) / asset->Sprite()->Size());
		}

		Container::draw(game);
	}
}
