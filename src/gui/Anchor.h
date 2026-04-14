#pragma once

#include <string>

#include "libs/olcPixelGameEngine.h"

namespace gui
{
class Anchor
{
public:
	static const Anchor topLeft;
	static const Anchor topMiddle;
	static const Anchor topRight;
	static const Anchor midLeft;
	static const Anchor midMiddle;
	static const Anchor midRight;
	static const Anchor btmLeft;
	static const Anchor btmMiddle;
	static const Anchor btmRight;

	Anchor(olc::vf2d);
	olc::vf2d apply(olc::vf2d size) const;
	std::string toString() const;

	bool operator==(const Anchor&) const = default;

private:
	olc::vf2d value;
};
} // namespace gui
