
project(otclient)

# *****************************************************************************
# Options
# *****************************************************************************
option(TOGGLE_FRAMEWORK_SOUND "Use SOUND " ON)
option(TOGGLE_FRAMEWORK_XML "Use XML " ON)
option(TOGGLE_FRAMEWORK_NET "Use NET " ON)
option(TOGGLE_DIRECTX "Use DX9 support" OFF)
option(TOGGLE_BIN_FOLDER "Use build/bin folder for generate compilation files" OFF)
option(TOGGLE_BOT_PROTECTION "Use bot protection" ON)
option(TOGGLE_PRE_COMPILED_HEADER "Use precompiled header (speed up compile)" OFF)
option(DEBUG_LOG "Enable Debug Log" OFF)
option(ASAN_ENABLED "Build this target with AddressSanitizer" OFF)

# *****************************************************************************
# Cmake Features
# *****************************************************************************
set(GNUCXX_MINIMUM_VERSION 9)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


# Make will print more details
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# *****************************************************************************
# Options Code
# *****************************************************************************

# Define Framework options for use in compilation
set(FRAMEWORK_DEFINITIONS "")
if (TOGGLE_FRAMEWORK_SOUND)
	set(FRAMEWORK_DEFINITIONS ${FRAMEWORK_DEFINITIONS} -DFRAMEWORK_SOUND)
endif()
if (TOGGLE_FRAMEWORK_XML)
	set(FRAMEWORK_DEFINITIONS ${FRAMEWORK_DEFINITIONS} -DFRAMEWORK_XML)
endif()
if (TOGGLE_FRAMEWORK_NET)
	set(FRAMEWORK_DEFINITIONS ${FRAMEWORK_DEFINITIONS} -DFRAMEWORK_NET)
endif()

# Set for use bot protection
if(TOGGLE_BOT_PROTECTION)
	message(STATUS "Bot protection: ON")
else(TOGGLE_BOT_PROTECTION)
	message(STATUS "Bot protection: OFF")
endif(TOGGLE_BOT_PROTECTION)


# Set for use precompiled header
if(TOGGLE_PRE_COMPILED_HEADER)
	include(cotire)
	cotire(${PROJECT_NAME})
	message(STATUS "Use precompiled header: ON")
else()
	message(STATUS "Use precompiled header: OFF")
endif(TOGGLE_PRE_COMPILED_HEADER)


set(VERSION "1.0.0")

# *****************************************************************************
# Client
# *****************************************************************************
if (MSVC)
	add_executable(${PROJECT_NAME} "" ../cmake/icon/otcicon.rc)
else()
	add_executable(${PROJECT_NAME} "")
endif()

# *****************************************************************************
# Build flags
# *****************************************************************************
if (NOT MSVC)
	if (CMAKE_COMPILER_IS_GNUCXX)
		target_compile_options(${PROJECT_NAME}  PRIVATE  -Wno-deprecated-declarations)
	endif()
endif()

if(THREADS_HAVE_PTHREAD_ARG)
	target_compile_options(${PROJECT_NAME} PUBLIC "-pthread")
endif()

# *****************************************************************************
# Definitions code
# *****************************************************************************


# === ASAN ===
if(ASAN_ENABLED)
  log_option_enabled("asan")

  if(MSVC)
	target_compile_options(${PROJECT_NAME} PUBLIC /fsanitize=address)
  else()
	target_compile_options(${PROJECT_NAME} PUBLIC -fsanitize=address)
	target_link_options(${PROJECT_NAME} PUBLIC -fsanitize=address)
  endif()
else()
  log_option_disabled("asan")
endif()


# === DEBUG LOG ===
# cmake -DDEBUG_LOG=ON ..
if(CMAKE_BUILD_TYPE MATCHES Debug)
  target_compile_definitions(${PROJECT_NAME} PRIVATE -DDEBUG_LOG=ON )
  log_option_enabled("DEBUG LOG")
  else()
  log_option_disabled("DEBUG LOG")
endif(CMAKE_BUILD_TYPE MATCHES Debug)

# *****************************************************************************
# Sanity Check
# *****************************************************************************

