#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

#include "Container.h"
#include "FlowContainer.h"
#include "TextContainer.h"

namespace gui
{
	class Button : public FlowContainer
	{
	public:
		Button(const std::string& text);

		std::string getText() const;

		Button* setText(const std::string& v);

		Button* onClick(std::function<void(Container&)> handler);

	private:
		TextContainer* textEle;
		std::function<void(Container&)> clickHandler = {};
	};
}

#endif
