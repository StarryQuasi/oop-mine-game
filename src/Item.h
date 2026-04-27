#pragma once

#include <string>
#include <vector>

class Block;

class Item
{
public:
	int getId() const;
	const std::string& getName() const;
	virtual const std::string& getTextureName() const;
	int getMaxStackSize() const;
	int getDurability() const;
	int getTier() const;
	const Block& getBlock() const;

	bool operator==(const Item& other) const;

private:
	friend class ItemBuilder;

	static int itemIdCounter;
	const int id;
	const std::string name;
	const std::string textureName;
	const int maxStackSize;
	const int durability;
	const int tier;
	const Block* block = nullptr;

	Item(
		std::string name,
		std::string textureName,
		const Block* block,
		int maxStackSize,
		int durability,
		int tier);
};

class ItemBuilder
{
public:
	ItemBuilder& name(std::string v);
	ItemBuilder& textureName(std::string v);
	ItemBuilder& block(const Block& v);
	ItemBuilder& maxStackSize(int v);
	ItemBuilder& durability(int v);
	ItemBuilder& tier(int v);
	Item build();

private:
	std::string _name = {};
	std::string _textureName = {};
	const Block* _block = nullptr;
	int _maxStackSize = 64;
	int _durability = 0;
	int _tier = 0;
};
