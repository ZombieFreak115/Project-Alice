# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#

cmake_minimum_required (VERSION 3.15)

project (ProjectAlice LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Turn on compile_commands.json export for wider editor/IDE support.
# This option is ignored on anything but Unix makefiles/Ninja.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

list(APPEND ALICE_CORE_SOURCES_LIST
	"src/main.cpp"
	"src/gui/alice_ui.cpp"
	"src/map/map_state.cpp"
	"src/map/map_data_loading.cpp"
	"src/map/map_borders.cpp"
	"src/map/map.cpp"
	"src/graphics/xac.cpp"
)

list(APPEND ALICE_INCREMENTAL_SOURCES_LIST
	${ALICE_CORE_SOURCES_LIST}
	"src/ai/ai.cpp"
	"src/ai/ai_campaign.cpp"
	"src/ai/ai_campaign_values.cpp"
	"src/ai/ai_focuses.cpp"
	"src/ai/ai_influence.cpp"
	"src/ai/ai_alliances.cpp"
	"src/ai/ai_economy.cpp"
	"src/ai/ai_war.cpp"
	"src/common_types/blake2.cpp"
	"src/common_types/date_interface.cpp"
	"src/common_types/prng.cpp"
	"src/culture/culture.cpp"
	"src/culture/politics.cpp"
	"src/culture/rebels.cpp"
	"src/economy/demographics.cpp"
	"src/economy/economy_stats.cpp"
	"src/economy/economy.cpp"
	"src/economy/economy_pops.cpp"
	"src/economy/economy_trade_routes.cpp"
	"src/economy/economy_government.cpp"
	"src/economy/economy_production.cpp"
	"src/economy/construction.cpp"
	"src/economy/advanced_province_buildings.cpp"
	"src/gamestate/commands.cpp"
	"src/gamestate/diplomatic_messages.cpp"
	"src/gamestate/modifiers.cpp"
	"src/gamestate/notifications.cpp"
	"src/gamestate/serialization.cpp"
	"src/graphics/opengl_wrapper.cpp"
	"src/graphics/texture.cpp"
	"src/gui/gui_common_elements.cpp"
	"src/gui/gui_effect_tooltips.cpp"
	"src/gui/widgets/table.cpp"
	"src/gui/gui_graphics.cpp"
	"src/gui/gui_leader_select.cpp"
	"src/gui/gui_leader_tooltip.cpp"
	"src/gui/gui_modifier_tooltips.cpp"
	"src/gui/province_tiles/province_tiles.cpp"
	"src/gui/gui_province_window.cpp"
	"src/gui/gui_trigger_tooltips.cpp"
	"src/gui/gui_error_window.cpp"
	"src/gui/map_tooltip.cpp"
	"src/gui/topbar_subwindows/gui_population_window.cpp"
	"src/gui/topbar_subwindows/gui_production_window.cpp"
	"src/gui/topbar_subwindows/gui_technology_window.cpp"
	"src/gui/immediate_mode.cpp"
	"src/gui/economy_viewer.cpp"
	"src/gui/unit_tooltip.cpp"
	"src/gui/gui_context_window.cpp"
	"src/gui/labour_details.cpp"
	"src/map/map_modes.cpp"
	"src/military/military.cpp"
	"src/nations/nations.cpp"
	"src/network/SHA512.cpp"
	"src/network/network.cpp"
	"src/parsing/float_from_chars.cpp"
	"src/parsing/parsers.cpp"
	"src/platform_specific.cpp"
	"src/provinces/province.cpp"
	"src/scripting/effects.cpp"
	"src/scripting/events.cpp"
	"src/scripting/triggers.cpp"
	"src/scripting/fif_triggers.cpp"
	"src/text/bmfont.cpp"
	"src/text/fonts.cpp"
	"src/text/text.cpp"
	"src/zstd/zstd.cpp"
	"src/network/pcp.cpp"
	"src/gamestate/game_scene.cpp"
)

if(WIN32)
add_executable(Alice WIN32
	${ALICE_CORE_SOURCES_LIST}
	"src/alice.rc"
	${ASSET_FILES})
add_executable(AliceIncremental WIN32 EXCLUDE_FROM_ALL
	${ALICE_INCREMENTAL_SOURCES_LIST}
	"src/alice.rc"
	${ASSET_FILES})
else()
add_executable(Alice WIN32
	${ALICE_CORE_SOURCES_LIST}
	${ASSET_FILES})
add_executable(AliceIncremental WIN32 EXCLUDE_FROM_ALL
	${ALICE_INCREMENTAL_SOURCES_LIST}
	${ASSET_FILES})
endif()

target_compile_definitions(AliceIncremental PRIVATE INCREMENTAL=1)

if(NOT WIN32)
	add_compile_definitions(PREFER_ONE_TBB)
endif()

# Include sub-projects.
add_subdirectory(dependencies EXCLUDE_FROM_ALL)
add_subdirectory(ParserGenerator EXCLUDE_FROM_ALL)
add_subdirectory(FifInterfaceGenerator EXCLUDE_FROM_ALL)

add_library(AliceCommon INTERFACE)
target_compile_definitions(AliceCommon INTERFACE "PROJECT_ROOT=\"${PROJECT_SOURCE_DIR}\"")
if(WIN32)
	# string(REPLACE "/GR" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	# string(REPLACE "/W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	# string(REPLACE "/MDd" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
	# string(REPLACE "/MD" "" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
	# string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	# string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	# string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})

	# To build Non-AVX version : change all "/arch:AVX2" to "/d2archSSE42"
	# -march=nehalem for clang release
	set(CMAKE_CXX_FLAGS "")
	set(CMAKE_CXX_FLAGS_DEBUG "")
	set(CMAKE_CXX_FLAGS_RELEASE "")
	if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		if(OPTIMIZE_MODE STREQUAL "On")
			message(STATUS "Optimizing with PGO data")
			target_compile_options(AliceCommon INTERFACE
				/bigobj /wd4100 /wd4189 /wd4065 /wd4201 /wd4324 /GR- /W4 /permissive- /WX /arch:AVX2 /GF /w34388 /w34389 -Wno-unused-macros -Wno-zero-length-array -Wno-switch-default -Wno-reserved-identifier -Wno-unused-parameter -Wno-unused-variable -Wno-unused-private-field /Z7 -Wno-invalid-offsetof -Wno-deprecated-volatile -Wno-missing-prototypes -Wno-reserved-identifier -Wno-implicit-int-float-conversion -Wno-unsafe-buffer-usage -Wno-float-equal -Wno-shadow-field-in-constructor -Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-double-promotion -Wno-shadow-uncaptured-local -Wno-nonportable-system-include-path -Wno-format-nonliteral -Wno-shorten-64-to-32 -Wno-ctad-maybe-unsupported -Wno-implicit-int-conversion -Wno-disabled-macro-expansion -Wno-comma -Wno-cast-function-type-strict -Wno-cast-function-type -Wno-invalid-offsetof -Wno-microsoft-cast -Wno-reserved-identifier -Wno-unused-but-set-variable /DNDEBUG /wd4530 /MT /O2 /Oi /sdl- /GS- /Gy /Gw /Zc:inline -Wno-profile-instr-missing -Wno-profile-instr-out-of-date -fprofile-instr-use=code.profdata)
			target_link_options(AliceCommon INTERFACE /OPT:REF /OPT:ICF /LTCG -fprofile-instr-use=code.profdata)
		elseif(PROFILE_MODE STREQUAL "On")
			message(STATUS "Compiling for PGO instrumentation")
			target_compile_options(AliceCommon INTERFACE
				/bigobj /wd4100 /wd4189 /wd4065 /wd4201 /wd4324 /GR- /W4 /permissive- /WX /arch:AVX2 /GF /w34388 /w34389 -Wno-unused-macros -Wno-zero-length-array -Wno-switch-default -Wno-reserved-identifier -Wno-unused-parameter -Wno-unused-variable -Wno-unused-private-field /Z7 -Wno-invalid-offsetof -Wno-deprecated-volatile -Wno-missing-prototypes -Wno-reserved-identifier -Wno-implicit-int-float-conversion -Wno-unsafe-buffer-usage -Wno-float-equal -Wno-shadow-field-in-constructor -Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-double-promotion -Wno-shadow-uncaptured-local -Wno-nonportable-system-include-path -Wno-format-nonliteral -Wno-shorten-64-to-32 -Wno-ctad-maybe-unsupported -Wno-implicit-int-conversion -Wno-disabled-macro-expansion -Wno-comma -Wno-cast-function-type-strict -Wno-cast-function-type -Wno-invalid-offsetof -Wno-microsoft-cast -Wno-reserved-identifier -Wno-unused-but-set-variable /DNDEBUG /wd4530 /MT /O2 /Oi /sdl- /GS- /Gy /Gw /Zc:inline -fprofile-instr-generate)
			target_link_options(AliceCommon INTERFACE /DEBUG:FULL /OPT:REF /OPT:ICF /LTCG -fprofile-instr-generate)
		else()
			target_compile_options(AliceCommon INTERFACE
											/bigobj /wd4100 /wd4189 /wd4065 /wd4201 /wd4324 /GR- /W4 /permissive- /WX /GF /w34388 /w34389 -Wno-unused-macros -Wno-unused-parameter -Wno-unused-variable -Wno-unused-private-field /Z7 -Wno-invalid-offsetof -Wno-deprecated-volatile -Wno-missing-prototypes -Wno-reserved-identifier -Wno-implicit-int-float-conversion -Wno-unsafe-buffer-usage -Wno-float-equal -Wno-shadow-field-in-constructor -Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-double-promotion -Wno-shadow-uncaptured-local -Wno-nonportable-system-include-path -Wno-format-nonliteral -Wno-shorten-64-to-32 -Wno-ctad-maybe-unsupported -Wno-implicit-int-conversion -Wno-disabled-macro-expansion -Wno-comma -Wno-cast-function-type-strict -Wno-cast-function-type -Wno-invalid-offsetof -Wno-switch-default -Wno-zero-length-array -Wno-microsoft-cast -Wno-reserved-identifier -Wno-unused-but-set-variable /arch:AVX2
				$<$<CONFIG:Debug>:			/RTC1 /EHsc /MTd /Od>
				$<$<NOT:$<CONFIG:Debug>>: 	/DNDEBUG /wd4530 /MT /O2 /Oi /sdl- /GS- /Gy /Gw /Zc:inline>)
			target_link_options(AliceCommon INTERFACE
				$<$<CONFIG:Debug>: 			/DEBUG:FULL >
				$<$<NOT:$<CONFIG:Debug>>: 	/OPT:REF /OPT:ICF /LTCG>)
		endif()
	else()
		set_target_properties(AliceCommon PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR})

		if(OPTIMIZE_MODE STREQUAL "On")
			message(STATUS "Optimizing with PGO data")
			target_compile_options(AliceCommon INTERFACE
				/bigobj /wd4100 /wd4189 /wd4065 /wd4201 /wd4324 /wd4723 /GR- /W4 /permissive- /Zc:preprocessor /WX /d2archSSE42 /GF /w34388 /w34389 /DNDEBUG /wd4530 /MT /O2 /Oi /GL /sdl- /GS- /Gy /Gw /Zc:preprocessor /Zc:inline)
			target_link_options(AliceCommon INTERFACE /OPT:REF /OPT:ICF /LTCG /USEPROFILE)
		elseif(PROFILE_MODE STREQUAL "On")
			message(STATUS "Compiling for PGO instrumentation")
			target_compile_options(AliceCommon INTERFACE
				/bigobj /wd4100 /wd4189 /wd4065 /wd4201 /wd4324 /wd4723 /GR- /W4 /permissive- /Zc:preprocessor /WX /d2archSSE42 /GF /w34388 /w34389 /Z7 /DNDEBUG /wd4530 /MT /O2 /Oi /GL /sdl- /GS- /Gy /Gw /Zc:preprocessor /Zc:inline)
			target_link_options(AliceCommon INTERFACE /DEBUG:FULL /OPT:REF /OPT:ICF /LTCG /GENPROFILE)
		else()
			message(STATUS "Normal, not PGO, build")
			target_compile_options(AliceCommon INTERFACE
				/bigobj /wd4100 /wd4189 /wd4065 /wd4201 /wd4324 /GR- /W4 /permissive- /Zc:preprocessor /WX /d2archSSE42 /GF /w34388 /w34389 /Z7
				  $<$<CONFIG:Debug>: /EHsc /MTd /RTC1 /Od>
# for faster debug builds, replace /RTC1 /Od with /O1 -- should be able to use /Ox, but for some reason that crashes ZSTD
				  $<$<NOT:$<CONFIG:Debug>>: 	/DNDEBUG /wd4530 /MT /O2 /Oi /GL /sdl- /GS- /Gy /Gw /Zc:preprocessor /Zc:inline>)
			target_link_options(AliceCommon INTERFACE
				$<$<CONFIG:Debug>: 			/DEBUG:FULL >
				$<$<NOT:$<CONFIG:Debug>>: 	/DEBUG:FULL /OPT:REF /OPT:ICF /LTCG>)
		endif()
	endif()
