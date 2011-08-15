CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(otclient)

# setup custom cmake modules path
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

OPTION(USE_PCH "Use precompiled header" ON)

# find needed packages
SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREADED OFF)
FIND_PACKAGE(Boost COMPONENTS system REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
FIND_PACKAGE(Lua REQUIRED)
FIND_PACKAGE(PhysFS REQUIRED)
FIND_PACKAGE(GMP REQUIRED)
FIND_PACKAGE(ZLIB REQUIRED)
FIND_PACKAGE(PCHSupport REQUIRED)

# choose a default build type if not specified
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Debug)
ENDIF(NOT CMAKE_BUILD_TYPE)
MESSAGE(STATUS "BUILD TYPE: " ${CMAKE_BUILD_TYPE})

# setup compiler options
IF(CMAKE_COMPILER_IS_GNUCXX)
    SET(CXX_WARNS "-Wall -Wextra -Werror -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-unused-result")
    SET(CMAKE_CXX_FLAGS "-std=c++0x -pipe ${CXX_WARNS}")
    SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -ggdb3 -fno-inline")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
    SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O1 -g -fno-inline")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)


INCLUDE_DIRECTORIES(
    ${Boost_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${LUA_INCLUDE_DIR}
    ${PHYSFS_INCLUDE_DIR}
    ${GMP_INCLUDE_DIR}
    ${ZLIB_INCLUDE_DIR}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/framework"
)

IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
    ADD_DEFINITIONS(-D_DEBUG)
ENDIF(CMAKE_BUILD_TYPE STREQUAL "Debug")

SET(SOURCES
    src/main.cpp
    src/otclient.cpp
    src/otclientluafunctions.cpp

    src/game.cpp
    src/map.cpp
    src/protocollogin.cpp
    src/protocolgame.cpp
    src/protocolgamesend.cpp
    src/protocolgameparse.cpp
    src/tibiadat.cpp
    src/tibiaspr.cpp
    src/item.cpp
    src/tile.cpp
    src/thing.cpp
    src/creature.cpp
    src/effect.cpp

    # framework third party
    src/framework/thirdparty/apngloader.cpp

    # framework net
    src/framework/net/connection.cpp
    src/framework/net/inputmessage.cpp
    src/framework/net/outputmessage.cpp
    src/framework/net/protocol.cpp
    src/framework/net/rsa.cpp

    # framework util
    src/framework/util/logger.cpp
    src/framework/util/color.cpp
    src/framework/util/translator.cpp

    # framework core
    src/framework/core/configmanager.cpp
    src/framework/core/resourcemanager.cpp
    src/framework/core/eventdispatcher.cpp
    src/framework/core/modulemanager.cpp
    src/framework/core/module.cpp

    # framework graphics
    src/framework/graphics/font.cpp
    src/framework/graphics/fontmanager.cpp
    src/framework/graphics/graphics.cpp
    src/framework/graphics/texture.cpp
    src/framework/graphics/framebuffer.cpp
    src/framework/graphics/animatedtexture.cpp
    src/framework/graphics/framebuffer.cpp
    src/framework/graphics/texturemanager.cpp
    src/framework/graphics/borderimage.cpp
    src/framework/graphics/image.cpp

    # framework otml
    src/framework/otml/otmldocument.cpp
    src/framework/otml/otmlemitter.cpp
    src/framework/otml/otmlexception.cpp
    src/framework/otml/otmlnode.cpp
    src/framework/otml/otmlparser.cpp

    # framework luascript
    src/framework/luascript/luainterface.cpp
    src/framework/luascript/luaobject.cpp
    src/framework/luascript/luaexception.cpp
    src/framework/luascript/luafunctions.cpp

    # framework ui
    src/framework/ui/uimanager.cpp
    src/framework/ui/uiwidget.cpp
    src/framework/ui/uilabel.cpp
    src/framework/ui/uibutton.cpp
    src/framework/ui/uilineedit.cpp
    src/framework/ui/uiwindow.cpp
    src/framework/ui/uianchorlayout.cpp
    src/framework/ui/uianchor.cpp
)

IF(WIN32)
    SET(SOURCES ${SOURCES} src/framework/core/win32platform.cpp)
    SET(ADDITIONAL_LIBRARIES ws2_32)
    IF(CMAKE_COMPILER_IS_GNUCXX)
        SET(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -mwindows")
        ADD_DEFINITIONS(-D_WIN32_WINNT=0x0501)
    ENDIF(CMAKE_COMPILER_IS_GNUCXX)
    ADD_DEFINITIONS(-DWIN32_NO_CONSOLE)
ELSE(WIN32)
    SET(SOURCES ${SOURCES} src/framework/core/x11platform.cpp)
ENDIF(WIN32)

# target executable
ADD_EXECUTABLE(otclient ${SOURCES})

# target link libraries
TARGET_LINK_LIBRARIES(
    otclient
    ${Boost_LIBRARIES}
    ${OPENGL_LIBRARIES}
    ${LUA_LIBRARIES}
    ${PHYSFS_LIBRARY}
    ${GMP_LIBRARY}
    ${ZLIB_LIBRARY}
    ${ADDITIONAL_LIBRARIES}
)

IF(USE_PCH)
    ADD_PRECOMPILED_HEADER(otclient ${CMAKE_CURRENT_SOURCE_DIR}/src/framework/pch.h)
ENDIF(USE_PCH)

# installation
SET(DATA_INSTALL_DIR share/otclient)
INSTALL(TARGETS otclient RUNTIME DESTINATION bin)
INSTALL(DIRECTORY modules DESTINATION ${DATA_INSTALL_DIR})

