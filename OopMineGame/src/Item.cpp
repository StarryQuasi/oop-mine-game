#include <ranges>

#include "Item.h"
#include "Block.h"
#include "Blocks.h"

int Item::itemIdCounter = 0;

Item::Item(std::string name, const Block* block, int maxStackSize) :
	id(itemIdCounter++),
	name(name),
	textureName(name
		| std::views::transform(std::tolower)
		| std::views::split(std::string(" "))
		| std::views::join_with(std::string("_"))
		| std::ranges::to<std::string>()),
	maxStackSize(maxStackSize),
	durability(0),
	block(block)
{}

int Item::getId() const
{
	return id;
}

const std::string& Item::getName() const
{
	return name;
}

const std::string& Item::getTextureName() const
{
	return textureName;
}

int Item::getMaxStackSize() const
{
	return maxStackSize;
}

int Item::getDurability() const
{
	return durability;
}

const Block& Item::getBlock() const
{
	return block != nullptr ? *block : Blocks::air;
}

bool Item::operator==(const Item& other) const
{
	return id == other.id;
}
