#include <algorithm>

#include "ItemStack.h"
#include "Items.h"

ItemStack::ItemStack()
{}

ItemStack::ItemStack(const Item& item, int count, int damage) :
	item(&item),
	count(count),
	damage(damage)
{
	validate();
}

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
	validate();
	return *this;
}

int ItemStack::getDamage() const
{
	return damage;
}

ItemStack& ItemStack::setDamage(int v)
{
	damage = v;
	validate();
	return *this;
}

ItemStack& ItemStack::decrease()
{
	count--;
	validate();
	return *this;
}

ItemStack& ItemStack::increase()
{
	count++;
	validate();
	return *this;
}

ItemStack ItemStack::copy() const
{
	return *this;
}

ItemStack& ItemStack::validate()
{
	if (item->getDurability() > 0)
	{
		if (damage < 0 || damage >= item->getDurability())
		{
			damage = 0;
			count--;
		}
	}
	else
	{
		damage = 0;
	}
	if (count <= 0)
		*this = {};
	else if (count > item->getMaxStackSize())
		count = item->getMaxStackSize();
	return *this;
}
