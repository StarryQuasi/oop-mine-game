#include <cassert>
#include <cctype>
#include <memory>
#include <vector>

#include "DroppedItem.h"
#include "Verify.h"
#include "gui/CraftingTable.h"
#include "libs/olcPixelGameEngine.h"

#include "Block.h"
#include "Item.h"
#include "ItemStack.h"
#include "Items.h"
#include "World.h"

int Block::blockIdCounter = 0;

Block::Block(
	std::string name,
	std::string textureName,
	const Item* item,
	bool transparent,
	LootTable lootTable) :
	id(blockIdCounter++),
	name(name),
	textureName(textureName),
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

void Block::onBreak(World& world, olc::vi2d pos) const
{
	for (auto& stack : getLoot(world, pos))
	{
		olc::vf2d dropPos = pos;
		const float posscale = 0.25f;
		dropPos.x += 0.5f + world.randomFloat(-posscale, posscale);
		dropPos.y += 0.5f + world.randomFloat(-posscale, posscale);
		DroppedItem& item = world.addEntity<DroppedItem>(dropPos, std::move(stack));
		olc::vf2d vel = {};
		const float velscale = 16;
		vel.x = world.randomFloat(-velscale, velscale);
		vel.y = world.randomFloat(-velscale * 2, velscale);
		item.setVel(vel);
	}
}

bool Block::onUse(World& world, olc::vi2d pos) const { return false; }

std::vector<ItemStack> Block::getLoot(World& world, olc::vi2d pos) const
{
	assert(item != nullptr);
	if (lootTable.entries.empty())
		return {ItemStack{*item, 1}};
	std::vector<ItemStack> list = {};
	for (const auto& entry : lootTable.entries)
	{
		assert(entry.item != nullptr);
		if (entry.item == &Items::air)
			continue;
		const float rand = world.randomFloat(0, 1);
		const int range = entry.max - entry.min + 1;
		int count;
		const float randRange = (1.0f - entry.probability);
		if (rand < 1.0f - entry.probability)
			count = 0;
		else
			count = (int)((rand - (1.0f - entry.probability)) /
							  entry.probability * range +
						  entry.min);
		assert(
			rand < 1 ? (count == 0 || Verify::in(count, entry.min, entry.max))
					 : Verify::in(count, entry.min, entry.max));
		if (count)
		{
			list.emplace_back(*entry.item, count);
		}
	}
	return list;
}

Block::operator int() const { return id; }

bool Block::operator==(const Block& other) const { return id == other.id; }

CraftingTable::CraftingTable(
	std::string name,
	std::string textureName,
	const Item* item,
	bool transparent,
	LootTable lootTable) :
	Block(
		std::move(name),
		std::move(textureName),
		item,
		transparent,
		std::move(lootTable))
{
}

bool CraftingTable::onUse(World& world, olc::vi2d pos) const
{
	world.getGame().openScreen(
		std::make_unique<gui::CraftingTable>(world.getPlayer()->get()));
	return true;
}

BlockBuilder& BlockBuilder::name(std::string v)
{
	_name = std::move(v);
	return *this;
}

BlockBuilder& BlockBuilder::textureName(std::string v)
{
	_textureName = std::move(v);
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
	_item = &v;
	assert(_probability > 0.0f && _probability <= 1.0f);
	assert(_min >= 0 && _min <= _max);
	assert(_max >= 0);
	assert(_item != nullptr);
	blockBuilder._lootTable.entries.push_back(
		Block::LootTable::Entry{_probability, _min, _max, _item});
	return blockBuilder;
}
