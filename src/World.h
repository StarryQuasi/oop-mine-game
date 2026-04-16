#pragma once

#include <cassert>
#include <concepts>
#include <functional>
#include <mdspan>
#include <memory>
#include <random>
#include <ranges>
#include <vector>

#include "libs/FastNoiseLite.h"
#include "libs/olcPixelGameEngine.h"

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
	World(OopMineGame& game, GenerationSettings settings);

	int randomInt(int min, int maxInclusive);
	float randomFloat(float min, float maxExclusive);

	OopMineGame& getGame();
	olc::vi2d getSize() const;
	int getSeed() const;
	const Block& getBlock(olc::vi2d p) const;

	void setBlock(olc::vi2d p, const Block& block);
	void breakBlock(olc::vi2d p);

	// Returns -1 if not found
	int findTopmostSolid(int x) const;
	// Returns -1 if not found
	int findTopmostBlock(int x, const Block& block) const;

	bool isValidPosition(olc::vi2d p) const;
	bool isSolidBlock(olc::vi2d p) const;

	template <typename T, typename... Args>
		requires std::derived_from<T, Entity>
	T& addEntity(Args&&... args);
	template <typename T, typename... Args>
		requires std::derived_from<T, Entity>
	T& adoptEntity(std::unique_ptr<T> v);
	std::optional<std::reference_wrapper<Entity>> getEntity(int id) const;
	template <typename T>
		requires std::derived_from<T, Entity>
	std::optional<std::reference_wrapper<T>> getEntity(int id) const;
	std::optional<std::reference_wrapper<Player>> getPlayer() const;
	template <typename T>
		requires std::derived_from<T, Entity>
	std::vector<std::reference_wrapper<T>>
	getEntities(std::pair<olc::vf2d, olc::vf2d> bb) const;

	void update(float elapsedTime);
	void draw(OopMineGame&);

private:
	const GenerationSettings settings;
	OopMineGame* game;
	FastNoiseLite noise;
	std::vector<int> blocksRaw;
	// std::dextents = dynamic extents
	// std::layout_left = column major (access by x, y)
	std::mdspan<int, std::dextents<size_t, 2>, std::layout_left> blocks;
	std::unordered_map<int, std::unique_ptr<Entity>> entities = {};
	std::mt19937 random = {};
	std::vector<olc::vf2d> drawBufPos = {};
	std::vector<olc::vf2d> drawBufUv = {};

	void generateWorld();
	// Returns [0, 1]
	float sampleAt(int x, int y);
	// Returns [0, 1]
	float sampleAt(FastNoiseLite& noise, int x, int y);
	// Returns [0, 1]
	float sampleAt(olc::vi2d pos);
	// Converts random unsigned to float [0, 1)
	float randToFloat(unsigned v);
};

template <typename T, typename... Args>
	requires std::derived_from<T, Entity>
T& World::addEntity(Args&&... args)
{
	return adoptEntity(std::make_unique<T>(std::forward<Args>(args)...));
}

template <typename T, typename... Args>
	requires std::derived_from<T, Entity>
T& World::adoptEntity(std::unique_ptr<T> entity)
{
	const int id = entity->getId();
	entities[id] = std::move(entity);
	return *static_cast<T*>(entities[id].get());
}

template <class T>
	requires std::derived_from<T, Entity>
std::optional<std::reference_wrapper<T>> World::getEntity(int id) const
{
	if (const auto it = entities.find(id); it != entities.end())
		if (T* t = dynamic_cast<T*>(&*it->second); t != nullptr && !t->isdead())
			return std::ref(*t);
	return {};
}

template <typename T>
	requires std::derived_from<T, Entity>
std::vector<std::reference_wrapper<T>>
World::getEntities(std::pair<olc::vf2d, olc::vf2d> bb) const
{
	assert(bb.second.x >= bb.first.x && bb.second.y >= bb.first.y);
	return entities | std::views::values |
		   std::views::transform(
			   [&bb](const std::unique_ptr<Entity>& e) -> T*
			   {
				   Entity* ep = e.get();
				   if (ep->isDead())
					   return nullptr;
				   if (T* tp = dynamic_cast<T*>(ep); tp != nullptr)
				   {
					   const std::pair<olc::vf2d, olc::vf2d> ebb = e->getBb();
					   if (ebb.first.x < bb.second.x &&
						   ebb.second.x > bb.first.x &&
						   ebb.first.y < bb.second.y &&
						   ebb.second.y > bb.first.y)
						   return tp;
				   }
				   return nullptr;
			   }) |
		   std::views::filter([](T* e) { return e != nullptr; }) |
		   std::views::transform([](T* e) { return std::ref(*e); }) |
		   std::ranges::to<std::vector>();
}
