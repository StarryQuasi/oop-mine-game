#include <filesystem>
#include <print>

#if defined(_WIN32)
#define NOMINMAX
#include <Windows.h>
#elif defined(__APPLE__)
#include <unistd.h>
#endif

#define OLC_IMAGE_STB
#define OLC_PGE_APPLICATION
#include <libs/olcPixelGameEngine.h>

#define OLC_PGEX_TRANSFORMEDVIEW
#include <libs/olcPGEX_TransformedView.h>

#include "OopMineGame.h"

// Sets the current working directory to the exe's directory,
// so the game can find the asset archive
void setCwd()
{
#if defined(_WIN32)
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(nullptr, path, MAX_PATH);
	std::filesystem::path exePath(path);
	std::filesystem::current_path(exePath.parent_path());
#elif defined(__APPLE__)
	char path[PATH_MAX];
	// This no work?
	ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
	if (count > 0)
	{
		std::filesystem::path exePath(std::string(path, count));
		std::filesystem::current_path(exePath.parent_path());
	}
#else
	throw std::runtime_error("setCwd() not implemented");
#endif
}

int main()
{
	try
	{
		setCwd();
		OopMineGame game;
		OopMineGame::instance = &game;
		const olc::vi2d windowSize = olc::vi2d{768, 432} * 2;
		const olc::vi2d pixelSize = {4, 4};
		const int pixelScale = 1;
		// const int pixelScale = 2;
		const olc::vi2d screenSize = windowSize / pixelSize;
		const bool fullScreen = false;
		// const bool fullScreen = true;
		if (game.Construct(
				screenSize.x,
				screenSize.y,
				pixelSize.x * pixelScale,
				pixelSize.y * pixelScale,
				fullScreen))
			game.Start();
	}
	catch (const std::exception& e)
	{
		std::println("Unhandled exception: {}", e.what());
		return -1;
	}

	return 0;
}

#if defined(_WIN32)
// Entry point for Windows app
int WINAPI wWinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	return main();
}
#endif
