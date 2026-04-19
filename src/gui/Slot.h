#pragma once

#include "Bindable.h"
#include "Container.h"
#include "ItemStack.h"
#include "TextContainer.h"

namespace gui
{
class Slot : public Container
{
public:
	Slot(Props props = {});
	Slot(Props props, Bindable<ItemStack>& binding);

	const ItemStack& getStack() const;
	
	Slot* setBinding(Bindable<ItemStack>& binding);
	Slot* setStack(ItemStack v);

	void draw(OopMineGame& game) const override;

private:
	Bindable<ItemStack> stack;
	TextContainer* textEle;
};
} // namespace gui
