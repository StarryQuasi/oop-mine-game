#pragma once

#include "libs/olcPixelGameEngine.h"

namespace gui
{
struct MouseEvent
{
	olc::vi2d pos{};
	olc::vi2d globalPos{};
	uint32_t btn{};
	bool ctrlDown{};
	bool altDown{};
	bool shiftDown{};
};

struct MouseDragEvent : public MouseEvent
{
	olc::vi2d globalPosStart{};
	olc::vi2d elePosStart{};
};
} // namespace gui
