#include "FlowContainer.h"
#include "Container.h"
#include "Direction.h"

namespace gui
{
FlowContainer::FlowContainer(Props props) :
	Container(props)
{
	direction = props.direction.value_or(Direction::horizontal);
	autoSizing = props.autoSizing.value_or(true);
}

Direction FlowContainer::getDirection() const { return direction; }

bool FlowContainer::getAutoSizing() const { return autoSizing; }

FlowContainer* FlowContainer::setDirection(Direction v)
{
	if (direction != v)
		invalidate();
	direction = v;
	return this;
}

FlowContainer* FlowContainer::setAutoSizing(bool v)
{
	if (autoSizing != v)
		invalidate();
	autoSizing = v;
	return this;
}

void FlowContainer::updateLayout()
{
	validate();

	if (autoSizing)
	{
		olc::vi2d main = {};
		olc::vi2d secondary = {};
		for (auto& child : getChildren())
		{
			main += (child->getDrawSize() + child->getMargin() * 2) *
					direction.getVector();
			secondary = secondary.max(
				(child->getDrawSize() + child->getMargin() * 2) *
				direction.getSecondaryVector());
		}
		setSize(main + secondary);

		olc::vi2d pos = {};
		for (auto& child : getChildren())
		{
			child->setPos(pos);
			pos += (child->getDrawSize() + child->getMargin() * 2) *
				   direction.getVector();
		}
	}

	Container::updateLayout();
}
} // namespace gui