# === GCC Minimum Version ===
if (CMAKE_COMPILER_IS_GNUCXX)
	if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS GNUCXX_MINIMUM_VERSION)
		message(FATAL_ERROR "GCC version must be at least ${GNUCXX_MINIMUM_VERSION}!")
	endif()
endif()


# Multiprocessor compiller
if(CMake_MSVC_PARALLEL)
	if(CMake_MSVC_PARALLEL GREATER 0)
		add_definitions(/MP${CMake_MSVC_PARALLEL})
	else()
		add_definitions(/MP)
	endif()
endif()


# Client options
add_definitions(-DCLIENT)
# Set bot protection
if(TOGGLE_BOT_PROTECTION)
	add_definitions(-DBOT_PROTECTION)
endif(TOGGLE_BOT_PROTECTION)
add_definitions(-D_WIN32_WINNT=0x0501)
add_definitions(${FRAMEWORK_DEFINITIONS})
add_definitions(-D"VERSION=${VERSION}")

# === Build options ===
message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})
add_definitions(-D"BUILD_TYPE=\\\"${CMAKE_BUILD_TYPE}\\\"")
message(STATUS "Build commit: ${BUILD_COMMIT}")
add_definitions(-D"BUILD_COMMIT=\\\"${BUILD_COMMIT}\\\"")
message(STATUS "Build revision: ${BUILD_REVISION}")
add_definitions(-D"BUILD_REVISION=\\\"${BUILD_REVISION}\\\"")

