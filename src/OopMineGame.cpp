#include <print>

#include "libs/miniz-3.1.1/miniz.h"
#include "libs/stb_image.h"

#include "Blocks.h"
#include "Entity.h"
#include "OopMineGame.h"
#include "Player.h"
#include "Sheep.h"
#include "Utils.h"
#include "Verify.h"
#include "gui/Anchor.h"
#include "gui/Button.h"
#include "gui/Debugger.h"
#include "gui/WorldSetting.h"

bool OopMineGame::debugGui = false;
bool OopMineGame::debugEntity = false;
std::string OopMineGame::debugMsg = "";
std::vector<std::function<void(OopMineGame&)>> OopMineGame::debugCallbacks = {};
OopMineGame* OopMineGame::instance = nullptr;

OopMineGame::OopMineGame() { sAppName = "OopMineGame"; }

std::vector<uint8_t>
OopMineGame::readFileData(const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path))
	{
		std::println("Failed to read file \"{}\"", path.string());
		return {};
	}
	const auto size = std::filesystem::file_size(path);
	if (size == 0)
	{
		std::println("Failed to read file \"{}\"", path.string());
		return {};
	}
	std::vector<uint8_t> buffer(size);
	std::ifstream fs(path, std::ios_base::binary);
	fs.read((char*)(buffer.data()), size);
	fs.close();
	return buffer;
}

void OopMineGame::printZipInfo(mz_zip_archive& zip)
{
	for (int i = 0; i < zip.m_total_files; i++)
	{
		mz_zip_archive_file_stat stat;
		if (!mz_zip_reader_file_stat(&zip, i, &stat))
		{
			std::println("File {}: mz_zip_reader_file_stat() failed", i + 1);
			continue;
		}
		if (stat.m_is_directory)
			std::println("Dir  {}: {}", i + 1, stat.m_filename);
		else
			std::println("File {}: {}", i + 1, stat.m_filename);
	}
}

std::vector<uint8_t>
OopMineGame::readFileFromZip(mz_zip_archive& zip, const std::string& path)
{
	const int index = mz_zip_reader_locate_file(&zip, path.c_str(), nullptr, 0);
	if (index < 0)
		goto fail;
	mz_zip_archive_file_stat stat;
	if (!mz_zip_reader_file_stat(&zip, index, &stat))
		goto fail;
	{
		std::vector<uint8_t> buffer(stat.m_uncomp_size);
		if (!mz_zip_reader_extract_to_mem(
				&zip, index, buffer.data(), buffer.size(), 0))
			goto fail;
		return buffer;
	}
fail:
	std::println("Failed to read file \"{}\" from zip", path);
	return {};
}

std::optional<olc::Renderable>
OopMineGame::loadAsset(mz_zip_archive& zip, const std::string& path)
{
	const auto data = readFileFromZip(zip, path);
	if (data.empty())
		goto fail;
	{
		int w = 0;
		int h = 0;
		int channels = 0;
		const uint8_t* pixels = stbi_load_from_memory(
			data.data(), (int)data.size(), &w, &h, &channels, 4);
		if (!(w && h && channels && pixels))
			goto fail;
		std::println(
			"Loaded asset \"{}\" ({}x{}, {} channels)", path, w, h, channels);
		olc::Renderable renderable;
		renderable.Create(w, h);
		std::memcpy(
			renderable.Sprite()->pColData.data(), pixels, (size_t)w * h * 4);
		renderable.Decal()->Update();
		std::free((void*)pixels);
		return std::move(renderable);
	}
fail:
	std::println("Failed to load asset \"{}\"", path);
	return {};
}

void OopMineGame::genNewWorld(const GenerationSettings& settings)
{
	world = std::make_unique<World>(settings);
	world->addEntity<Sheep>(world->getPlayer()->get().getPos());
	if (!freecamEnabled)
	{
		transforms.clear();
		transforms.push_back(Transform(
			8.0f,
			8.0f,
			std::chrono::seconds(1),
			Transform::Easing::outQuart,
			[this](float v) { this->view.SetWorldScale({v, v}); }));
	}
}

