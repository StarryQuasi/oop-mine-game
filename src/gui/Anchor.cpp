#include "Anchor.h"

namespace gui
{
const Anchor Anchor::topLeft = {{0.0f, 0.0f}};
const Anchor Anchor::topMiddle = {{0.5f, 0.0f}};
const Anchor Anchor::topRight = {{1.0f, 0.0f}};
const Anchor Anchor::midLeft = {{0.0f, 0.5f}};
const Anchor Anchor::midMiddle = {{0.5f, 0.5f}};
const Anchor Anchor::midRight = {{1.0f, 0.5f}};
const Anchor Anchor::btmLeft = {{0.0f, 1.0f}};
const Anchor Anchor::btmMiddle = {{0.5f, 1.0f}};
const Anchor Anchor::btmRight = {{1.0f, 1.0f}};

Anchor::Anchor(olc::vf2d value) :
	value(value)
{
}

olc::vf2d Anchor::apply(olc::vf2d size) const { return size * value; }

std::string Anchor::toString() const
{
	if (*this == topLeft)
		return "topLeft";
	if (*this == topMiddle)
		return "topMiddle";
	if (*this == topRight)
		return "topRight";
	if (*this == midLeft)
		return "midLeft";
	if (*this == midMiddle)
		return "midMiddle";
	if (*this == midRight)
		return "midRight";
	if (*this == btmLeft)
		return "btmLeft";
	if (*this == btmMiddle)
		return "btmMiddle";
	if (*this == btmRight)
		return "btmRight";
	return std::format("{{{}, {}}}", value.x, value.y);
}
} // namespace gui
