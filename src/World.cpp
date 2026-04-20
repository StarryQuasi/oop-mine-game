#include <bit>
#include <chrono>
#include <cstdarg>
#include <initializer_list>
#include <mdspan>
#include <print>
#include <random>
#include <ranges>

#include "Block.h"
#include "Blocks.h"
#include "Entity.h"
#include "OopMineGame.h"
#include "Player.h"
#include "Sheep.h"
#include "Utils.h"
#include "Verify.h"
#include "World.h"

World::World(OopMineGame& game, GenerationSettings settings) :
	game(&game),
	settings(settings),
	noise(settings.seed),
	blocksRaw(settings.size.area()),
	blocks(blocksRaw.data(), settings.size.x, settings.size.y),
	random(std::random_device()())
{
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	noise.SetFrequency(settings.noiseFrequency);
	generateWorld();
}

int World::randomInt(int min, int maxInclusive)
{
	return std::uniform_int_distribution<int>(min, maxInclusive)(random);
}

float World::randomFloat(float min, float maxExclusive)
{
	return std::uniform_real_distribution<float>(min, maxExclusive)(random);
}

OopMineGame& World::getGame() { return *game; }

olc::vi2d World::getSize() const { return settings.size; }

int World::getSeed() const { return settings.seed; }

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

void World::breakBlock(olc::vi2d p)
{
	assert(isValidPosition(p));
	const Block& block = getBlock(p);
	setBlock(p, Blocks::air);
	if (block != Blocks::air)
	{
		block.onBreak(*this, p);
	}
}

int World::findTopmostSolid(int x) const
{
	for (int y = 0; y < getSize().y; y++)
		if (getBlock({x, y}).isSolid())
			return y;
	return -1;
}

int World::findTopmostBlock(int x, const Block& block) const
{
	for (int y = 0; y < getSize().y; y++)
		if (getBlock({x, y}) == block)
			return y;
	return -1;
}

olc::vi2d World::findNiceSpawnPoint(int x, int radius) const
{
	for (const int x2 :
		 std::views::iota(0, radius + 1) |
			 std::views::transform([x](int i)
								   { return std::array{-i + x, i + x}; }) |
			 std::views::join | std::views::drop(1))
	{
		if (!isValidPosition({x2, 0}))
			continue;
		const int grassY = findTopmostBlock(x2, Blocks::grassBlock);
		if (!(isValidPosition({x2, grassY - 1}) &&
			  !getBlock({x2, grassY - 1}).isSolid() &&
			  !getBlock({x2, grassY - 2}).isSolid()))
			continue;
		return {x2, grassY};
	}
	return {getSize().x / 2, 0};
}

bool World::isValidPosition(olc::vi2d p) const
{
	return Verify::inExclusive(p, {0, 0}, getSize());
}

bool World::isSolidBlock(olc::vi2d p) const
{
	assert(isValidPosition(p));
	return getBlock(p).isSolid();
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
		if (Player* player = dynamic_cast<Player*>(entity.get());
			player != nullptr)
			return std::ref(*player);
	}
	return {};
}

void World::addParticle(Particle p) { particles.push_back(std::move(p)); }

void World::update(float elapsed)
{
	if (std::chrono::steady_clock::now() - lastRandomUpdate >=
		std::chrono::milliseconds(1000 / 20))
	{
		lastRandomUpdate = std::chrono::steady_clock::now();
		randomUpdate(elapsed);
	}
	for (auto& [id, entity] : entities)
	{
		entity->update(*this, elapsed);
	}
	drawUpdate(elapsed);
	updateParticles(elapsed);
	std::erase_if(
		entities, [](const auto& pair) { return pair.second->isDead(); });
}

void World::randomUpdate(float elapsed) {}

void World::drawUpdate(float elapsed)
{
	const int radius = 32;
	const auto& player = getPlayer()->get();
	for (int _ = 0; _ < 200; _++)
	{
		const olc::vi2d pos = {
			(int)player.getX() + randomInt(-radius, radius),
			(int)player.getY() + randomInt(-radius, radius)};
		if (isValidPosition(pos) && getBlock(pos).requiresDrawUpdate())
			getBlock(pos).drawUpdate(*this, pos);
	}
}

void World::updateParticles(float elapsed)
{
	const auto now = std::chrono::steady_clock::now();
	std::erase_if(
		particles, [now](const Particle& p) { return now >= p.lifeEnd; });
	for (Particle& p : particles)
	{
		p.pos += p.vel * elapsed;
	}
}