bool OopMineGame::OnUserCreate()
{
	// Load asset zip and block assets, other assets are read on the go
	assetsBuffer = readFileData("assets.zip");
	if (!mz_zip_reader_init_mem(
			&assetsZip, assetsBuffer.data(), assetsBuffer.size(), 0))
		return false;
	// printZipInfo(assetsZip);
	// Create block atlas
	assetsBlock.resize(Blocks::getAllBlocks().size());
	assetsBlockPatch.resize(Blocks::getAllBlocks().size());
	{
		olc::Renderable& atlas =
			(assetsOther["atlas"] = olc::Renderable()).value();
		atlas.Create(256, 256);
		olc::Sprite* atlasSprite = atlas.Sprite();
		olc::vi2d atlasPos = {};
		int atlasRowHeight = 0;
		for (const auto block : Blocks::getAllBlocks())
		{
			auto asset = loadAsset(
				assetsZip, "block/" + block->getTextureName() + ".png");
			if (asset.has_value())
			{
				olc::Sprite* assetSpr = asset.value().Sprite();
				for (int y = 0; y < assetSpr->Size().y; y++)
				{
					std::memcpy(
						atlasSprite->GetData() + atlasPos.x +
							atlasSprite->Size().x * (atlasPos.y + y),
						assetSpr->GetData() + y * assetSpr->Size().x,
						sizeof(olc::Pixel) * assetSpr->Size().x);
				}
				olc::DecalPatch patch =
					atlas.Decal()->Patch(atlasPos, assetSpr->Size());
				assetsBlockPatch[block->getId()] = std::move(patch);

				atlasPos.x += assetSpr->Size().x;
				atlasRowHeight = std::max(atlasRowHeight, assetSpr->Size().y);
				if (atlasPos.x >= atlasSprite->Size().x)
				{
					atlasPos.x = 0;
					atlasPos.y += atlasRowHeight;
					atlasRowHeight = 0;
				}
				assetsBlock[block->getId()] = std::move(asset.value());
			}
		}
		atlas.Decal()->Update();
	}

	// Transformed view for world
	view = olc::TileTransformedView({ScreenWidth(), ScreenHeight()}, {32, 32});
	// 2nd render layer (below the gui)
	layerMain = CreateLayer();
	EnableLayer(layerMain, true);

	// Create gui
	guiRoot = std::make_unique<gui::Container>(gui::Props{
		.padding = {{1, 1}},
	});
	guiRoot->setSize(GetScreenSize() - guiRoot->getPadding() * 2);
	guiRoot->setDebugName("root");
	guiHotbarText = guiRoot->addChild<gui::TextContainer>({
		.pos = {{0, -25}},
		.anchor = gui::Anchor::btmMiddle,
		.origin = gui::Anchor::btmMiddle,
	});
	guiHotbar = guiRoot->addChild<gui::Hotbar>({
		.anchor = gui::Anchor::btmMiddle,
		.origin = gui::Anchor::btmMiddle,
	});
	{
		auto flow = guiRoot->addChild<gui::FlowContainer>();
		flow->setDirection(gui::Direction::vertical);
		// c->setMargin({ 5, 5 });
		flow->addChild<gui::Button>({.text = "enable debug"})
			->onClick(
				[this](auto& me)
				{
					inspectorEnabled = !inspectorEnabled;
					if (inspectorEnabled)
					{
						static_cast<gui::Button*>(&me)->setText(
							"disable debug");
						inspectorId =
							guiRoot
								->addChild<gui::Debugger>(
									{
										.anchor = gui::Anchor::topRight,
										.origin = gui::Anchor::topRight,
									},
									guiRoot.get())
								->getId();
						gui::Container::setDebug(inspectorEnabled);
					}
					else
					{
						static_cast<gui::Button*>(&me)->setText("enable debug");
						guiRoot->removeChild(inspectorId);
						gui::Container::setDebug(inspectorEnabled);
					}
				});
		flow->addChild<gui::Button>({.text = "enable freecam"})
			->onClick(
				[this](auto& me)
				{
					freecamEnabled = !freecamEnabled;
					if (freecamEnabled)
						static_cast<gui::Button*>(&me)->setText(
							"disable freecam");
					else
						static_cast<gui::Button*>(&me)->setText(
							"enable freecam");
				});
		flow->addChild<gui::WorldSetting>()->onValueChanged(
			[this](const GenerationSettings& v)
			{ this->schedule([this, v]() { this->genNewWorld(v); }); });
	}
	guiDebugText = guiRoot->addChild<gui::TextContainer>({
		.anchor = gui::Anchor::btmRight,
		.origin = gui::Anchor::btmRight,
	});

	// Root element mouse handling
	guiRoot->onMouseDown(
		[this](gui::Container& me, const gui::MouseEvent& e)
		{
			const olc::vf2d targetWorldSpace =
				view.ScreenToWorld(GetMousePos());
			queuedInput.target = targetWorldSpace;
			if (e.btn == olc::Mouse::LEFT)
				queuedInput.attack = true;
			if (e.btn == olc::Mouse::RIGHT)
				queuedInput.use = true;
			return true;
		});
	guiRoot->onMouseUp([this](gui::Container& me, const gui::MouseEvent& e)
					   { return true; });
	guiRoot->onMouseDrag(
		[this](gui::Container& me, const gui::MouseDragEvent& e)
		{ return true; });

	genNewWorld({});

	debugEntity = true;
	return true;
}