else() # GCC or CLANG
	# See for FMA support: https://stackoverflow.com/questions/21001388/fma3-in-gcc-how-to-enable
	if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		target_compile_options(AliceCommon INTERFACE
			-Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-unused-variable -Wno-switch -Wdangling-else -Wno-unused-private-field -Wno-invalid-offsetof -Wno-unused-but-set-variable -Wno-microsoft-cast -std=c++20 #-Werror
			$<$<CONFIG:Debug>:			-mfpmath=sse -mavx2 -mfma -g -pipe>
			$<$<NOT:$<CONFIG:Debug>>: 	-mfpmath=sse -mavx2 -mfma -O3 -pipe -fno-rtti -DNDEBUG>)
	else()
		target_compile_options(AliceCommon INTERFACE
										# -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-switch -Wno-unused-private-field -Wno-unused-but-set-variable -Wno-parentheses
			$<$<CONFIG:Debug>:			-mfpmath=sse -mavx2 -mfma -g -pipe>
			$<$<NOT:$<CONFIG:Debug>>: 	-mfpmath=sse -mavx2 -mfma -O3 -pipe -fno-rtti -DNDEBUG>)
	endif()
endif()
target_link_libraries(AliceCommon INTERFACE dependency_DataContainer)
target_link_libraries(AliceCommon INTERFACE libglew_static)
target_link_libraries(AliceCommon INTERFACE stb_image)
target_link_libraries(AliceCommon INTERFACE freetype)
target_link_libraries(AliceCommon INTERFACE harfbuzz)
target_link_libraries(AliceCommon INTERFACE glm)
if (WIN32)
	target_link_libraries(AliceCommon INTERFACE ws2_32)
	target_link_libraries(AliceCommon INTERFACE wsock32)
