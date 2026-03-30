#ifndef ITEMSTACK_H
#define ITEMSTACK_H

#include "Item.h"
#include "Items.h"

class ItemStack
{
public:
	ItemStack(const Item& item = Items::air, int count = 0, int damage = 0);
	const Item& getItem() const;
	bool isEmpty() const;
	int getCount() const;
	void setCount(int);
	int getDamage() const;
	void setDamage(int);
	ItemStack getValidated() const;

	bool operator==(const ItemStack& other) const = default;

private:
	const Item* item = &Items::air;
	int count = 0;
	int damage = 0;
};

#endif
