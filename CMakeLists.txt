cmake_minimum_required(VERSION 3.20)

if(APPLE)
	include(FindOpenGL)
endif()

project(OopMineGame VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(OopMineGame)

target_include_directories(OopMineGame PRIVATE
	libs
	libs/miniz-3.1.1
	src
	src/gui
)

target_sources(OopMineGame PRIVATE
	libs/miniz-3.1.1/miniz.c
	src/Block.cpp
	src/Blocks.cpp
	src/Entity.cpp
	src/Item.cpp
	src/Items.cpp
	src/ItemStack.cpp
	src/main.cpp
	src/OopMineGame.cpp
	src/Player.cpp
	src/Sheep.cpp
	src/Transform.cpp
	src/World.cpp
	src/gui/Anchor.cpp
	src/gui/Button.cpp
	src/gui/Container.cpp
	src/gui/Debugger.cpp
	src/gui/Direction.cpp
	src/gui/FlowContainer.cpp
	src/gui/Hotbar.cpp
	src/gui/Slider.cpp
	src/gui/Slot.cpp
	src/gui/TextContainer.cpp
	src/gui/WorldSetting.cpp
)

# Enable hardening so it caches std::views::iota(max, min)
if (CMAKE_CXX_COMPILER_ID STREQUAL Clang OR CMAKE_CXX_COMPILER_ID STREQUAL AppleClang)
	target_compile_definitions(OopMineGame PRIVATE
		$<$<CONFIG:Debug>:_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_DEBUG>
	)
endif()

if(APPLE)
	find_package(OpenGL REQUIRED)
	find_library(LIB_FOUNDATION Foundation REQUIRED)
	find_library(LIB_GLUT GLUT REQUIRED)

	target_link_libraries(OopMineGame PRIVATE
		OpenGL::GL
		${LIB_FOUNDATION}
		${LIB_GLUT}
	)
endif()

add_custom_target(CopyAsset
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/assets.zip"
        "$<TARGET_FILE_DIR:OopMineGame>/"
    COMMENT "Copying asset.zip"
)

add_dependencies(OopMineGame CopyAsset)