#pragma once

#include <print>
#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include <deque>

#include <olcPixelGameEngine.h>
#include <olcPGEX_TransformedView.h>
#include <miniz.h>

#include "World.h"
#include "Transform.h"
#include "gui/Gui.h"

class OopMineGame : public olc::PixelGameEngine
{
public:
	static std::string debugMsg;
	static std::vector<std::function<void(OopMineGame&)>> debugCallbacks;
	static OopMineGame* instance;

	OopMineGame();
	bool OnUserCreate() override;
	bool OnUserDestroy() override;
	bool OnUserUpdate(float elapsed) override;
	olc::TileTransformedView& getView();
	std::optional<olc::Renderable>& getAsset(const std::string& path);
	std::optional<olc::Renderable>& getBlockAsset(int id);
	std::optional<olc::DecalPatch>& getBlockAssetPatch(int id);
	// Schedules a function that modifies critical data, to be run on the next update
	void schedule(std::function<void()> func);

private:
	std::vector<uint8_t> assetsBuffer = {};
	mz_zip_archive assetsZip = {};
	std::unordered_map<std::string, std::optional<olc::Renderable>> assetsOther = {};
	std::vector<std::optional<olc::Renderable>> assetsBlock = {};
	std::vector<std::optional<olc::DecalPatch>> assetsBlockPatch = {};
	std::deque<std::function<void()>> scheduledFunctions = {};
	olc::TileTransformedView view = {};
	olc::vf2d cameraPos = {};
	std::unique_ptr<World> world = nullptr;
	uint32_t layerMain = 0;
	std::vector<Transform> transforms;
	int hotbarSelection = 0;
	std::unique_ptr<gui::Container> guiRoot = nullptr;
	gui::FlowContainer* guiHotbar = nullptr;
	std::array<gui::Slot*, 9> guiHotbarSlots = {};
	std::array<gui::Container*, 9> guiHotbarSelections = {};
	gui::TextContainer* guiHotbarText = nullptr;
	bool inspectorEnabled = false;
	int inspectorId = 0;
	bool freecamEnabled = false;

	static std::vector<uint8_t> readFileData(const std::filesystem::path& path);
	static std::vector<uint8_t> readFileFromZip(mz_zip_archive& zip, const std::string& path);
	static std::optional<olc::Renderable> loadAsset(mz_zip_archive& zip, const std::string& path);
	void genNewWorld(const GenerationSettings&);
	void drawDebugUi();
	void emitUiEvents();
};
