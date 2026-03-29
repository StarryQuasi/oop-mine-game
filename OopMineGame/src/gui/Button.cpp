#include <olcPixelGameEngine.h>

#include "Button.h"

namespace gui
{
	Button::Button(const std::string& text) :
		FlowContainer(),
		textEle(nullptr),
		clickHandler()
	{
		textEle = addChild<TextContainer>(text);
		textEle->setDebugName(std::format("button {} text", getId()));
		onMouseDown([this](Container& me, const MouseEvent& e)
			{
				if (e.btn == olc::Mouse::LEFT)
					this->clickHandler(me);
				return true;
			});
		setAssetName("gui/button.png");
		setPadding({ 4, 2 });
	}

	std::string Button::getText() const
	{
		return textEle->getText();
	}

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
}
