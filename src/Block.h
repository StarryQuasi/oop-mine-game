#pragma once

#include <cassert>
#include <concepts>
#include <ranges>
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
	virtual void onBreak(World& world, olc::vi2d pos) const;
	virtual bool onUse(World& world, olc::vi2d pos) const;
	// Returns list of each loot table entry item of count [min, max] mapped
	// from [probability, 1.0f) rolled from a random float
	virtual std::vector<ItemStack> getLoot(World& world, olc::vi2d pos) const;

	operator int() const;
	bool operator==(const Block& other) const;

protected:
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

	Block(
		std::string name,
		std::string textureName,
		const Item* item,
		bool transparent,
		LootTable lootTable);

private:
	static int blockIdCounter;
	const int id{};
	const std::string name{};
	const std::string textureName{};
	const Item* item{};
	const bool transparent{};
	const LootTable lootTable{};
};

class CraftingTable : public Block
{
public:
	CraftingTable(
		std::string name,
		std::string textureName,
		const Item* item,
		bool transparent,
		LootTable lootTable);

	bool onUse(World& world, olc::vi2d pos) const override;
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
	BlockBuilder& textureName(std::string v);
	BlockBuilder& item(const Item& v);
	BlockBuilder& transparent(bool v);
	LootTableBuilder loot();
	template <typename T = Block>
		requires std::derived_from<T, Block>
	T build();

private:
	friend class LootTableBuilder;

	std::string _name = {};
	std::string _textureName = {};
	const Item* _item = nullptr;
	bool _transparent = false;
	Block::LootTable _lootTable = {};
};

template <typename T>
	requires std::derived_from<T, Block>
T BlockBuilder::build()
{
	assert(!_name.empty());
	if (_textureName.empty())
		_textureName = _name |
					   std::views::transform([](unsigned char c)
											 { return std::tolower(c); }) |
					   std::views::split(std::string(" ")) |
					   std::views::join_with(std::string("_")) |
					   std::ranges::to<std::string>();
	if (_item == nullptr)
		_item = &Items::air;
	return T{
		std::move(_name),
		std::move(_textureName),
		_item,
		_transparent,
		std::move(_lootTable)};
}