bool OopMineGame::OnUserDestroy()
{
	mz_zip_reader_end(&assetsZip);
	return true;
}

bool OopMineGame::OnUserUpdate(float elapsed)
{
	// Clamp it to prevent physics from going insane when debugging or lagged
	elapsed = std::min(elapsed, 1.0f / 20);

	const olc::vi2d pixelSize = GetPixelSize();
	if (GetWindowSize() / pixelSize != GetScreenSize())
	{
		const auto window = GetWindowSize();
		const auto pixel = GetPixelSize();
		const auto screen = GetScreenSize();
		// std::println("window {}x{} pixel {}x{} screen {}x{}", window.x,
		// window.y, pixel.x, pixel.y, screen.x, screen.y);
		SetScreenSize(window.x / pixelSize.x, window.y / pixelSize.y);
		view.SetViewArea(screen);
		guiRoot->setSize(screen - guiRoot->getPadding() * 2);
	}

	if (GetKey(olc::Key::ESCAPE).bPressed)
		return false;

	for (; !scheduledFunctions.empty(); scheduledFunctions.pop_front())
		scheduledFunctions.front()();

	emitUiEvents();
	handleInput(elapsed);

	Player& player = world->getPlayer().value();
	if (!freecamEnabled)
	{
		cameraPos = player.getPos();
		cameraPos.y -= player.getSize().y / 2.0f;
	}
	view.SetWorldOffset(cameraPos - view.ScaleToWorld(GetScreenSize() / 2.0f));

	for (auto& t : transforms)
		t.update();
	std::erase_if(transforms, std::mem_fn(&Transform::isFinished));

	world->update(elapsed);

	guiDebugText->setText(debugMsg);
	debugMsg = "";

	for (int i = 0; i < 9; i++)
		guiHotbar->setStack(i, player.getInvItem(i));
	guiRoot->update(*this);
	while (guiRoot->needsLayout())
		guiRoot->updateLayout();

	// Set layer to 1 below
	SetDrawTarget(layerMain);
	Clear(olc::BLACK);
	// TODO: Make fast clear work with the hair, buffer it onto a decal to
	// maintain the draw order
	// FillRectDecal({}, GetScreenSize(), olc::BLACK);

	world->draw(*this);

	const olc::vf2d targetWorldSpace = view.ScreenToWorld(GetMousePos());
	const float reach = 6;
	std::optional<olc::vi2d> target = {};
	if ((targetWorldSpace - player.getEyePos()).mag2() <= reach * reach)
		target = targetWorldSpace.floor();

	if (target.has_value())
		Utils::drawRectOutline(
			view, *target, *target + olc::vf2d{1.0f, 1.0f}, olc::GREY);

	// Set layer to front most
	SetDrawTarget(nullptr);
	Clear(olc::BLANK);

	guiRoot->draw(*this);
	for (const auto& callback : debugCallbacks)
		callback(*this);

	debugCallbacks.clear();

	return true;
}

olc::TileTransformedView& OopMineGame::getView() { return view; }

std::optional<olc::Renderable>& OopMineGame::getAsset(const std::string& path)
{
	if (const auto it = assetsOther.find(path); it != assetsOther.end())
		return it->second;
	assetsOther[path] = loadAsset(assetsZip, path);
	return assetsOther[path];
}

std::optional<olc::Renderable>& OopMineGame::getBlockAsset(int id)
{
	return assetsBlock[id];
}

std::optional<olc::DecalPatch>& OopMineGame::getBlockAssetPatch(int id)
{
	return assetsBlockPatch[id];
}

void OopMineGame::schedule(std::function<void()> func)
{
	scheduledFunctions.emplace_back(std::move(func));
}

