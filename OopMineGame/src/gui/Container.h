#ifndef GUI_CONTAINER_H
#define GUI_CONTAINER_H

#include <vector>
#include <memory>

#include "Anchor.h"
#include "Events.h"

class OopMineGame;

namespace gui
{
	// The base gui element that hosts children, can be positioned relatively to its parent, and handles basic hit testing.
	// Also equipped with fluent interface:
	//   Set position with `setPos(olc::vi2d)`.
	//   Set origin on parent with `setAnchor(gui::Anchor)`
	//     and self with `setOrigin(gui::Anchor)`.
	//   Add child to it with `addChild<gui::Container>()`.
	class Container
	{
	public:
		Container(olc::vi2d s = olc::vi2d{ 10,10 });
		virtual ~Container() = default;

		const std::string& getDebugName() const;
		int getId() const;
		olc::vi2d getSize() const;
		olc::vi2d getDrawSize() const;
		olc::vi2d getPos() const;
		Anchor getAnchor() const;
		Anchor getOrigin() const;
		olc::vi2d getMargin() const;
		olc::vi2d getPadding() const;
		bool isVisible() const;
		Container* getParent() const;
		olc::vi2d getAbsolutePos() const;

		Container* setDebugName(std::string v);
		Container* setSize(olc::vi2d v);
		Container* setPos(olc::vi2d v);
		Container* setAnchor(Anchor v);
		Container* setOrigin(Anchor v);
		Container* setMargin(olc::vi2d v);
		Container* setPadding(olc::vi2d v);
		Container* setAssetName(std::string v);
		Container* setVisible(bool v);

		template<typename T, typename... Args> requires std::derived_from<T, Container>
		T* addChild(Args&&... args);
		template<typename T> requires std::derived_from<T, Container>
		T* addChild(std::unique_ptr<T> v);
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

		bool mouseDown(const MouseEvent& e); // Returns whether it was handled
		bool mouseDrag(const MouseDragEvent& e); // Returns whether it was handled
		bool mouseUp(const MouseEvent& e); // Returns whether it was handled
		void onMouseDown(std::function<bool(Container&, const MouseEvent&)> handler); // Returns whether it was handled
		void onMouseDrag(std::function<bool(Container&, const MouseDragEvent&)> handler); // Returns whether it was handled
		void onMouseUp(std::function<bool(Container&, const MouseEvent&)> handler); // Returns whether it was handled

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
		Anchor anchor = Anchor::topLeft;     // Point on parent my origin is attached to
		Anchor origin = Anchor::topLeft;	   // Point of my origin
		olc::vi2d size = { 10,10 };    // Size of content
		olc::vi2d pos = {};     // Local offset
		olc::vi2d margin = {};  // Exterior spacing
		olc::vi2d padding = {}; // Interior spacing
		olc::vi2d absPos = {};
		Container* parent = nullptr;
		std::string assetName = "";
		bool visible = true;
		bool dirty = true;
		std::function<bool(Container&, const MouseEvent&)> mouseDownHandler = {};
		std::function<bool(Container&, const MouseDragEvent&)> mouseDragHandler = {};
		std::function<bool(Container&, const MouseEvent&)> mouseUpHandler = {};
	};

	template<typename T, typename... Args> requires std::derived_from<T, Container>
	T* Container::addChild(Args&&... args)
	{
		return addChild(std::make_unique<T>(std::forward<Args>(args)...));
	}

	template<typename T> requires std::derived_from<T, Container>
	T* Container::addChild(std::unique_ptr<T> v)
	{
		T* ptr = v.get();
		v->parent = this;
		children.push_back(std::move(v));
		invalidate();
		return ptr;
	}
}

#endif