# *****************************************************************************
# Packages / Libs
# *****************************************************************************
find_package(OpenSSL QUIET)
find_package(PhysFS REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Protobuf REQUIRED)
find_package(LibLZMA REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(asio REQUIRED)
find_package(Threads REQUIRED)
# OpenGL/GLEW = Graphics
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
if(UNIX)
	find_package(X11 REQUIRED)
endif()
find_package(LuaJIT REQUIRED)
if(WIN32)
	find_package(DbgHelp REQUIRED)
endif(WIN32)
if(NOT OPENSSL_FOUND)
	find_package(GMP REQUIRED)
endif()
if(TOGGLE_DIRECTX)
	find_package(DirectX REQUIRED)
endif()
if(TOGGLE_FRAMEWORK_SOUND)
	find_package(OpenAL CONFIG REQUIRED)
	find_package(VorbisFile REQUIRED)
	find_package(Vorbis CONFIG REQUIRED)
	find_package(Ogg CONFIG REQUIRED)
endif()

# *****************************************************************************
# OTClient source files configuration
# *****************************************************************************
target_sources(${PROJECT_NAME}
	PRIVATE
	client/animatedtext.cpp
	client/animator.cpp
	client/spriteappearances.cpp
	client/client.cpp
	client/container.cpp
	client/creature.cpp
	client/creatures.cpp
	client/effect.cpp
	client/game.cpp
	client/houses.cpp
	client/item.cpp
	client/itemtype.cpp
	client/lightview.cpp
	client/localplayer.cpp
	client/luafunctions.cpp
	client/luavaluecasts.cpp
	client/map.cpp
	client/mapio.cpp
	client/mapview.cpp
	client/minimap.cpp
	client/missile.cpp
	client/outfit.cpp
	client/player.cpp
	client/protocolcodes.cpp
	client/protocolgame.cpp
	client/protocolgameparse.cpp
	client/protocolgamesend.cpp
	client/shadermanager.cpp
	client/spritemanager.cpp
	client/statictext.cpp
	client/thing.cpp
	client/thingtype.cpp
	client/thingtypemanager.cpp
	client/tile.cpp
	client/towns.cpp
	client/uicreature.cpp
	client/uiitem.cpp
	client/uimap.cpp
	client/uimapanchorlayout.cpp
	client/uiminimap.cpp
	client/uiprogressrect.cpp
	client/uisprite.cpp
	framework/core/adaptativeframecounter.cpp
	framework/core/application.cpp
	framework/core/asyncdispatcher.cpp
	framework/core/binarytree.cpp
	framework/core/clock.cpp
	framework/core/config.cpp
	framework/core/configmanager.cpp
	framework/core/event.cpp
	framework/core/eventdispatcher.cpp
	framework/core/filestream.cpp
	framework/core/graphicalapplication.cpp
	framework/core/logger.cpp
	framework/core/module.cpp
	framework/core/modulemanager.cpp
	framework/core/resourcemanager.cpp
	framework/core/scheduledevent.cpp
	framework/core/timer.cpp
	framework/graphics/animatedtexture.cpp
	framework/graphics/apngloader.cpp
	framework/graphics/bitmapfont.cpp
	framework/graphics/cachedtext.cpp
	framework/graphics/coordsbuffer.cpp
	framework/graphics/drawpool.cpp
	framework/graphics/fontmanager.cpp
	framework/graphics/framebuffer.cpp
	framework/graphics/framebuffermanager.cpp
	framework/graphics/graphics.cpp
	framework/graphics/image.cpp
	framework/graphics/ogl/painterogl.cpp
	framework/graphics/ogl/painterogl1.cpp
	framework/graphics/ogl/painterogl2.cpp
	framework/graphics/painter.cpp
	framework/graphics/paintershaderprogram.cpp
	framework/graphics/particle.cpp
	framework/graphics/particleaffector.cpp
	framework/graphics/particleeffect.cpp
	framework/graphics/particleemitter.cpp
	framework/graphics/particlemanager.cpp
	framework/graphics/particlesystem.cpp
	framework/graphics/particletype.cpp
	framework/graphics/pool.cpp
	framework/graphics/shader.cpp
	framework/graphics/shaderprogram.cpp
	framework/graphics/texture.cpp
	framework/graphics/texturemanager.cpp
	framework/input/mouse.cpp
	framework/luaengine/luaexception.cpp
	framework/luaengine/luainterface.cpp
	framework/luaengine/luaobject.cpp
	framework/luaengine/luavaluecasts.cpp
	framework/luafunctions.cpp
	framework/net/connection.cpp
	framework/net/inputmessage.cpp
	framework/net/outputmessage.cpp
	framework/net/protocol.cpp
	framework/net/protocolhttp.cpp
	framework/net/server.cpp
	framework/otml//otmldocument.cpp
	framework/otml//otmlemitter.cpp
	framework/otml//otmlexception.cpp
	framework/otml//otmlnode.cpp
	framework/otml//otmlparser.cpp
	framework/platform/platform.cpp
	framework/platform/platformwindow.cpp
	framework/platform/unixcrashhandler.cpp
	framework/platform/unixplatform.cpp
	framework/platform/win32crashhandler.cpp
	framework/platform/win32platform.cpp
	framework/platform/win32window.cpp
	framework/platform/x11window.cpp
	framework/sound/combinedsoundsource.cpp
	framework/sound/oggsoundfile.cpp
	framework/sound/soundbuffer.cpp
	framework/sound/soundchannel.cpp
	framework/sound/soundfile.cpp
	framework/sound/soundmanager.cpp
	framework/sound/soundsource.cpp
	framework/sound/streamsoundsource.cpp
	framework/stdext/demangle.cpp
	framework/stdext/math.cpp
	framework/stdext/net.cpp
	framework/stdext/string.cpp
	framework/stdext/time.cpp
	framework/ui/uianchorlayout.cpp
	framework/ui/uiboxlayout.cpp
	framework/ui/uigridlayout.cpp
	framework/ui/uihorizontallayout.cpp
	framework/ui/uilayout.cpp
	framework/ui/uimanager.cpp
	framework/ui/uiparticles.cpp
	framework/ui/uitextedit.cpp
	framework/ui/uitranslator.cpp
	framework/ui/uiverticallayout.cpp
	framework/ui/uiwidget.cpp
	framework/ui/uiwidgetbasestyle.cpp
	framework/ui/uiwidgetimage.cpp
	framework/ui/uiwidgettext.cpp
	framework/util/color.cpp
	framework/util/crypt.cpp
	framework/xml/tinystr.cpp
	framework/xml/tinyxml.cpp
	framework/xml/tinyxmlerror.cpp
	framework/xml/tinyxmlparser.cpp
	protobuf/appearances.pb.cc
	main.cpp
)

# *****************************************************************************
# Includes and librarys
# *****************************************************************************
if(MSVC)

	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
		string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
	elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
		string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
		string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
	elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
		string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
		string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
	endif()

	target_compile_options(${PROJECT_NAME} PUBLIC /MP /FS /Zf )

	target_include_directories(${PROJECT_NAME}
		PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${MYSQL_INCLUDE_DIR}
		${LUAJIT_INCLUDE_DIR}
		${Protobuf_INCLUDE_DIRS}
		${VORBISFILE_INCLUDE_DIR}
		${MYSQL_INCLUDE_DIR}
		${GMP_INCLUDE_DIR}
		${OPENSSL_INCLUDE_DIR}
		${PHYSFS_INCLUDE_DIR}
		${NLOHMANN_JSON_INCLUDE_DIR}
	)
	target_link_libraries(${PROJECT_NAME}
		PRIVATE
		${MYSQL_CLIENT_LIBS}
		${LUAJIT_LIBRARY}
		${CMAKE_THREAD_LIBS_INIT}
		${PHYSFS_LIBRARY}
		${ZLIB_LIBRARY}
		${PROTOBUF_LIBRARY}
		${NLOHMANN_JSON_LIBRARY}
		${OPENGL_LIBRARIES}
		${DirectX_LIBRARY}
		${DirectX_LIBRARIES}
		${VORBISFILE_LIBRARY}
		${VORBIS_LIBRARY}
		${OGG_LIBRARY}
		${MYSQL_LIBRARY}
		${GMP_LIBRARY}
		${OPENSSL_LIBRARIES}
		${DBGHELP_LIBRARY}

		Threads::Threads
		GLEW::GLEW
		asio::asio
		OpenAL::OpenAL
		LibLZMA::LibLZMA
	)
else()
	target_include_directories(${PROJECT_NAME}
		PRIVATE
		${CMAKE_SOURCE_DIR}/src
		${LUAJIT_INCLUDE_DIR}
		${CMAKE_THREAD_LIBS_INIT}
		${Protobuf_INCLUDE_DIRS}
		${MYSQL_INCLUDE_DIR}
		${GMP_INCLUDE_DIR}
		${OPENSSL_INCLUDE_DIR}
		${PHYSFS_INCLUDE_DIR}
		${GLEW_INCLUDE_DIR}
		${NLOHMANN_JSON_INCLUDE_DIR}
	)
	target_link_libraries(${PROJECT_NAME}
		PRIVATE
		${LUAJIT_LIBRARY}
		${PHYSFS_LIBRARY}
		${ZLIB_LIBRARY}
		${PROTOBUF_LIBRARY}
		${NLOHMANN_JSON_LIBRARY}
		${GLEW_LIBRARY}
		${OPENGL_LIBRARIES}
		${DirectX_LIBRARY}
		${DirectX_LIBRARIES}
		${OGG_LIBRARY}
		${MYSQL_LIBRARY}
		${GMP_LIBRARY}
		${OPENSSL_LIBRARIES}

		Threads::Threads
		X11::X11
		asio::asio
		OpenAL::OpenAL
		Vorbis::vorbis
		Vorbis::vorbisfile
		LibLZMA::LibLZMA
	)
endif()

# *****************************************************************************
# Enable otclient console only for debug build
# *****************************************************************************
if(WIN32)
	set_target_properties(${PROJECT_NAME}
	PROPERTIES
		LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
		LINK_FLAGS_RELEASE "/SUBSYSTEM:windows /ENTRY:mainCRTStartup"
		LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:windows /ENTRY:mainCRTStartup"
		LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:windows /ENTRY:mainCRTStartup"
	)
endif(WIN32)

# *****************************************************************************
# Link compilation files folder
# *****************************************************************************

## Link compilation files to build/bin folder, else link to the main dir
if (TOGGLE_BIN_FOLDER)
	set_target_properties(${PROJECT_NAME}
		PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
	)
else()
	set_target_properties(${PROJECT_NAME}
	PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/"
	)
endif()
