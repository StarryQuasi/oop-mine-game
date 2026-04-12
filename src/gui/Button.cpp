#include <libs/olcPixelGameEngine.h>

#include "Button.h"

namespace gui
{
Button::Button(Props props) :
	FlowContainer(props)
{
	setAssetName(props.assetName.value_or("gui/button.png"));
	setPadding(props.padding.value_or({4, 2}));
	onMouseDown(
		[this](Container& me, const MouseEvent& e)
		{
			if (e.btn == olc::Mouse::LEFT)
				this->clickHandler(me);
			return true;
		});

	textEle = addChild<TextContainer>({.text = std::move(props.text)});
	textEle->setDebugName(std::format("button {} text", getId()));
}

std::string Button::getText() const { return textEle->getText(); }

Button* Button::setText(const std::string& v)
{
	if (getText() != v)
		invalidate();
	textEle->setText(v);
	return this;
}

Button* Button::onClick(std::function<void(Container&)> handler)
{
	clickHandler = std::move(handler);
	return this;
}
} // namespace gui
