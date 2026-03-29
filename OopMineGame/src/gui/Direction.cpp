#include "Direction.h"

namespace gui
{
	const Direction Direction::horizontal = { {1,0}, {0,1} };
	const Direction Direction::vertical = { {0,1}, {1,0} };

	Direction::Direction(olc::vi2d v, olc::vi2d v2) :
		vector(v),
		vector2(v2)
	{}

	olc::vi2d Direction::getVector() const
	{
		return vector;
	}

	olc::vi2d Direction::getSecondaryVector() const
	{
		return vector2;
	}
}