void OopMineGame::emitUiEvents()
{
	static olc::vi2d mouseDragStart = {};
	static olc::vi2d mouseDragStartElePos = {};
	for (const uint32_t btn :
		 {olc::Mouse::LEFT, olc::Mouse::MIDDLE, olc::Mouse::RIGHT})
	{
		gui::MouseEvent e;
		e.globalPos = GetMousePos();
		e.pos = GetMousePos();
		e.btn = btn;
		if (GetMouse(btn).bPressed)
		{
			if (guiRoot->mouseDown(e) && btn == olc::Mouse::LEFT)
			{
				mouseDragStart = e.pos;
				mouseDragStartElePos =
					guiRoot
						->searchElement(gui::Container::getFocusedElementId())
						->getPos();
			}
		}
		if (GetMouse(btn).bReleased)
		{
			if (btn == olc::Mouse::LEFT)
			{
				gui::Container* p = guiRoot->searchElement(
					gui::Container::getFocusedElementId());
				if (p != nullptr)
					p->mouseUp(e);
				gui::Container::setFocusedElementId(-1);
			}
			// else
			//	guiRoot->mouseUp(e);
		}
	}
	if (GetMouse(olc::Mouse::LEFT).bHeld &&
		gui::Container::getFocusedElementId() != -1)
	{
		gui::MouseDragEvent e;
		e.globalPos = GetMousePos();
		e.pos = GetMousePos();
		e.btn = olc::Mouse::LEFT;
		e.globalPosStart = mouseDragStart;
		e.elePosStart = mouseDragStartElePos;
		gui::Container* p =
			guiRoot->searchElement(gui::Container::getFocusedElementId());
		if (p != nullptr)
			p->mouseDrag(e);
		else
			gui::Container::setFocusedElementId(-1);
	}
}

void OopMineGame::setHotbarSelection(int i)
{
	assert(Verify::in(i, -1, 10));
	hotbarSelection = i;
	hotbarSelection = (hotbarSelection + 9) % 9;
	guiHotbar->setSelection(hotbarSelection);
	auto& player = world->getPlayer()->get();
	if (player.getInvItem(hotbarSelection).getItem() != Items::air)
		guiHotbarText->setText(
			player.getInvItem(hotbarSelection).getItem().getName());
	else
		guiHotbarText->setText("");
}

void OopMineGame::handleInput(float elapsed)
{
	Player& player = world->getPlayer().value();
	{
		if (!freecamEnabled)
		{
			Entity::Input& input = queuedInput;
			input.left = GetKey(olc::Key::A).bHeld;
			input.right = GetKey(olc::Key::D).bHeld;
			input.jump = GetKey(olc::Key::SPACE).bHeld;
			input.sprint = GetKey(olc::Key::SHIFT).bHeld;
			input.invSelection = hotbarSelection;
			player.setInput(input);
			queuedInput = {};
		}
		else
		{
			const float speed = 32;
			if (GetKey(olc::Key::W).bHeld)
				cameraPos.y -= speed * elapsed;
			if (GetKey(olc::Key::S).bHeld)
				cameraPos.y += speed * elapsed;
			if (GetKey(olc::Key::A).bHeld)
				cameraPos.x -= speed * elapsed;
			if (GetKey(olc::Key::D).bHeld)
				cameraPos.x += speed * elapsed;
		}

#if defined(__APPLE__)
		const auto zoomOut = olc::Key::O;
		const auto zoomIn = olc::Key::P;
#else
		const auto zoomOut = olc::Key::MINUS;
		const auto zoomIn = olc::Key::EQUALS;
#endif
		if (GetKey(zoomIn).bPressed)
			view.ZoomAtScreenPos(1 / 0.75f, GetScreenSize() / 2);
		if (GetKey(zoomOut).bPressed)
			view.ZoomAtScreenPos(0.75, GetScreenSize() / 2);

		const auto numKeys = {
			olc::Key::K1,
			olc::Key::K2,
			olc::Key::K3,
			olc::Key::K4,
			olc::Key::K5,
			olc::Key::K6,
			olc::Key::K7,
			olc::Key::K8,
			olc::Key::K9,
		};
		int i = 0;
		for (const auto key : numKeys)
		{
			if (GetKey(key).bPressed)
			{
				setHotbarSelection(i);
			}
			i++;
		}

		if (GetMouseWheel())
		{
			setHotbarSelection(
				hotbarSelection + (GetMouseWheel() < 0 ? 1 : -1));
		}
	}
}
