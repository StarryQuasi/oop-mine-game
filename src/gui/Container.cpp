#include <ranges>

#include "Container.h"
#include "OopMineGame.h"
#include "Verify.h"

namespace gui
{
bool Container::debug = false;
// bool Container::debug = true;
int Container::idCounter = 0;
int Container::focusedElementId = -1;

Container::Container(Props props) :
	id(idCounter++)
{
	if (props.size.has_value())
		size = props.size.value();
	if (props.pos.has_value())
		pos = props.pos.value();
	if (props.anchor.has_value())
		anchor = props.anchor.value();
	if (props.origin.has_value())
		origin = props.origin.value();
	if (props.margin.has_value())
		margin = props.margin.value();
	if (props.padding.has_value())
		padding = props.padding.value();
	if (props.visible.has_value())
		visible = props.visible.value();
	if (props.assetName.has_value())
		assetName = props.assetName.value();
}

const std::string& Container::getDebugName() const
{
#ifdef _DEBUG
	return name;
#else
	static const std::string empty = "";
	return empty;
#endif
}

int Container::getId() const { return id; }

olc::vi2d Container::getSize() const { return size; }

olc::vi2d Container::getDrawSize() const { return getSize() + padding * 2; }

olc::vi2d Container::getPos() const { return pos; }

Anchor Container::getAnchor() const { return anchor; }

Anchor Container::getOrigin() const { return origin; }

olc::vi2d Container::getMargin() const { return margin; }

olc::vi2d Container::getPadding() const { return padding; }

bool Container::isVisible() const { return visible; }

Container* Container::getParent() const { return parent; }

olc::vi2d Container::getAbsolutePos() const { return absPos; }

Container* Container::setDebugName(std::string v)
{
#ifdef _DEBUG
	name = std::move(v);
#endif
	return this;
}

Container* Container::setSize(olc::vi2d v)
{
	assert(Verify::elemsNonNeg(v));
	if (size != v && debug)
		std::println(
			"{} size set to {},{}",
			getDebugName().empty() ? typeid(*this).name() : getDebugName(),
			v.x,
			v.y);
	if (size != v)
		invalidate();
	size = v;
	return this;
}

Container* Container::setPos(olc::vi2d v)
{
	if (pos != v && debug)
		std::println(
			"{} pos set to {},{}",
			getDebugName().empty() ? typeid(*this).name() : getDebugName(),
			v.x,
			v.y);
	if (pos != v)
		invalidate();
	pos = v;
	return this;
}

Container* Container::setAnchor(Anchor v)
{
	if (anchor != v)
		invalidate();
	anchor = v;
	return this;
}

Container* Container::setOrigin(Anchor v)
{
	if (origin != v)
		invalidate();
	origin = v;
	return this;
}

Container* Container::setMargin(olc::vi2d v)
{
	assert(Verify::elemsNonNeg(v));
	if (margin != v)
		invalidate();
	margin = v;
	return this;
}

Container* Container::setPadding(olc::vi2d v)
{
	assert(Verify::elemsNonNeg(v));
	if (padding != v)
		invalidate();
	padding = v;
	return this;
}

Container* Container::setAssetName(std::string v)
{
	assetName = std::move(v);
	return this;
}

Container* Container::setVisible(bool v)
{
	if (visible != v && debug)
		std::println(
			"{} visible set to {}",
			getDebugName().empty() ? typeid(*this).name() : getDebugName(),
			v);
	visible = v;
	return this;
}

void Container::setAbsolutePos(olc::vi2d v)
{
	// Absolute pos is the outcome of layout, so should not invalidate
	if (absPos != v && debug)
		std::println(
			"{} absPos set to {},{}",
			getDebugName().empty() ? typeid(*this).name() : getDebugName(),
			v.x,
			v.y);
	absPos = v;
}

void Container::removeChild(int id)
{
	auto it = std::ranges::find(
		children,
		id,
		[id](const std::unique_ptr<Container>& child)
		{ return child->getId(); });
	assert(it != children.end());
	children.erase(it);
}

std::vector<std::unique_ptr<Container>>& Container::getChildren()
{
	return children;
}

Container* Container::searchElement(int id)
{
	if (getId() == id)
		return this;
	for (auto& child : children)
		if (Container* p = child->searchElement(id); p != nullptr)
			return p;
	return nullptr;
}

Container* Container::searchElement(olc::vi2d pos)
{
	if (!isPointInside(pos))
		return nullptr;
	pos = parentToLocal(pos);
	for (auto& child : children | std::views::reverse)
		if (Container* p = child->searchElement(pos); p != nullptr)
			return p;
	return this;
}

int Container::getFocusedElementId() { return focusedElementId; }

void Container::setFocusedElementId(int id)
{
	if (debug)
		std::println("focus changed from {} to {}", focusedElementId, id);
	focusedElementId = id;
}

void Container::setDebug(bool v) { debug = v; }

olc::vi2d Container::calcAbsolutePos() const
{
	if (parent)
		return localToParent({}) + parent->getAbsolutePos();
	return localToParent({});
}

olc::vi2d Container::parentToLocal(olc::vi2d p) const
{
	return p - localToParent({});
}

olc::vi2d Container::localToParent(olc::vi2d p) const
{
	p += pos + margin;
	p -= origin.apply(size + padding * 2).floor();
	if (parent)
		p += parent->padding + anchor.apply(parent->size).floor();
	return p;
}

olc::vi2d Container::screenToLocal(olc::vi2d p) const
{
	return p - getAbsolutePos();
}

olc::vi2d Container::localToScreen(olc::vi2d p) const
{
	return p + getAbsolutePos();
}

bool Container::isPointInside(olc::vi2d parentSpace) const
{
	const olc::vi2d local = parentToLocal(parentSpace);
	return Verify::inExclusive(local, {0, 0}, getDrawSize());
}

bool Container::mouseDown(const MouseEvent& e)
{
	MouseEvent local = e;
	local.pos =
		parent ? parent->screenToLocal(local.globalPos) : local.globalPos;
	if (!isPointInside(local.pos))
		return false;
	if (debug)
		std::println(
			"mouse down {}({},{},{},{})",
			getDebugName().empty() ? typeid(*this).name() : getDebugName(),
			getAbsolutePos().x,
			getAbsolutePos().y,
			getSize().x,
			getSize().y);
	for (auto& child : children | std::views::reverse)
		if (child->mouseDown(local))
			return true;
	if (mouseDownHandler && mouseDownHandler(*this, local))
	{
		if (local.btn == olc::Mouse::LEFT)
			setFocusedElementId(getId());
		return true;
	}
	return false;
}

bool Container::mouseDrag(const MouseDragEvent& e)
{
	assert(getFocusedElementId() == getId());
	MouseDragEvent local = e;
	local.pos =
		parent ? parent->screenToLocal(local.globalPos) : local.globalPos;
	if (getId() != getFocusedElementId())
		return false;
	if (mouseDragHandler && mouseDragHandler(*this, local))
		return true;
	return false;
}

bool Container::mouseUp(const MouseEvent& e)
{
	MouseEvent local = e;
	local.pos =
		parent ? parent->screenToLocal(local.globalPos) : local.globalPos;
	if (!isPointInside(local.pos))
		return false;
	for (auto& child : children | std::views::reverse)
		if (child->mouseUp(local))
			return true;
	if (mouseUpHandler && mouseUpHandler(*this, local))
		return true;
	return false;
}

void Container::onMouseDown(
	std::function<bool(Container&, const MouseEvent&)> handler)
{
	mouseDownHandler = std::move(handler);
}

void Container::onMouseDrag(
	std::function<bool(Container&, const MouseDragEvent&)> handler)
{
	mouseDragHandler = std::move(handler);
}

void Container::onMouseUp(
	std::function<bool(Container&, const MouseEvent&)> handler)
{
	mouseUpHandler = std::move(handler);
}

bool Container::needsLayout() const { return dirty; }

void Container::invalidate(bool propagate)
{
	// if (debug)
	//	std::println("{} invalidated", getId());
	dirty = true;
	if (propagate && parent)
		parent->invalidate(propagate);
}

void Container::validate()
{
	// if (debug)
	//	std::println("{} validated", getId());
	dirty = false;
}

void Container::updateLayout()
{
	// When Container::updateLayout() is invoked, it validates self (clears
	// the dirty flag) and calculates its absolute pos,
	// same logic cascades down to all children
	//
	// When an autosizing FlowContainer::updateLayout() is invoked:
	// 1st pass: validates self, calculates the required parent size to fit,
	// sets self size and its absolute pos; for all children, sets their pos,
	// they propagate invalidation upwards, then their absolute pos are
	// calculated in Container::updateLayout() like above
	// 2nd pass: same as above, but nothing is invalidated, now the validation
	// loop ends

	validate();

	setAbsolutePos(calcAbsolutePos());

	for (auto& child : children)
		child->updateLayout();
}

void Container::update(OopMineGame& game)
{
	for (auto& child : children)
		child->update(game);
}

void Container::draw(OopMineGame& game) const
{
	if (!isVisible())
		return;

	if (!assetName.empty())
	{
		const auto decal = game.getAsset(assetName).value().Decal();
		const olc::vi2d assetSize = {decal->width, decal->height};
		game.DrawDecal(
			getAbsolutePos(), decal, (olc::vf2d)getDrawSize() / assetSize);
	}

	for (auto& child : children)
		child->draw(game);

	if (debug)
	{
		if (margin != olc::vi2d())
		{
			const olc::Pixel pixel = 0xC0FF0000;
			const auto pos = getAbsolutePos();
			const auto s = getDrawSize();
			const auto m = getMargin();
			game.FillRectDecal(pos - m, olc::vi2d{s.x + m.x * 2, m.y}, pixel);
			game.FillRectDecal(
				pos - m + olc::vi2d{0, m.y + s.y},
				olc::vi2d{s.x + m.x * 2, m.y},
				pixel);
			game.FillRectDecal(
				pos - m + olc::vi2d{0, m.y}, olc::vi2d{m.x, s.y}, pixel);
			game.FillRectDecal(
				pos - m + olc::vi2d{m.x + s.x, m.y},
				olc::vi2d{m.x, s.y},
				pixel);
		}
		if (padding != olc::vi2d())
		{
			const olc::Pixel pixel = 0xC000FF00;
			const auto pos = getAbsolutePos();
			const auto s = getDrawSize();
			const auto p = getPadding();
			game.FillRectDecal(pos, olc::vi2d{s.x, p.y}, pixel);
			game.FillRectDecal(
				pos + olc::vi2d{0, s.y - p.y}, olc::vi2d{s.x, p.y}, pixel);
			game.FillRectDecal(
				pos + olc::vi2d{0, p.y}, olc::vi2d{p.x, s.y - p.y * 2}, pixel);
			game.FillRectDecal(
				pos + olc::vi2d{s.x - p.x, p.y},
				olc::vi2d{p.x, s.y - p.y * 2},
				pixel);
		}
		game.DrawRectDecal(
			getAbsolutePos(), getDrawSize(), olc::Pixel(0xC0FFFFFF));
	}
}
} // namespace gui
