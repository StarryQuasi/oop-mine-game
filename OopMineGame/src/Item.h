#pragma once

#include <string>

class Block;

class Item
{
public:
	Item(std::string name, const Block* block, int maxStackSize = 64);
	int getId() const;
	const std::string& getName() const;
	virtual const std::string& getTextureName() const;
	int getMaxStackSize() const;
	int getDurability() const;
	const Block& getBlock() const;

	bool operator ==(const Item& other) const;

private:
	static int itemIdCounter;
	const int id;
	const std::string name;
	const std::string textureName;
	const int maxStackSize;
	const int durability;
	const Block* block = nullptr;
};
