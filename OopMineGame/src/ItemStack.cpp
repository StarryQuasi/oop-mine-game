#include <algorithm>

#include "ItemStack.h"
#include "Items.h"

ItemStack::ItemStack(const Item& item, int count, int damage) :
	item(&item),
	count(count),
	damage(damage)
{}

const Item& ItemStack::getItem() const
{
	return *item;
}

bool ItemStack::isEmpty() const
{
	return *item == Items::air;
}

int ItemStack::getCount() const
{
	return count;
}

ItemStack& ItemStack::setCount(int v)
{
	count = v;
	return *this;
}

int ItemStack::getDamage() const
{
	return 0;
}

ItemStack& ItemStack::setDamage(int v)
{
	damage = v;
	return *this;
}

ItemStack& ItemStack::decrease()
{
	count--;
	return *this;
}

ItemStack& ItemStack::increase()
{
	count++;
	return *this;
}

// TODO: Make it always valid
ItemStack ItemStack::getValidated() const
{
	ItemStack r = *this;
	if (r.item->getDurability() > 0)
	{
		if (r.damage < 0 || r.damage >= r.item->getDurability())
		{
			r.damage = 0;
			r.count--;
		}
	}
	else
	{
		r.damage = 0;
	}
	if (r.count <= 0)
		r = ItemStack();
	else if (r.count > r.item->getMaxStackSize())
		r.count = r.item->getMaxStackSize();
	return r;
}
