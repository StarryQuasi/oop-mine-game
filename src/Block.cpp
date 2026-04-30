#include <cassert>
#include <memory>
#include <queue>
#include <unordered_set>
#include <vector>

#include "Data.h"
#include "libs/olcPixelGameEngine.h"

#include "Block.h"
#include "DroppedItem.h"
#include "Item.h"
#include "ItemStack.h"
#include "Items.h"
#include "Utils.h"
#include "Verify.h"
#include "World.h"
#include "gui/CraftingTable.h"

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

bool Block::requiresRandomUpdate() const { return false; }

bool Block::requiresDrawUpdate() const { return false; }

bool Block::isSolid() const { return !transparent; }

bool Block::isReplaceable() const { return transparent; }

const Item& Block::getItem() const { return *item; }

void Block::update(World& world, olc::vi2d pos) const {}

void Block::randomUpdate(World& world, olc::vi2d pos) const {}

void Block::drawUpdate(World& world, olc::vi2d pos) const {}

void Block::onBreak(World& world, olc::vi2d pos) const
{
	for (auto& stack : getLoot(world, pos))
	{
		olc::vf2d dropPos = pos;
		const float posscale = 0.25f;
		dropPos.x += 0.5f + world.randomFloat(-posscale, posscale);
		dropPos.y += 0.5f + world.randomFloat(-posscale, posscale);
		DroppedItem& item =
			world.addEntity<DroppedItem>(dropPos, std::move(stack));
		olc::vf2d vel = {};
		const float velScale = 16;
		vel.x = world.randomFloat(-velScale, velScale);
		vel.y = world.randomFloat(-velScale * 2, velScale);
		item.setVel(vel);
	}

	const auto now = std::chrono::steady_clock::now();
	const olc::vf2d velRange = {0.5f, 3.0f};
	const auto& assetOpt = world.getGame().getBlockAssetPatch(getId());
	if (assetOpt.has_value())
	{
		const auto& asset = assetOpt.value();
		for (int _ = 0; _ < 32; _++)
		{
			olc::Pixel color = asset.decal->sprite->GetPixel(
				(int)(world.randomFloat(asset.coords[1].x, asset.coords[2].x) *
					  asset.decal->width),
				(int)(world.randomFloat(asset.coords[1].y, asset.coords[0].y) *
					  asset.decal->height));
			world.addParticle({
				.lifeStart = now,
				.lifeEnd =
					now + std::chrono::milliseconds(world.randomInt(100, 700)),
				.pos = olc::vf2d(pos) +
					   olc::vf2d{
						   world.randomFloat(0.0f, 1.0f),
						   world.randomFloat(0.0f, 1.0f)},
				.vel =
					{world.randomFloat(-velRange.x / 2.0f, velRange.x / 2.0f),
					 world.randomFloat(0.0f, velRange.y)},
				.color = color,
				.scale = world.randomFloat(2.0f, 5.0f),
				.alive = true,
				.type = ParticleType::generic,
			});
		}
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
		if (entry.probability == 1.0f && entry.min == entry.max)
		{
			list.emplace_back(*entry.item, entry.min);
			continue;
		}
		const float rand = world.randomFloat(0, 1);
		const float successRange = 1.0f - entry.probability;
		// {0, probability..1.0f}
		// v
		// {0, min..max}
		int count;
		if (rand < successRange)
			count = 0;
		else
			count = (int)Utils::map(
				rand - successRange,
				0.0f,
				entry.probability,
				(float)entry.min,
				(float)(entry.max + 1));
		assert(
			entry.probability == 1
				? Verify::in(count, entry.min, entry.max)
				: (count == 0 || Verify::in(count, entry.min, entry.max)));
		if (count)
		{
			list.emplace_back(*entry.item, count);
		}
	}
	return std::move(list);
}

Block::operator int() const { return id; }

bool Block::operator==(const Block& other) const { return id == other.id; }

bool CraftingTable::onUse(World& world, olc::vi2d pos) const
{
	world.getGame().openScreen(
		std::make_unique<gui::CraftingTable>(world.getPlayer()->get()));
	return true;
}

bool Leaves::requiresDrawUpdate() const { return true; }

bool Leaves::requiresRandomUpdate() const { return true; }

void Leaves::drawUpdate(World& world, olc::vi2d pos) const
{
	if (world.randomInt(1, 5) == 1)
	{
		const auto now = std::chrono::steady_clock::now();
		const olc::vf2d velRange = {1.0f, 1.5f};
		olc::Pixel color =
			std::ranges::find_if(
				Data::getTreeTypes(),
				[this](const auto& t) { return this == t.leaves; })
				->color;
		color.a = world.randomInt(64, 192);
		world.addParticle({
			.lifeStart = now,
			.lifeEnd =
				now + std::chrono::milliseconds(world.randomInt(1000, 2500)),
			.pos = olc::vf2d(pos) +
				   olc::vf2d{
					   world.randomFloat(0.0f, 1.0f),
					   world.randomFloat(0.0f, 1.0f)},
			.vel =
				{world.randomFloat(-velRange.x / 2.0f, velRange.x / 2.0f),
				 world.randomFloat(0.0f, velRange.y)},
			.color = color,
			.scale = world.randomFloat(3.0f, 7.0f),
			.alive = true,
			.type = ParticleType::generic,
		});
	}
}

void Leaves::randomUpdate(World& world, olc::vi2d pos) const
{
	struct Node
	{
		olc::vi2d pos;
		int distance;
	};
	const Block* log = std::ranges::find_if(
						   Data::getTreeTypes(),
						   [this](const auto& t) { return this == t.leaves; })
						   ->log;
	std::unordered_set<olc::vi2d, vi2dHash> visited;
	std::queue<Node> frontier;
	bool hasLogNearby = false;
	const int maxDist = 6;
	visited.insert(pos);
	frontier.push({pos, 0});
	while (!frontier.empty())
	{
		const Node cur = frontier.front();
		frontier.pop();
		if (cur.distance <= maxDist && world.getBlock(cur.pos) == *log)
		{
			hasLogNearby = true;
			break;
		}
		for (const olc::vi2d& dir : {olc::vi2d{-1, 0}, {0, -1}, {1, 0}, {0, 1}})
		{
			const olc::vi2d next = cur.pos + dir;
			if (!world.isValidPosition(next))
				continue;
			if (!(world.getBlock(next) == *log ||
				  world.getBlock(next) == *this))
				continue;
			if (visited.contains(next))
				continue;
			if (cur.distance + 1 > maxDist)
				continue;
			frontier.push({next, cur.distance + 1});
			visited.insert(next);
		}
	}
	if (!hasLogNearby)
	{
		world.breakBlock(pos);
	}
}

bool Sapling::requiresRandomUpdate() const { return true; }

void Sapling::randomUpdate(World& world, olc::vi2d pos) const
{
	if (world.randomInt(1, 5) == 1)
	{
		const auto& pats =
			std::ranges::find_if(
				Data::getTreeTypes(),
				[this](const auto& t) { return this == t.sapling; })
				->patterns;
		world.pastePattern(pos, pats[world.randomInt(0, pats.size() - 1)]);
	}
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
