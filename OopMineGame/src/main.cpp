#include <filesystem>
#include <print>

#define NOMINMAX
#include <Windows.h>

#define OLC_IMAGE_STB
#define OLC_PGE_APPLICATION
#include <olcPixelGameEngine.h>

#define OLC_PGEX_TRANSFORMEDVIEW
#include <olcPGEX_TransformedView.h>

#include "OopMineGame.h"

int main()
{
	// Set the current working directory to the exe's directory,
	// so the game can find the asset archive
	{
		wchar_t path[MAX_PATH];
		GetModuleFileNameW(nullptr, path, MAX_PATH);
		std::filesystem::path exePath(path);
		std::filesystem::current_path(exePath.parent_path());
	}

	try
	{
		OopMineGame game;
		OopMineGame::instance = &game;
		const olc::vi2d windowSize = { 768, 432 };
		const olc::vi2d pixelSize = { 4, 4 };
		const int pixelScale = 1;
		//const int pixelScale = 2;
		const olc::vi2d screenSize = windowSize / pixelSize;
		if (game.Construct(screenSize.x, screenSize.y, pixelSize.x * pixelScale, pixelSize.y * pixelScale))
			game.Start();
	}
	catch (const std::exception& e)
	{
		std::println("Unhandled exception: {}", e.what());
		return -1;
	}

	return 0;
}

// Entry point for Windows app
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	return main();
}
