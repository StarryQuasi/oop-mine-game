#pragma once

#include "Container.h"
#include "Direction.h"

namespace gui
{
	class FlowContainer : public Container
	{
	public:
		FlowContainer();

		Direction getDirection() const;
		bool getAutoSizing() const;

		FlowContainer* setDirection(Direction v);
		FlowContainer* setAutoSizing(bool v);

		void updateLayout() override;

	private:
		Direction direction = Direction::horizontal;
		bool autoSizing = true;
	};
}
