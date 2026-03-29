#ifndef GUI_FLOWCONTAINER_H
#define GUI_FLOWCONTAINER_H

#include "Container.h"
#include "Direction.h"

namespace gui
{
	class FlowContainer : public Container
	{
	public:
		FlowContainer(
			olc::vi2d s = olc::vi2d{ 10,10 },
			olc::vi2d p = olc::vi2d{ 0,0 },
			Anchor a = Anchor::topLeft,
			Anchor o = Anchor::topLeft);

		Direction getDirection() const;
		bool getAutoSizing() const;

		FlowContainer* setDirection(Direction v);
		FlowContainer* setAutoSizing(bool v);

		void updateLayout() override;

	private:
		Direction direction;
		bool autoSizing;
	};
}

#endif
