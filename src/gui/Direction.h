#pragma once

#include <olcPixelGameEngine.h>

namespace gui
{
class Direction
{
public:
	static const Direction horizontal;
	static const Direction vertical;

	Direction(olc::vi2d, olc::vi2d);
	olc::vi2d getVector() const;
	olc::vi2d getSecondaryVector() const;

	bool operator==(const Direction&) const = default;

private:
	olc::vi2d vector;
	olc::vi2d vector2;
};
} // namespace gui
