#pragma once

#include <deque>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "libs/miniz-3.1.1/miniz.h"
#include "libs/msf_gif.h"
#include "libs/olcPGEX_TransformedView.h"
#include "libs/olcPixelGameEngine.h"

#include "Transform.h"
#include "World.h"
#include "gui/Container.h"
#include "gui/Hotbar.h"

class OopMineGame : public olc::PixelGameEngine
{
public:
	static bool debugGui;
	static bool debugEntity;
	static std::string debugMsg;
	static std::vector<std::function<void(OopMineGame&)>> debugCallbacks;
	static OopMineGame* instance;

	OopMineGame();
	bool OnUserCreate() override;
	bool OnUserDestroy() override;
	bool OnUserUpdate(float elapsed) override;
	olc::TileTransformedView& getView();
	World& getWorld();
	std::optional<olc::Renderable>& getAsset(const std::string& path);
	std::optional<olc::Renderable>& getBlockAsset(int id);
	std::optional<olc::DecalPatch>& getBlockAssetPatch(int id);
	// Schedules a function that modifies critical data, to be run on the next
	// update
	void schedule(std::function<void()> func);
	template <typename T>
		requires std::derived_from<T, gui::Container>
	void openScreen(std::unique_ptr<T> p);
	void closeScreen();

private:
	std::vector<uint8_t> assetsBuffer = {};
	mz_zip_archive assetsZip = {};
	std::unordered_map<std::string, std::optional<olc::Renderable>>
		assetsOther = {};
	std::vector<std::optional<olc::Renderable>> assetsBlock = {};
	std::vector<std::optional<olc::DecalPatch>> assetsBlockPatch = {};
	std::deque<std::function<void()>> scheduledFunctions = {};
	olc::TileTransformedView view = {};
	olc::vf2d cameraPos = {};
	std::unique_ptr<World> world = nullptr;
	uint32_t layerMain = 0;
	std::vector<Transform> transforms;
	Bindable<int> hotbarSelection{};
	std::unique_ptr<gui::Container> guiRoot = nullptr;
	gui::Hotbar* guiHotbar = nullptr;
	gui::TextContainer* guiHotbarText = nullptr;
	gui::TextContainer* guiDebugText = nullptr;
	gui::Container* guiOverlay = nullptr;
	bool inspectorEnabled = false;
	int inspectorId = 0;
	bool freecamEnabled = false;
	Entity::Input queuedInput = {};
	std::optional<MsfGifState> gifState{};

	static std::vector<uint8_t> readFileData(const std::filesystem::path& path);
	static void printZipInfo(mz_zip_archive& zip);
	static std::vector<uint8_t>
	readFileFromZip(mz_zip_archive& zip, const std::string& path);
	static std::optional<olc::Renderable>
	loadAsset(mz_zip_archive& zip, const std::string& path);
	void genNewWorld(const GenerationSettings&);
	void emitUiEvents();
	void setHotbarSelection(int i);
	void handleInput(float elapsed);
};

template <typename T>
	requires std::derived_from<T, gui::Container>
void OopMineGame::openScreen(std::unique_ptr<T> p)
{
	if (guiOverlay != nullptr)
	{
		guiRoot->removeChild(guiOverlay->getId());
		guiOverlay = nullptr;
	}
	guiOverlay = guiRoot->adoptChild(std::move(p));
}
