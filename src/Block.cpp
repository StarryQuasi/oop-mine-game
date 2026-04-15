#include <cassert>
#include <cctype>
#include <ranges>
#include <vector>

#include "libs/olcPixelGameEngine.h"

#include "Block.h"
#include "World.h"
#include "Item.h"
#include "ItemStack.h"
#include "Items.h"

int Block::blockIdCounter = 0;

Block::Block(
	std::string name, const Item* item, bool transparent, LootTable lootTable) :
	id(blockIdCounter++),
	name(name),
	textureName(
		name |
		std::views::transform([](unsigned char c) { return std::tolower(c); }) |
		std::views::split(std::string(" ")) |
		std::views::join_with(std::string("_")) |
		std::ranges::to<std::string>()),
	item(item),
	transparent(transparent),
	lootTable(std::move(lootTable))
{
	assert(item != nullptr);
}

int Block::getId() const { return id; }

const std::string& Block::getName() const { return name; }

int Block::getLightLevel() const { return 15; }

const std::string& Block::getTextureName() const { return textureName; }

bool Block::requiresUpdate() const { return false; }

bool Block::requiresRenderUpdate() const { return false; }

bool Block::isSolid() const { return !transparent; }

bool Block::isReplaceable() const { return transparent; }

const Item& Block::getItem() const { return *item; }

void Block::update(World& world, olc::vi2d pos) const {}

void Block::renderUpdate(World& world, olc::vi2d pos) const {}

std::vector<ItemStack> Block::getLoot(World& world, olc::vi2d pos) const
{
	assert(item != nullptr);
	if (lootTable.entries.empty())
		return {ItemStack{*item, 1}};
	std::vector<ItemStack> list = {};
	for (const auto& entry : lootTable.entries)
	{
		assert(entry.item != nullptr && entry.item != &Items::air);
		const int count = (int)world.randomFloat(entry.min, entry.max + 1);
		if (count)
		{
			list.emplace_back(*entry.item, count);
		}
	}
	return list;
}

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
	assert(_lootTable.entries.size() > 0 || _item == &Items::air);
	return Block{std::move(_name), _item, _transparent, std::move(_lootTable)};
}

LootTableBuilder BlockBuilder::loot() { return {*this}; }

LootTableBuilder::LootTableBuilder(BlockBuilder& v) :
	blockBuilder(v)
{
}

LootTableBuilder& LootTableBuilder::probability(float v)
{
	_probability = v;
	return *this;
}

LootTableBuilder& LootTableBuilder::min(int v)
{
	_min = v;
	return *this;
}

LootTableBuilder& LootTableBuilder::max(int v)
{
	_max = v;
	return *this;
}

BlockBuilder& LootTableBuilder::item(const Item& v)
{
	assert(_probability > 0.0f);
	assert(_min >= 0 && _min <= _max);
	assert(_max >= 0);
	assert(_item != nullptr);
	blockBuilder._lootTable.entries.push_back(
		Block::LootTable::Entry{_probability, _min, _max, _item});
	return blockBuilder;
}
