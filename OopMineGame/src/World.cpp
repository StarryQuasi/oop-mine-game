#include <random>
#include <limits>
#include <ranges>
#include <mdspan>

#include "OopMineGame.h"
#include "Blocks.h"
#include "World.h"
#include "Entity.h"
#include "Utils.h"
#include "Player.h"
#include "Verify.h"

World::World(GenerationSettings settings) :
	settings(settings),
	noise(settings.seed),
	blocksRaw(settings.size.area()),
	blocks(blocksRaw.data(), settings.size.x, settings.size.y)
{
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	noise.SetFrequency(settings.noiseFrequency);
	generateWorld();
}

olc::vi2d World::getSize() const
{
	return settings.size;
}

int World::getSeed() const
{
	return settings.seed;
}

const Block& World::getBlock(olc::vi2d p) const
{
	assert(isValidPosition(p));
	return Blocks::getById(blocks[p.x, p.y]);
}

void World::setBlock(olc::vi2d p, const Block& block)
{
	assert(isValidPosition(p));
	blocks[p.x, p.y] = block.getId();
}

int World::findTopmostSolid(int x) const
{
	for (int y = 0; y < getSize().y; y++)
		if (!getBlock({ x, y }).isReplaceable())
			return y;
	return -1;
}

int World::findTopmostBlock(int x, const Block& block) const
{
	for (int y = 0; y < getSize().y; y++)
		if (getBlock({ x, y }) == block)
			return y;
	return -1;
}

bool World::isValidPosition(olc::vi2d p) const
{
	return Verify::inExclusive(p, { 0, 0 }, getSize());
}

bool World::isSolidBlock(olc::vi2d p) const
{
	assert(isValidPosition(p));
	return !getBlock(p).isReplaceable();
}

int World::addEntity(std::unique_ptr<Entity> entity)
{
	const int id = entity->getId();
	entities[id] = std::move(entity);
	return id;
}

std::optional<std::reference_wrapper<Entity>> World::getEntity(int id) const
{
	if (const auto it = entities.find(id); it != entities.end())
		return std::ref(*it->second);
	return {};
}

std::optional<std::reference_wrapper<Player>> World::getPlayer() const
{
	for (auto& [id, entity] : entities)
	{
		if (Player* player = dynamic_cast<Player*>(entity.get()))
		{
			if (player != nullptr)
				return std::ref(*player);
			return {};
		}
	}
	return {};
}

void World::update(float elapsedTime)
{
	for (auto& [id, entity] : entities)
	{
		entity->update(*this, elapsedTime);
	}
	std::erase_if(entities, [](const auto& pair) { return pair.second->isDead(); });
}

void World::draw(OopMineGame& game)
{
	auto& view = game.getView();

	// TODO: This sometimes loops indefinitely when regenerating world while zoomed out 
	const olc::vi2d tl = view.GetTopLeftTile().max({ 0,0 });
	const olc::vi2d br = view.GetBottomRightTile().min(getSize() - 1);
	for (const auto& pos : Iterate::over(tl, br))
	{
		const Block& block = getBlock(pos);
		if (auto& asset = game.getBlockAssetPatch(block.getId()); asset.has_value())
		{
			drawBufPos.emplace_back(view.WorldToScreen(olc::vi2d{ pos.x, pos.y + 1 }));
			drawBufPos.emplace_back(view.WorldToScreen(pos));
			drawBufPos.emplace_back(view.WorldToScreen(olc::vi2d{ pos.x + 1, pos.y }));
			drawBufUv.emplace_back(asset->coords[0]);
			drawBufUv.emplace_back(asset->coords[1]);
			drawBufUv.emplace_back(asset->coords[2]);
			drawBufPos.emplace_back(view.WorldToScreen(olc::vi2d{ pos.x, pos.y + 1 }));
			drawBufPos.emplace_back(view.WorldToScreen(olc::vi2d{ pos.x + 1, pos.y }));
			drawBufPos.emplace_back(view.WorldToScreen(olc::vi2d{ pos.x + 1, pos.y + 1 }));
			drawBufUv.emplace_back(asset->coords[0]);
			drawBufUv.emplace_back(asset->coords[2]);
			drawBufUv.emplace_back(asset->coords[3]);
		}
	}
	game.SetDecalStructure(olc::DecalStructure::LIST);
	game.DrawPolygonDecal(game.getAsset("atlas").value().Decal(), drawBufPos, drawBufUv, olc::WHITE);
	game.SetDecalStructure(olc::DecalStructure::FAN);
	drawBufPos.clear();
	drawBufUv.clear();

	for (auto& [id, entity] : entities)
	{
		entity->draw(game);
	}
}

