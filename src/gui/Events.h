#pragma once

#include <olcPixelGameEngine.h>

namespace gui
{
struct MouseEvent
{
	olc::vi2d pos = {};
	olc::vi2d globalPos = {};
	uint32_t btn = 0;
	bool ctrlDown = 0;
	bool altDown = 0;
	bool shiftDown = 0;
};

struct MouseDragEvent : public MouseEvent
{
	olc::vi2d globalPosStart = {};
	olc::vi2d elePosStart = {};
};
} // namespace gui
