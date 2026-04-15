#include <cassert>
#include <cctype>
#include <ranges>

#include "Block.h"
#include "Item.h"
#include "Items.h"

int Block::blockIdCounter = 0;

Block::Block(std::string name, const Item* item, bool transparent) :
	id(blockIdCounter++),
	name(name),
	textureName(
		name |
		std::views::transform([](unsigned char c) { return std::tolower(c); }) |
		std::views::split(std::string(" ")) |
		std::views::join_with(std::string("_")) |
		std::ranges::to<std::string>()),
	item(item),
	transparent(transparent)
{
}

int Block::getId() const { return id; }

const std::string& Block::getName() const { return name; }

int Block::getLightLevel() const { return 15; }

const std::string& Block::getTextureName() const { return textureName; }

bool Block::requiresUpdate() const { return false; }

bool Block::requiresRenderUpdate() const { return false; }

bool Block::isSolid() const { return !transparent; }

bool Block::isReplaceable() const { return transparent; }

const Item& Block::getItem() const
{
	return item != nullptr ? *item : Items::air;
}

void Block::update(World& world, int x, int y) const {}

void Block::renderUpdate(World& world, int x, int y) const {}

Block::operator int() const { return id; }

bool Block::operator==(const Block& other) const { return id == other.id; }

BlockBuilder& BlockBuilder::name(std::string v)
{
	_name = std::move(v);
	return *this;
}

BlockBuilder& BlockBuilder::item(const Item& v)
{
	_item = &v;
	return *this;
}

BlockBuilder& BlockBuilder::transparent(bool v)
{
	_transparent = v;
	return *this;
}

Block BlockBuilder::build()
{
	assert(!_name.empty());
	if (_item == nullptr)
		_item = &Items::air;
	return Block{_name, _item, _transparent};
}
