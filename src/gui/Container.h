#pragma once
#include <memory>
																																					
#include <optional>
#include <vector>

#include "Anchor.h"
#include "Direction.h"
#include "Events.h"

class OopMineGame;

namespace gui
{
struct Props
{
	std::optional<olc::vi2d> size = {};
	std::optional<olc::vi2d> pos = {};
	std::optional<Anchor> anchor = {};
	std::optional<Anchor> origin = {};
	std::optional<olc::vi2d> margin = {};
	std::optional<olc::vi2d> padding = {};
	std::optional<bool> visible = {};
	std::optional<std::string> assetName = {};
	std::optional<std::string> text = {};	 // TextContainer
	std::optional<float> scale = {};	     // TextContainer
	std::optional<Direction> direction = {}; // FlowContainer
	std::optional<olc::Pixel> color = {};	 // TextContainer
	std::optional<bool> autoSizing = {};	 // FlowContainer
};

// The base gui component that hosts child elements, can be positioned
// relatively to its parent, performs layout updates, and handles mouse events.
// Also equipped with various layout properties in fluent interface:
//   `setPos(olc::vi2d)` - sets position.
//   `setSize(olc::vi2d)` - sets size of the inner child area.
//   `setAnchor(gui::Anchor)` - sets origin on parent.
//   `setOrigin(gui::Anchor)` - sets origin of self.
//   `setMargin(olc::vi2d)` - sets margin between self and its parent, not
//   related to draw size, but contributes to layout size.
//   `setPadding(olc::vi2d)` - sets padding between self and the inner child
//   area, expands the draw size by padding * 2.
// 	 `addChild<gui::Container>()` - adds child to it.
class Container
{
public:
	Container(Props props = {});
	virtual ~Container() = default;

	const std::string& getDebugName() const;
	int getId() const;
	olc::vi2d getSize() const;
	olc::vi2d getDrawSize() const;
	olc::vi2d getPos() const;
	olc::vi2d getAbsolutePos() const;
	Anchor getAnchor() const;
	Anchor getOrigin() const;
	olc::vi2d getMargin() const;
	olc::vi2d getPadding() const;
	bool isVisible() const;
	Container* getParent() const;

	Container* setDebugName(std::string v);
	Container* setSize(olc::vi2d v);
	Container* setPos(olc::vi2d v);
	Container* setAnchor(Anchor v);
	Container* setOrigin(Anchor v);
	Container* setMargin(olc::vi2d v);
	Container* setPadding(olc::vi2d v);
	Container* setAssetName(std::string v);
	Container* setVisible(bool v);

	template <typename T, typename... Args>
		requires std::derived_from<T, Container>
	T* addChild();
	template <typename T, typename... Args>
		requires std::derived_from<T, Container>
	T* addChild(Props props, Args&&... args);
	template <typename T>
		requires std::derived_from<T, Container>
	T* adoptChild(std::unique_ptr<T> v);
	void removeChild(int id);
	std::vector<std::unique_ptr<Container>>& getChildren();

	Container* searchElement(int id);
	Container* searchElement(olc::vi2d pos);
	static int getFocusedElementId();
	static void setFocusedElementId(int id);

	static void setDebug(bool v);

	olc::vi2d calcAbsolutePos() const;
	olc::vi2d parentToLocal(olc::vi2d p) const;
	olc::vi2d localToParent(olc::vi2d p) const;
	olc::vi2d screenToLocal(olc::vi2d p) const;
	olc::vi2d localToScreen(olc::vi2d p) const;
	bool isPointInside(olc::vi2d parentSpace) const;

	bool mouseDown(const MouseEvent& e);	 // Returns whether it was handled
	bool mouseDrag(const MouseDragEvent& e); // Returns whether it was handled
	bool mouseUp(const MouseEvent& e);		 // Returns whether it was handled
	void onMouseDown(
		std::function<bool(Container&, const MouseEvent&)>
			handler); // Returns whether it was handled
	void onMouseDrag(
		std::function<bool(Container&, const MouseDragEvent&)>
			handler); // Returns whether it was handled
	void onMouseUp(
		std::function<bool(Container&, const MouseEvent&)>
			handler); // Returns whether it was handled

	bool needsLayout() const;
	void invalidate(bool propagate = true);

	virtual void updateLayout(); // Multi pass, call until !needsLayout()
	virtual void update(OopMineGame& game);
	virtual void draw(OopMineGame& game) const;

protected:
	static bool debug;
	void setAbsolutePos(olc::vi2d v);
	void validate();

private:
	static int idCounter;
	static int focusedElementId;
#ifdef _DEBUG
	std::string name = {};
#endif
	int id;
	std::vector<std::unique_ptr<Container>> children = {};
	Anchor anchor = Anchor::topLeft; // Point on parent my origin is attached to
	Anchor origin = Anchor::topLeft; // Point of my origin
	olc::vi2d size = {10, 10};		 // Size of content
	olc::vi2d pos = {};				 // Local offset
	olc::vi2d margin = {};			 // Exterior spacing
	olc::vi2d padding = {};			 // Interior spacing
	olc::vi2d absPos = {};
	Container* parent = nullptr;
	std::string assetName = "";
	bool visible = true;
	bool dirty = true;
	std::function<bool(Container&, const MouseEvent&)> mouseDownHandler = {};
	std::function<bool(Container&, const MouseDragEvent&)> mouseDragHandler =
		{};
	std::function<bool(Container&, const MouseEvent&)> mouseUpHandler = {};
};

template <typename T, typename... Args>
	requires std::derived_from<T, Container>
T* Container::addChild()
{
	return adoptChild(std::make_unique<T>(Props{}));
}

template <typename T, typename... Args>
	requires std::derived_from<T, Container>
T* Container::addChild(Props props, Args&&... args)
{
	return adoptChild(
		std::make_unique<T>(std::move(props), std::forward<Args>(args)...));
}

template <typename T>
	requires std::derived_from<T, Container>
T* Container::adoptChild(std::unique_ptr<T> v)
{
	T* ptr = v.get();
	v->parent = this;
	children.push_back(std::move(v));
	invalidate();
	return ptr;
}
} // namespace gui