else()
	target_link_libraries(AliceCommon INTERFACE dependency_tbb)
	target_link_libraries(AliceCommon INTERFACE glfw)
	target_link_libraries(AliceCommon INTERFACE miniaudio)
	target_link_libraries(AliceCommon INTERFACE dependency_icu)
endif()
target_include_directories(AliceCommon INTERFACE
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/src/ai
	${PROJECT_SOURCE_DIR}/src/common_types
	${PROJECT_SOURCE_DIR}/src/filesystem
	${PROJECT_SOURCE_DIR}/src/gamestate
	${PROJECT_SOURCE_DIR}/src/gui
	${PROJECT_SOURCE_DIR}/src/gui/province_tiles
	${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows
	${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows/diplomacy_subwindows
	${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows/production_subwindows
	${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows/politics_subwindows
	${PROJECT_SOURCE_DIR}/src/gui/topbar_subwindows/military_subwindows
	${PROJECT_SOURCE_DIR}/src/graphics
	${PROJECT_SOURCE_DIR}/src/parsing
	${PROJECT_SOURCE_DIR}/src/window
	${PROJECT_SOURCE_DIR}/src/text
	${PROJECT_SOURCE_DIR}/src/sound
	${PROJECT_SOURCE_DIR}/src/map
	${PROJECT_SOURCE_DIR}/src/network
	${PROJECT_SOURCE_DIR}/src/nations
	${PROJECT_SOURCE_DIR}/src/provinces
	${PROJECT_SOURCE_DIR}/src/economy
	${PROJECT_SOURCE_DIR}/src/culture
	${PROJECT_SOURCE_DIR}/src/military
	${PROJECT_SOURCE_DIR}/src/scripting
	${PROJECT_SOURCE_DIR}/src/zstd
	"${glew_SOURCE_DIR}/include/GL"
	"ankerl")

target_link_libraries(Alice PRIVATE AliceCommon)
target_link_libraries(AliceIncremental PRIVATE AliceCommon)

if (WIN32)
	target_link_libraries(Alice PRIVATE ${PROJECT_SOURCE_DIR}/libs/LLVM-C.lib)
	target_link_libraries(AliceIncremental PRIVATE ${PROJECT_SOURCE_DIR}/libs/LLVM-C.lib)
else()
	target_link_libraries(Alice PRIVATE fmt::fmt)
	target_link_libraries(AliceIncremental PRIVATE fmt::fmt)
endif()

# System headers
target_precompile_headers(Alice
	PRIVATE <stddef.h>
	PRIVATE <stdint.h>
	PRIVATE <assert.h>
	PRIVATE <stdarg.h>
	PRIVATE <stdlib.h>
	PRIVATE <math.h>
	PRIVATE <signal.h>
	PRIVATE <ctype.h>
)
target_precompile_headers(AliceIncremental
	PRIVATE <stddef.h>
	PRIVATE <stdint.h>
	PRIVATE <assert.h>
	PRIVATE <stdarg.h>
	PRIVATE <stdlib.h>
	PRIVATE <math.h>
	PRIVATE <signal.h>
	PRIVATE <ctype.h>
)
# Standard C++ library headers
target_precompile_headers(Alice
	PRIVATE <vector>
	PRIVATE <array>
	PRIVATE <optional>
	PRIVATE <memory>
	PRIVATE <variant>
	PRIVATE <string>
	PRIVATE <string_view>
	PRIVATE <charconv>
	PRIVATE <algorithm>
	PRIVATE <iterator>
	PRIVATE <functional>
	PRIVATE <atomic>
	PRIVATE <chrono>
	PRIVATE <tuple>
	PRIVATE <type_traits>
	PRIVATE <new>
	PRIVATE <limits>
	PRIVATE <iterator>
)
target_precompile_headers(AliceIncremental
	PRIVATE <vector>
	PRIVATE <array>
	PRIVATE <optional>
	PRIVATE <memory>
	PRIVATE <variant>
	PRIVATE <string>
	PRIVATE <string_view>
	PRIVATE <charconv>
	PRIVATE <algorithm>
	PRIVATE <iterator>
	PRIVATE <functional>
	PRIVATE <atomic>
	PRIVATE <chrono>
	PRIVATE <tuple>
	PRIVATE <type_traits>
	PRIVATE <new>
	PRIVATE <limits>
	PRIVATE <iterator>
)
# Aliases for C library
target_precompile_headers(Alice
	PRIVATE <cstdlib>
	PRIVATE <cstddef>
	PRIVATE <cstdint>
	PRIVATE <cassert>
	PRIVATE <cstdarg>
	PRIVATE <cmath>
)
target_precompile_headers(AliceIncremental
	PRIVATE <cstdlib>
	PRIVATE <cstddef>
	PRIVATE <cstdint>
	PRIVATE <cassert>
	PRIVATE <cstdarg>
	PRIVATE <cmath>
)
# Alice provided headers
target_precompile_headers(Alice
	PRIVATE [["simple_fs.hpp"]]
	PRIVATE [["constants.hpp"]]
	PRIVATE [["unordered_dense.h"]]
	PRIVATE [["dcon_generated.hpp"]]
	PRIVATE [["container_types.hpp"]]
	PRIVATE [["parsers_declarations.hpp"]]
	PRIVATE [["cyto_any.hpp"]]
	PRIVATE [["date_interface.hpp"]]
	PRIVATE [["stb_image.h"]]
)
target_precompile_headers(AliceIncremental
	PRIVATE [["simple_fs.hpp"]]
	PRIVATE [["constants.hpp"]]
	PRIVATE [["unordered_dense.h"]]
	PRIVATE [["dcon_generated.hpp"]]
	PRIVATE [["container_types.hpp"]]
	PRIVATE [["parsers_declarations.hpp"]]
	PRIVATE [["cyto_any.hpp"]]
	PRIVATE [["date_interface.hpp"]]
	PRIVATE [["stb_image.h"]]
)

target_precompile_headers(AliceIncremental
	PRIVATE [["system_state.hpp"]]
	)

if(WIN32)
else()
	target_precompile_headers(Alice
		PRIVATE [["miniaudio.h"]])
	target_precompile_headers(AliceIncremental
		PRIVATE [["miniaudio.h"]])
endif()

# GENERATE CONTAINER
set(CONTAINER_PATH ${PROJECT_SOURCE_DIR}/src/gamestate/dcon_generated)

# The command to build the generated file
add_custom_command(
	OUTPUT ${CONTAINER_PATH}.hpp
	COMMAND DCONGENERATOR ${CONTAINER_PATH}.txt
	DEPENDS ${CONTAINER_PATH}.txt
	VERBATIM)

add_custom_command(
	OUTPUT ${PROJECT_SOURCE_DIR}/src/gamestate/fif_dcon_generated.hpp
	COMMAND DCONINTERFACEGEN ${CONTAINER_PATH}.txt
	DEPENDS ${CONTAINER_PATH}.txt
	VERBATIM)

# Sets a dependency on the generated file
add_custom_target(GENERATE_CONTAINER DEPENDS ${CONTAINER_PATH}.hpp)
add_dependencies(Alice GENERATE_CONTAINER ParserGenerator)
add_dependencies(AliceIncremental GENERATE_CONTAINER ParserGenerator)

add_custom_target(GENERATE_CONTAINERIFACE DEPENDS ${PROJECT_SOURCE_DIR}/src/gamestate/fif_dcon_generated.hpp)
add_dependencies(Alice GENERATE_CONTAINERIFACE)
add_dependencies(AliceIncremental GENERATE_CONTAINERIFACE)

# The command to build the generated parsers file
add_custom_command(
	OUTPUT ${PROJECT_SOURCE_DIR}/src/parsing/parser_defs_generated.hpp
	COMMAND ParserGenerator ${PROJECT_SOURCE_DIR}/src/parsing/parser_defs.txt
	DEPENDS ${PROJECT_SOURCE_DIR}/src/parsing/parser_defs.txt
	VERBATIM)

add_custom_command(
	OUTPUT ${PROJECT_SOURCE_DIR}/src/parsing/tutorial_parser_defs_generated.hpp
	COMMAND ParserGenerator ${PROJECT_SOURCE_DIR}/src/parsing/tutorial_parser_defs.txt
	DEPENDS ${PROJECT_SOURCE_DIR}/src/parsing/tutorial_parser_defs.txt
	VERBATIM)

add_custom_command(
	OUTPUT ${PROJECT_SOURCE_DIR}/src/parsing/gui_parser_defs_generated.hpp
	COMMAND ParserGenerator ${PROJECT_SOURCE_DIR}/src/parsing/gui_parser_defs.txt
	DEPENDS ${PROJECT_SOURCE_DIR}/src/parsing/gui_parser_defs.txt
	VERBATIM)

add_custom_command(
	OUTPUT ${PROJECT_SOURCE_DIR}/src/parsing/save_parser_defs_generated.hpp
	COMMAND ParserGenerator ${PROJECT_SOURCE_DIR}/src/parsing/save_parser_defs.txt
	DEPENDS ${PROJECT_SOURCE_DIR}/src/parsing/save_parser_defs.txt
	VERBATIM)

add_custom_command(
	OUTPUT ${PROJECT_SOURCE_DIR}/src/parsing/news_parser_defs_generated.hpp
	COMMAND ParserGenerator ${PROJECT_SOURCE_DIR}/src/parsing/news_parser_defs.txt
	DEPENDS ${PROJECT_SOURCE_DIR}/src/parsing/news_parser_defs.txt
	VERBATIM)

add_custom_command(
	OUTPUT ${PROJECT_SOURCE_DIR}/src/parsing/effect_parser_defs_generated.hpp
	COMMAND ParserGenerator ${PROJECT_SOURCE_DIR}/src/parsing/effect_parser_defs.txt
	DEPENDS ${PROJECT_SOURCE_DIR}/src/parsing/effect_parser_defs.txt
	VERBATIM)

add_custom_command(
	OUTPUT ${PROJECT_SOURCE_DIR}/src/parsing/trigger_parser_defs_generated.hpp
	COMMAND ParserGenerator ${PROJECT_SOURCE_DIR}/src/parsing/trigger_parser_defs.txt
	DEPENDS ${PROJECT_SOURCE_DIR}/src/parsing/trigger_parser_defs.txt
	VERBATIM)

add_custom_command(
  OUTPUT ${PROJECT_SOURCE_DIR}/src/text/font_defs_generated.hpp
  COMMAND ParserGenerator ${PROJECT_SOURCE_DIR}/src/text/font_defs.txt
  DEPENDS ${PROJECT_SOURCE_DIR}/src/text/font_defs.txt
  VERBATIM)

add_custom_command(
  OUTPUT ${PROJECT_SOURCE_DIR}/src/text/bmfont_defs_generated.hpp
  COMMAND ParserGenerator ${PROJECT_SOURCE_DIR}/src/text/bmfont_defs.txt
  DEPENDS ${PROJECT_SOURCE_DIR}/src/text/bmfont_defs.txt
  VERBATIM)

# Sets a dependency on the generated file
add_custom_target(GENERATE_PARSERS DEPENDS
	${PROJECT_SOURCE_DIR}/src/parsing/parser_defs_generated.hpp
	${PROJECT_SOURCE_DIR}/src/parsing/save_parser_defs_generated.hpp
	${PROJECT_SOURCE_DIR}/src/parsing/tutorial_parser_defs_generated.hpp
	${PROJECT_SOURCE_DIR}/src/parsing/gui_parser_defs_generated.hpp
	${PROJECT_SOURCE_DIR}/src/parsing/news_parser_defs_generated.hpp
	${PROJECT_SOURCE_DIR}/src/parsing/effect_parser_defs_generated.hpp
	${PROJECT_SOURCE_DIR}/src/parsing/trigger_parser_defs_generated.hpp
	${PROJECT_SOURCE_DIR}/src/text/bmfont_defs_generated.hpp
	${PROJECT_SOURCE_DIR}/src/text/font_defs_generated.hpp)
add_dependencies(Alice GENERATE_PARSERS)
add_dependencies(AliceIncremental GENERATE_PARSERS)

if (BUILD_TESTING)
    enable_testing()
	add_subdirectory(tests EXCLUDE_FROM_ALL)
endif()

if(WIN32)
	add_subdirectory(SaveEditor EXCLUDE_FROM_ALL)
	add_subdirectory(DbgAlice EXCLUDE_FROM_ALL)
endif()
add_subdirectory(Launcher)

# Installation
if(NOT WIN32)
	install(TARGETS Alice launch_alice
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
    install(FILES ${PROJECT_SOURCE_DIR}/src/alice.desktop DESTINATION /usr/share/applications)
    install(FILES ${PROJECT_SOURCE_DIR}/src/alice.png DESTINATION /usr/share/icons/hicolor/128x128/apps)
endif()