void World::drawParticles(OopMineGame& game)
{
	auto& view = game.getView();
	const auto now =
		std::chrono::steady_clock::now().time_since_epoch().count();
	const size_t capBefore = drawBufPos.capacity();
	for (const Particle& p : particles)
	{
		const float scale = Utils::map(
			(float)now,
			(float)p.lifeStart.time_since_epoch().count(),
			(float)p.lifeEnd.time_since_epoch().count(),
			1.0f,
			0.25f);
		const olc::vf2d size = olc::vf2d{1, 1} / 32.0f * p.scale * scale;
		const olc::vf2d tl = view.WorldToScreen(p.pos - size / 2.0f);
		const olc::vf2d br = view.WorldToScreen(p.pos + size / 2.0f);
		const olc::vf2d ss = br - tl; // Size in screen space
		olc::Pixel color = p.color;
		color.a = color.a * scale;
		drawBufPos.emplace_back(tl.x, tl.y + ss.y);
		drawBufPos.emplace_back(tl.x, tl.y);
		drawBufPos.emplace_back(tl.x + ss.x, tl.y);
		drawBufUv.emplace_back(0, 0);
		drawBufUv.emplace_back(0, 0);
		drawBufUv.emplace_back(0, 0);
		drawBufColor.emplace_back(color);
		drawBufColor.emplace_back(color);
		drawBufColor.emplace_back(color);
		drawBufPos.emplace_back(tl.x, tl.y + ss.y);
		drawBufPos.emplace_back(tl.x + ss.x, tl.y);
		drawBufPos.emplace_back(tl.x + ss.x, tl.y + ss.y);
		drawBufUv.emplace_back(0, 0);
		drawBufUv.emplace_back(0, 0);
		drawBufUv.emplace_back(0, 0);
		drawBufColor.emplace_back(color);
		drawBufColor.emplace_back(color);
		drawBufColor.emplace_back(color);
	}
	if (drawBufPos.capacity() > capBefore)
		std::println(
			"Vertex buffer size increased from {} to {}",
			capBefore,
			drawBufPos.capacity());
	game.SetDecalStructure(olc::DecalStructure::LIST);
	game.DrawPolygonDecal(nullptr, drawBufPos, drawBufUv, drawBufColor);
	game.SetDecalStructure(olc::DecalStructure::FAN);
	drawBufPos.clear();
	drawBufUv.clear();
	drawBufColor.clear();
}

void World::draw(OopMineGame& game)
{
	auto& view = game.getView();

	const olc::vi2d tl = view.GetTopLeftTile().max({0, 0});
	const olc::vi2d br = view.GetBottomRightTile().min(getSize());
	const size_t capBefore = drawBufPos.capacity();
	for (const auto& p : Iterate::over(tl, br))
	{
		const Block& block = getBlock(p);
		if (auto& asset = game.getBlockAssetPatch(block.getId());
			asset.has_value())
		{
			const olc::vf2d size = olc::vf2d{1, 1};
			const olc::vf2d tl = view.WorldToScreen(p);
			const olc::vf2d br = view.WorldToScreen(p + size);
			const olc::vf2d ss = br - tl; // Size in screen space
			drawBufPos.emplace_back(tl.x, tl.y + ss.y);
			drawBufPos.emplace_back(tl.x, tl.y);
			drawBufPos.emplace_back(tl.x + ss.x, tl.y);
			drawBufUv.emplace_back(asset->coords[0]);
			drawBufUv.emplace_back(asset->coords[1]);
			drawBufUv.emplace_back(asset->coords[2]);
			drawBufPos.emplace_back(tl.x, tl.y + ss.y);
			drawBufPos.emplace_back(tl.x + ss.x, tl.y);
			drawBufPos.emplace_back(tl.x + ss.x, tl.y + ss.y);
			drawBufUv.emplace_back(asset->coords[0]);
			drawBufUv.emplace_back(asset->coords[2]);
			drawBufUv.emplace_back(asset->coords[3]);
		}
	}
	if (drawBufPos.capacity() > capBefore)
		std::println(
			"Vertex buffer size increased from {} to {}",
			capBefore,
			drawBufPos.capacity());
	game.SetDecalStructure(olc::DecalStructure::LIST);
	game.DrawPolygonDecal(
		game.getAsset("atlas").value().Decal(),
		drawBufPos,
		drawBufUv,
		olc::WHITE);
	game.SetDecalStructure(olc::DecalStructure::FAN);
	drawBufPos.clear();
	drawBufUv.clear();

	for (auto& [id, entity] : entities)
	{
		entity->draw(game);
	}

	drawParticles(game);
}