void World::generateWorld()
{
	// Origin is top left!

	assert(settings.noiseFrequency >= 0.0f);
	assert(settings.terrainHeightMin <= settings.terrainHeightMax);
	assert(settings.copperThresholdMin <= settings.copperThresholdMax);

	auto begin = std::chrono::steady_clock::now();

	// Place blocks
	for (int x = 0; x < getSize().x; x++)
	{
		const float terrainHeightNoiseY = 8192.0f;
		const float sample = sampleAt(x, terrainHeightNoiseY);
		const int terrainHeight = settings.terrainHeightMin - (int)(sample * (settings.terrainHeightMax - settings.terrainHeightMin));
		int y = terrainHeight;
		setBlock({ x, y++ }, Blocks::grassBlock);
		setBlock({ x, y++ }, Blocks::dirt);
		setBlock({ x, y++ }, Blocks::dirt);
		setBlock({ x, y++ }, Blocks::dirt);
		while (y < getSize().y)
		{
			if (sampleAt(x + 1337, y + 1337) >= settings.diamondThreshold)
				setBlock({ x, y }, Blocks::diamondOre);
			else if (Verify::inExclusive(sampleAt(x + 420, y + 420), settings.copperThresholdMin, settings.copperThresholdMax))
				setBlock({ x, y }, Blocks::copperOre);
			else if (sampleAt(x, y) >= settings.coalThreshold)
				setBlock({ x, y }, Blocks::coalOre);
			else
				setBlock({ x, y }, Blocks::stone);
			y++;
		}
	}
	// Place trees
	const auto treePattern = []() -> const std::array<std::array<const Block*, 7>, 7>
		{
			const auto o = &Blocks::air;
			const auto S = &Blocks::oakLeaves;
			const auto H = &Blocks::oakLog;
			return
			{ {
				{o,o,o,S,o,o,o},
				{o,o,S,S,S,o,o},
				{o,S,S,H,S,S,o},
				{S,S,S,H,S,S,S},
				{S,S,S,H,S,S,S},
				{o,o,o,H,o,o,o},
				{o,o,o,H,o,o,o}
			} };
		}();
	for (int x = 0; x < getSize().x; x += 16)
	{
		const float treeNoiseY = 16384.0f;
		const int chunkSeed = (getSeed() * 73856093) ^ (x / 16 * 19349663);
		std::mt19937 rng(chunkSeed);
		const int tries = rng() % 8;
		for (int _ = 0; _ < tries; _++)
		{
			const int xsub = rng() % 16;
			const unsigned rand = rng();
			const unsigned sample = (unsigned)(
				(noise.GetNoise((float)(x + xsub), treeNoiseY) + 1) / 2.0f
				* std::numeric_limits<unsigned>::max());
			if (x + xsub < getSize().x && rand > sample)
			{
				int y = findTopmostBlock(x + xsub, Blocks::grassBlock);
				if (y == -1)
					continue;
				y--;
				const olc::vi2d patSize = { treePattern[0].size(), treePattern.size() };
				for (const olc::vi2d ppat : Iterate::over({ 0, 0 }, patSize))
				{
					const Block& block = *treePattern[ppat.y][ppat.x];
					if (block == Blocks::air)
						continue;
					olc::vi2d preal = { x + xsub, y };
					preal += ppat;
					preal.x -= treePattern[ppat.y].size() / 2;
					preal.y -= treePattern.size() - 1;
					if (isValidPosition(preal) && getBlock(preal).isReplaceable())
					{
						setBlock(preal, block);
					}
				}
			}
		}
	}

	// Place player
	const int y = findTopmostSolid(getSize().x / 2);
	addEntity<Player>(olc::vf2d{ getSize().x / 2.0f + 0.5f, (float)y });

	auto end = std::chrono::steady_clock::now();
	auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
	std::println("world gen finished in {}", dur);
}

float World::sampleAt(int x, int y)
{
	return (noise.GetNoise((float)x, (float)y) + 1) / 2.0f;
}

float World::sampleAt(olc::vi2d pos)
{
	return (noise.GetNoise((float)pos.x, (float)pos.y) + 1) / 2.0f;
}
