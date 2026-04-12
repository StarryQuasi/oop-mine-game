#include "TextContainer.h"
#include "OopMineGame.h"

namespace gui
{
TextContainer::TextContainer(Props props) :
	Container(props)
{
	if (props.text.has_value())
		text = props.text.value();
	if (props.scale.has_value())
		scale = props.scale.value();
}

std::string TextContainer::getText() const { return text; }

olc::vf2d TextContainer::getScale() const { return scale; }

TextContainer* TextContainer::setText(std::string v)
{
	if (text != v)
		invalidate();
	text = std::move(v);
	return this;
}

TextContainer* TextContainer::setScale(olc::vf2d v)
{
	if (scale != v)
		invalidate();
	scale = v;
	return this;
}

void TextContainer::updateLayout()
{
	validate();

	setSize((OopMineGame::instance->GetTextSize(text) * scale).ceil());

	Container::updateLayout();
}

void TextContainer::draw(OopMineGame& game) const
{
	Container::draw(game);
	game.DrawStringDecal(getAbsolutePos(), text, olc::WHITE, scale);
}
} // namespace gui
