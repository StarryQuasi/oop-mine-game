#pragma once

#include <mdspan>
#include <vector>

#include <FastNoiseLite.h>
#include <olcPixelGameEngine.h>

#include "Block.h"
#include "Entity.h"
#include "Player.h"

struct GenerationSettings
{
	olc::vi2d size = {256, 96};
	int seed = 0;
	float noiseFrequency = 0.05f;
	int terrainHeightMin = 32;
	int terrainHeightMax = 48;
	float diamondThreshold = 0.85f;
	float copperThresholdMin = 0.31f;
	float copperThresholdMax = 0.41f;
	float coalThreshold = 0.7f;
};

class World
{
public:
	World(GenerationSettings settings);

	olc::vi2d getSize() const;
	int getSeed() const;
	const Block& getBlock(olc::vi2d p) const;

	void setBlock(olc::vi2d p, const Block& block);

	int findTopmostSolid(int x) const; // Returns -1 if not found
	int findTopmostBlock(
		int x, const Block& block) const; // Returns -1 if not found

	bool isValidPosition(olc::vi2d p) const;
	bool isSolidBlock(olc::vi2d p) const;

	template <typename T, typename... Args>
		requires std::derived_from<T, Entity>
	int addEntity(Args&&... args);
	int addEntity(std::unique_ptr<Entity> v);
	std::optional<std::reference_wrapper<Entity>> getEntity(int id) const;
	template <class T>
		requires std::derived_from<T, Entity>
	std::optional<std::reference_wrapper<T>> getEntity(int id) const;
	std::optional<std::reference_wrapper<Player>> getPlayer() const;

	void update(float elapsedTime);
	void draw(OopMineGame&);

private:
	const GenerationSettings settings;
	FastNoiseLite noise;
	std::vector<int> blocksRaw;
	// std::dextents = dynamic extents, std::layout_left = column major (access
	// by x, y)
	std::mdspan<int, std::dextents<size_t, 2>, std::layout_left> blocks;
	std::unordered_map<int, std::unique_ptr<Entity>> entities = {};
	std::vector<olc::vf2d> drawBufPos = {};
	std::vector<olc::vf2d> drawBufUv = {};

	void generateWorld();
	float sampleAt(int x, int y);						// Returns [0, 1]
	float sampleAt(FastNoiseLite& noise, int x, int y); // Returns [0, 1]
	float sampleAt(olc::vi2d pos);						// Returns [0, 1]
};

template <typename T, typename... Args>
	requires std::derived_from<T, Entity>
int World::addEntity(Args&&... args)
{
	return addEntity(std::make_unique<T>(std::forward<Args>(args)...));
}

template <class T>
	requires std::derived_from<T, Entity>
std::optional<std::reference_wrapper<T>> World::getEntity(int id) const
{
	if (const auto it = entities.find(id); it != entities.end())
		if (T* t = dynamic_cast<T*>(&*it->second); t != nullptr)
			return std::ref(*t);
	return {};
}
