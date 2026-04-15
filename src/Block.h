#pragma once

#include <string>

#include "libs/olcPixelGameEngine.h"

#include "ItemStack.h"

class World;
class Item;

class Block
{
public:
	int getId() const;
	const std::string& getName() const;
	virtual int getLightLevel() const;
	virtual const std::string& getTextureName() const;
	virtual const Item& getItem() const;

	virtual bool requiresUpdate() const;
	virtual bool requiresRenderUpdate() const;
	virtual bool isSolid() const;
	// Whether it can be replaced by vegetation like trees
	virtual bool isReplaceable() const;

	virtual void update(World& world, olc::vi2d pos) const;
	virtual void renderUpdate(World& world, olc::vi2d pos) const;
	// Returns list of each loot table entry item of count [min, max] mapped from [0, probability)
	virtual std::vector<ItemStack> getLoot(World& world, olc::vi2d pos) const;

	operator int() const;
	bool operator==(const Block& other) const;

private:
	friend class BlockBuilder;
	friend class LootTableBuilder;

	struct LootTable
	{
		struct Entry
		{
			float probability{};
			int min{};
			int max{};
			const Item* item{};
		};
		std::vector<Entry> entries{};
	};

	static int blockIdCounter;
	const int id{};
	const std::string name{};
	const std::string textureName{};
	const Item* item{};
	const bool transparent{};
	const LootTable lootTable{};

	Block(
		std::string name,
		const Item* item,
		bool transparent,
		LootTable lootTable);
};

class BlockBuilder;

class LootTableBuilder
{
public:
	LootTableBuilder(BlockBuilder& v);

	LootTableBuilder& probability(float v);
	LootTableBuilder& min(int v);
	LootTableBuilder& max(int v);
	BlockBuilder& item(const Item& v);

private:
	BlockBuilder& blockBuilder;
	float _probability = 1.0f;
	int _min = 1;
	int _max = 1;
	const Item* _item = nullptr;
};

class BlockBuilder
{
public:
	BlockBuilder& name(std::string v);
	BlockBuilder& item(const Item& v);
	BlockBuilder& transparent(bool v);
	LootTableBuilder loot();
	Block build();

private:
	friend class LootTableBuilder;

	std::string _name = {};
	const Item* _item = nullptr;
	bool _transparent = false;
	Block::LootTable _lootTable = {};
};
