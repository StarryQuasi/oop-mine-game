#pragma once

#include "Container.h"
#include "TextContainer.h"
#include "../ItemStack.h"

namespace gui
{
	class Slot : public Container
	{
	public:
		Slot(olc::vi2d size = { 20, 20 });

		ItemStack getStack() const;
		Slot* setStack(const ItemStack& v);

		void draw(OopMineGame& game) const override;

	private:
		ItemStack stack = {};
		TextContainer* textEle;
	};
}
