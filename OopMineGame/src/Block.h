#ifndef BLOCK_H
#define BLOCK_H

#include <string>

class World;
class Item;

class Block
{
public:
	Block(std::string name, const Item* item);
	Block(std::string name, std::string textureName, const Item* item);
	int getId() const;
	const std::string& getName() const;
	virtual int getLightLevel() const;
	virtual const std::string& getTextureName() const;
	virtual bool requiresUpdate() const;
	virtual bool requiresRenderUpdate() const;
	virtual bool canCollide() const;
	// Whether it can be replaced by vegetation like trees
	virtual bool isReplaceable() const;
	virtual const Item& getItem() const;
	virtual void update(World& world, int x, int y) const;
	virtual void renderUpdate(World& world, int x, int y) const;

	operator int() const;
	bool operator ==(const Block& other) const;

private:
	static int blockIdCounter;
	const int id;
	const std::string name;
	const std::string textureName;
	const Item* item = nullptr;
};

class TransparentBlock : public Block
{
public:
	TransparentBlock(const std::string& name, const Item* item);
	bool isReplaceable() const override;
};

#endif
