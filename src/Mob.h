#pragma once

#include "Entity.h"
#include <chrono>
#include <optional>

class Mob : public Entity
{
public:
	struct PathCandidate
	{
		enum class Kind
		{
			Valid, Invalid, Final
		};
		olc::vi2d pos = {};
		Kind kind = Kind::Invalid;
	};

	Mob(olc::vf2d pos, olc::vf2d size);

	std::optional<olc::vi2d> getTargetPos() const;

	virtual void updateAi(World& world, float elapsed);
	void findPath(World& world, float elapsed);
	void update(World& world, float elapsed) override;

protected:
	Mob* setTargetPos(std::optional<olc::vi2d> v);

private:
	std::optional<olc::vi2d> targetPos = {};
	std::chrono::steady_clock::time_point lastAiUpdateTime = {};
	std::vector<olc::vi2d> cachedPath = {};
	std::vector<PathCandidate> debugPathCandidates = {};
};
