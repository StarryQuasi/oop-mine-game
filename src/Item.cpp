#include <cassert>
#include <cctype>
#include <ranges>

#include "Block.h"
#include "Blocks.h"
#include "Item.h"

int Item::itemIdCounter = 0;

Item::Item(
	std::string name,
	std::string textureName,
	const Block* block,
	int maxStackSize,
	int durability,
	int tier) :
	id(itemIdCounter++),
	name(name),
	textureName(textureName),
	maxStackSize(maxStackSize),
	durability(durability),
	block(block),
	tier(tier)
{
}

int Item::getId() const { return id; }

const std::string& Item::getName() const { return name; }

const std::string& Item::getTextureName() const { return textureName; }

int Item::getMaxStackSize() const { return maxStackSize; }

int Item::getDurability() const { return durability; }

const Block& Item::getBlock() const
{
	return block != nullptr ? *block : Blocks::air;
}

bool Item::operator==(const Item& other) const { return id == other.id; }

ItemBuilder& ItemBuilder::name(std::string v)
{
	_name = std::move(v);
	return *this;
}

ItemBuilder& ItemBuilder::textureName(std::string v)
{
	_textureName = std::move(v);
	return *this;
}

ItemBuilder& ItemBuilder::block(const Block& v)
{
	_block = &v;
	return *this;
}

ItemBuilder& ItemBuilder::maxStackSize(int v)
{
	_maxStackSize = v;
	return *this;
}

ItemBuilder& ItemBuilder::durability(int v)
{
	_durability = v;
	return *this;
}

ItemBuilder& ItemBuilder::tier(int v)
{
	_tier = v;
	return *this;
}

Item ItemBuilder::build()
{
	assert(!_name.empty());
	if (_textureName.empty())
	{
		_textureName = _name;
		for (char& c : _textureName)
		{
			c = std::tolower(c);
			if (c == ' ')
				c = '_';
		}
	}
	if (_block == nullptr)
		_block = &Blocks::air;
	assert(_maxStackSize >= 0);
	assert(_durability >= 0);
	assert(_tier >= 0);
	return Item{
		std::move(_name),
		std::move(_textureName),
		_block,
		_maxStackSize,
		_durability,
		_tier};
}