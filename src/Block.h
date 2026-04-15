#pragma once

#include <string>

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

	virtual void update(World& world, int x, int y) const;
	virtual void renderUpdate(World& world, int x, int y) const;

	operator int() const;
	bool operator==(const Block& other) const;

private:
	friend class BlockBuilder;

	static int blockIdCounter;
	const int id;
	const std::string name;
	const std::string textureName;
	const Item* item = nullptr;
	const bool transparent;

	Block(std::string name, const Item* item, bool transparent);
};

class BlockBuilder
{
public:
	BlockBuilder& name(std::string v);
	BlockBuilder& item(const Item& v);
	BlockBuilder& transparent(bool v);
	Block build();

private:
	std::string _name = {};
	const Item* _item = nullptr;
	bool _transparent = false;
};
