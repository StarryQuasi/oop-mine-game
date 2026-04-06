#include <ranges>
#include <cctype>

#include "Block.h"
#include "Item.h"
#include "Items.h"

int Block::blockIdCounter = 0;

Block::Block(std::string name, const Item* item) :
	id(blockIdCounter++),
	name(name),
	textureName(name
		| std::views::transform([](unsigned char c) { return std::tolower(c); } )
		| std::views::split(std::string(" "))
		| std::views::join_with(std::string("_"))
		| std::ranges::to<std::string>()),
	item(item)
{}

Block::Block(std::string name, std::string textureName, const Item* item) :
	id(blockIdCounter++),
	name(std::move(name)),
	textureName(std::move(textureName)),
	item(item)
{}

int Block::getId() const
{
	return id;
}

const std::string& Block::getName() const
{
	return name;
}

int Block::getLightLevel() const
{
	return 15;
}

const std::string& Block::getTextureName() const
{
	return textureName;
}

bool Block::requiresUpdate() const
{
	return false;
}

bool Block::requiresRenderUpdate() const
{
	return false;
}

bool Block::canCollide() const
{
	return true;
}

bool Block::isReplaceable() const
{
	return false;
}

const Item& Block::getItem() const
{
	return item != nullptr ? *item : Items::air;
}

void Block::update(World& world, int x, int y) const
{
}

void Block::renderUpdate(World & world, int x, int y) const
{
}

Block::operator int() const
{
	return id;
}

bool Block::operator==(const Block& other) const
{
	return id == other.id;
}

TransparentBlock::TransparentBlock(const std::string& name, const Item* item) :
	Block(name, item)
{
}

bool TransparentBlock::isReplaceable() const
{
	return true;
}