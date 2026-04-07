#pragma once

#include "Item.h"
#include "Items.h"

class ItemStack
{
public:
	ItemStack();
	ItemStack(const Item& item, int count = 0, int damage = 0);

	const Item& getItem() const;
	int getCount() const;
	int getDamage() const;

	ItemStack& setCount(int v);
	ItemStack& setDamage(int v);

	ItemStack& decrease(int v = 1);
	ItemStack& increase(int v = 1);

	ItemStack copy() const;

	ItemStack& validate();

	bool isEmpty() const;

	bool operator==(const ItemStack& other) const = default;

private:
	const Item* item = &Items::air;
	int count = 0;
	int damage = 0;
};
