#pragma once

#include "Container.h"
#include "FlowContainer.h"
#include "TextContainer.h"

namespace gui
{
class Button : public FlowContainer
{
public:
	Button(Props props = {});

	std::string getText() const;

	Button* setText(const std::string& v);

	Button* onClick(std::function<void(Container&)> handler);

private:
	TextContainer* textEle{};
	std::function<void(Container&)> clickHandler{};
};
} // namespace gui
