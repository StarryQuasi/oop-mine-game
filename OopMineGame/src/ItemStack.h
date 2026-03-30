#ifndef ITEMSTACK_H
#define ITEMSTACK_H

#include "Item.h"
#include "Items.h"

class ItemStack
{
public:
	ItemStack(const Item& item = Items::air, int count = 0, int damage = 0);

	const Item& getItem() const;
	int getCount() const;
	int getDamage() const;

	ItemStack& setCount(int);
	ItemStack& setDamage(int);

	ItemStack& decrease();
	ItemStack& increase();

	ItemStack getValidated() const;

	bool isEmpty() const;

	bool operator==(const ItemStack& other) const = default;

private:
	const Item* item = &Items::air;
	int count = 0;
	int damage = 0;
};

#endif
