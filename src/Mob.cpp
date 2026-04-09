#include "Mob.h"
#include "Entity.h"
#include "OopMineGame.h"
#include "Utils.h"
#include "World.h"
#include <algorithm>
#include <cassert>
#include <chrono>
#include <optional>
#include <queue>
#include <unordered_map>
#include <vector>

Mob::Mob(olc::vf2d pos, olc::vf2d size) :
	Entity(pos, size)
{
}

std::optional<olc::vi2d> Mob::getTargetPos() const { return targetPos; }

Mob* Mob::setTargetPos(std::optional<olc::vi2d> v)
{
	targetPos = v;
	return this;
}

void Mob::updateAi(World& world, float elapsed) {}

void Mob::findPath(World& world, float elapsed)
{
	if (getTargetPos() == std::nullopt)
		return;

	static const auto walkableDirs = {
		olc::vi2d{0, 1},   // Down
		olc::vi2d{-1, 0},  // Left
		olc::vi2d{1, 0},   // Right
		olc::vi2d{1, -1},  // Top right
		olc::vi2d{-1, -1}, // Top left
		olc::vi2d{1, 1},   // Btm right
		olc::vi2d{-1, 1},  // Btm left
	};
	static const auto canReach =
		[](World& world, olc::vi2d from, olc::vi2d to) -> bool
	{
		if (world.isValidPosition(from) && world.isValidPosition(to) &&
			!world.isSolidBlock(to) && world.isSolidBlock(to + olc::vi2d{0, 1}))
		{
			return true;
		}
		return false;
	};

	const olc::vi2d start = getPos() - olc::vf2d{0, 0.01f};
	const olc::vi2d target = getTargetPos().value() - olc::vf2d{0, 0.01f};
	std::unordered_map<olc::vi2d, olc::vi2d, vi2dHash> cameFromMap = {};
	cameFromMap[start] = start;
	std::queue<olc::vi2d> frontier = {};
	frontier.push(start);
	debugPathCandidates.clear();
	debugPathCandidates.push_back({start, PathCandidate::Kind::Valid});
	bool found = false;
	while (!frontier.empty())
	{
		const olc::vi2d front = frontier.front();
		frontier.pop();

		if (front == target)
		{
			found = true;
			break;
		}

		for (const olc::vi2d dir : walkableDirs)
		{
			const olc::vi2d next = front + dir;
			if (!cameFromMap.contains(next))
			{
				cameFromMap[next] = front;
				if (Utils::manhattanDist(start, next) <= 16 &&
					canReach(world, front, next))
				{
					frontier.push(next);
					debugPathCandidates.push_back(
						{next, PathCandidate::Kind::Valid});
				}
				else
				{
					debugPathCandidates.push_back(
						{next, PathCandidate::Kind::Invalid});
				}
			}
		}
	}
	std::vector<olc::vi2d> path = {};
	if (found)
	{
		olc::vi2d cur = target;
		while (cur != start)
		{
			path.push_back(cur);
			cur = cameFromMap[cur];
			debugPathCandidates.push_back({cur, PathCandidate::Kind::Final});
		}
		debugPathCandidates.push_back({cur, PathCandidate::Kind::Final});
		std::ranges::reverse(path);
	}
	cachedPath = std::move(path);
}

void Mob::update(World& world, float elapsed)
{
	if (std::chrono::steady_clock::now() - lastAiUpdateTime >=
		std::chrono::milliseconds(100))
	{
		lastAiUpdateTime = std::chrono::steady_clock::now();
		updateAi(world, elapsed);
		findPath(world, elapsed);
	}
	if (!targetPos.has_value())
	{
		debugPathCandidates.clear();
		cachedPath.clear();
	}
	if (targetPos.has_value() && !cachedPath.empty())
	{
		const olc::vf2d pos = getPos() - olc::vf2d{0, 0.01f};
		if ((cachedPath.front() - pos).mag() <= 0.25f)
			cachedPath.erase(cachedPath.begin());
		if (!cachedPath.empty())
		{
			const olc::vf2d dir =
				cachedPath.front() + olc::vf2d{0.5f, 1.0f} - pos;
			Input input = {};
			input.left = dir.x < 0;
			input.right = dir.x > 0;
			input.jump = dir.y < 0;
			setInput(input);
		}
	}
	OopMineGame::debugCallbacks.push_back(
		[path = debugPathCandidates](OopMineGame& game)
		{
			for (const auto& cand : path)
			{
				if (cand.kind == PathCandidate::Kind::Valid)
					game.getView().FillRectDecal(
						cand.pos, {1, 1}, olc::Pixel{0, 255, 0, 64});
				else if (cand.kind == PathCandidate::Kind::Invalid)
					game.getView().FillRectDecal(
						cand.pos, {1, 1}, olc::Pixel{255, 0, 0, 64});
			}
			for (const auto& cand : path)
			{
				if (cand.kind == PathCandidate::Kind::Final)
					game.getView().DrawRectDecal(
						cand.pos, {1, 1}, olc::Pixel{255, 255, 255, 128});
			}
		});
	Entity::update(world, elapsed);
}