void World::generateWorld()
{
	// Origin is top left!

	assert(settings.noiseFrequency >= 0.0f);
	assert(settings.terrainHeightMin <= settings.terrainHeightMax);
	assert(settings.copperThresholdMin <= settings.copperThresholdMax);

	auto begin = std::chrono::steady_clock::now();

	FastNoiseLite noise2;
	noise2.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	noise2.SetFrequency(settings.noiseFrequency * 4);

	// Place blocks
	for (int x = 0; x < getSize().x; x++)
	{
		const float terrainHeightNoiseY = 8192.0f;
		const float sample = (sampleAt(x, terrainHeightNoiseY) +
							  sampleAt(noise2, x, terrainHeightNoiseY)) /
							 2.0f;
		const int terrainHeight = settings.terrainHeightMin -
								  (int)(sample * (settings.terrainHeightMax -
												  settings.terrainHeightMin));
		int y = terrainHeight;
		setBlock({x, y++}, Blocks::grassBlock);
		setBlock({x, y++}, Blocks::dirt);
		setBlock({x, y++}, Blocks::dirt);
		setBlock({x, y++}, Blocks::dirt);
		while (y < getSize().y)
		{
			if (sampleAt(x + 1337, y + 1337) >= settings.diamondThreshold)
				setBlock({x, y}, Blocks::diamondOre);
			else if (Verify::inExclusive(
						 sampleAt(x + 420, y + 420),
						 settings.copperThresholdMin,
						 settings.copperThresholdMax))
				setBlock({x, y}, Blocks::copperOre);
			else if (sampleAt(x, y) >= settings.coalThreshold)
				setBlock({x, y}, Blocks::coalOre);
			else
				setBlock({x, y}, Blocks::stone);
			y++;
		}
	}
	// Place trees
	const auto treePatterns = makeTreePatterns();
	for (int x = 0; x < getSize().x; x += 16)
	{
		const float treeNoiseY = 16384.0f;
		const int chunkSeed = (getSeed() * 73856093) ^ (x / 16 * 19349663);
		// Avoid using std::uniform_int_distribution here because I think it
		// might call rng multiple times and become unpredictable
		std::mt19937 rng(chunkSeed);
		const int tries = rng() % 8;
		for (int _ = 0; _ < tries; _++)
		{
			const int xsub = rng() % 16;
			const int treeType = rng() % treePatterns.size();
			const float rand = randToFloat(rng());
			const float sample = sampleAt((float)x + xsub, treeNoiseY);
			if (x + xsub < getSize().x && rand > sample)
			{
				int y = findTopmostBlock(x + xsub, Blocks::grassBlock);
				if (y == -1)
					continue;
				y--;
				const olc::vi2d patSize = {
					(int)treePatterns[treeType][0].size(),
					(int)treePatterns[treeType].size()};
				const olc::vi2d patOffsetFromMid = {
					-patSize.x / 2, -(patSize.y - 1)};
				for (const olc::vi2d ppat : Iterate::over({0, 0}, patSize))
				{
					const Block& block =
						*treePatterns[treeType][ppat.y][ppat.x];
					if (block == Blocks::air)
						continue;
					olc::vi2d preal = {x + xsub, y};
					preal += ppat;
					preal += patOffsetFromMid;
					if (isValidPosition(preal) &&
						getBlock(preal).isReplaceable())
					{
						setBlock(preal, block);
					}
				}
			}
		}
	}

	// Place player
	const olc::vi2d nicePoint = findNiceSpawnPoint(getSize().x / 2);
	addEntity<Player>(olc::vf2d{nicePoint.x + 0.5f, (float)nicePoint.y});

	// Place animals
	addEntity<Sheep>(getPlayer()->get().getPos());

	auto end = std::chrono::steady_clock::now();
	auto dur =
		std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
	std::println("World gen finished in {}", dur);
}

float World::sampleAt(int x, int y)
{
	return (noise.GetNoise((float)x, (float)y) + 1) / 2.0f;
}

float World::sampleAt(FastNoiseLite& noise, int x, int y)
{
	return (noise.GetNoise((float)x, (float)y) + 1) / 2.0f;
}

float World::sampleAt(olc::vi2d pos)
{
	return (noise.GetNoise((float)pos.x, (float)pos.y) + 1) / 2.0f;
}

float World::randToFloat(unsigned v)
{
	// https://blog.bithole.dev/blogposts/random-float/
	return std::bit_cast<float>(v >> 9 | 0x3f800000) - 1.0f;
}

std::vector<std::vector<std::vector<const Block*>>> World::makeTreePatterns()
{
	const auto makePattern =
		[](const std::initializer_list<std::pair<char, const Block&>> mappings,
		   const std::initializer_list<std::string_view> pattern)
	{
		std::vector<std::vector<const Block*>> res;
		std::unordered_map<char, const Block*> map;
		map[' '] = &Blocks::air;
		for (const auto& [c, block] : mappings)
			map[c] = &block;
		for (const auto& row : pattern)
		{
			std::vector<const Block*> resultRow;
			for (const char c : row)
				resultRow.push_back(map.at(c));
			res.push_back(std::move(resultRow));
			assert(res.back().size() == res.front().size());
		}
		return res;
	};
	// spell-checker:disable
	std::vector<std::vector<std::vector<const Block*>>> treePatterns;
	treePatterns.push_back(makePattern(
		{{'S', Blocks::oakLeaves}, {'H', Blocks::oakLog}},
		{"   S   ",
		 "  SSS  ",
		 " SSHSS ",
		 "SSSHSSS",
		 "SSSHSSS",
		 "   H   ",
		 "   H   "}));
	treePatterns.push_back(makePattern(
		{{'S', Blocks::cherryLeaves}, {'H', Blocks::cherryLog}},
		{"    SSSSS    ",
		 "  SSSSSSSSS  ",
		 " SSSSSSSSSHS ",
		 "SSSHSSSHHHSSS",
		 " SSSHHHHSSSS ",
		 "  SS  H  SS  ",
		 "      H      ",
		 "      H      "}));
	// spell-checker:enable
	return treePatterns;
}
