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

	// Returns whether the stack is of Items::air
	bool isEmpty() const;
	// Returns whether both stacks are empty or have the same item and damage
	bool isSameType(const ItemStack& other) const;
	// Returns whether this is empty, or both are the same type and this is 1
	// less than the max stack size
	bool canAdd(const ItemStack& other) const;
	// Returns whether this is empty, or both are the same type and all of other
	// can be added without exceeding the max stack size
	bool canAddAll(const ItemStack& other) const;
	ItemStack& decrease(int v = 1);
	ItemStack& increase(int v = 1);

	// Returns a mutable copy of the const instance, to use inline with
	// setCount()
	ItemStack copy() const;

	ItemStack& validate();

	bool operator==(const ItemStack& other) const = default;

private:
	const Item* item = &Items::air;
	int count = 0;
	int damage = 0;
};
