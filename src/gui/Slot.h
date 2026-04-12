#pragma once

#include "Container.h"
#include "ItemStack.h"
#include "TextContainer.h"

namespace gui
{
class Slot : public Container
{
public:
	Slot(Props props);

	const ItemStack& getStack() const;

	Slot* setStack(const ItemStack& v);

	void draw(OopMineGame& game) const override;

private:
	ItemStack stack = {};
	TextContainer* textEle;
};
} // namespace gui
